#include <core/opengl.hpp>

#include <core/systems/renderable_system.hpp>
#include <core/common/glm.hpp>
#include <core/screen.hpp>

#include <core/systems/spatial_system.hpp>
#include <core/systems/camera_system.hpp>
#include <core/components/camera_component.hpp>
#include <core/components/spatial_component.hpp>
#include <core/components/mesh_component.hpp>
#include <core/resources/material.hpp>
#include <core/resources/mesh.hpp>
#include <core/resources/texture.hpp>
#include <core/resources/program.hpp>

namespace luck
{
	renderable_system::renderable_system(spatial_system* s) : Base(luck::component_filter().requires<spatial_component, mesh_component>()), m_spatial_system(s)
	{

	}

	void renderable_system::onEntityAdded(luck::entity& e)
	{
		e.getComponent<spatial_component>().aabb = e.getComponent<mesh_component>().mesh->aabb;
	}

	std::function<bool(const detail::uniform&, const detail::uniform&)>  sort_renderables2(glm::vec3 camera_position, bool transparent)
	{
		return [camera_position, transparent](const detail::uniform & ua, const detail::uniform & ub) -> bool
		{
			auto a = ua.entity;
			auto b = ub.entity;
			if(transparent)
			{
				return glm::distance(camera_position, a.getComponent<spatial_component>().position) > glm::distance(camera_position, b.getComponent<spatial_component>().position);
			}
			return glm::distance(camera_position, a.getComponent<spatial_component>().position) < glm::distance(camera_position, b.getComponent<spatial_component>().position);
		};
	}

	void renderable_system::render(luck::entity current_camera) ///@todo I'm not satisfied with this rendering function
	{
		auto& c_spatial = current_camera.getComponent<spatial_component>();

		glm::mat4 mat_projection = camera_system::calculate_projection(current_camera);
		glm::mat4 mat_view = camera_system::calculate_view(current_camera);

		auto entities = m_spatial_system->m_tree.query_frustum(mat_projection * mat_view);
		entities.erase(std::remove_if(entities.begin(), entities.end(),
		[](const luck::entity& e)
		{
			return !e.hasComponent<mesh_component>();
		}), entities.end());

		LOG("Rendering #", entities.size(), " entities");

		std::vector<detail::renderable_state_reference> solid_renderables;
		std::vector<detail::renderable_state_reference> transparent_renderables;

		for(auto e : entities)
		{
			auto mesh = e.getComponent<luck::mesh_component>().mesh;
			auto materials = e.getComponent<luck::mesh_component>().materials;

			for(size_t i = 0; i < mesh->index_list.size(); ++i)
			{
				try
				{
					auto material = materials.at(i);
					//for(auto pass : material.passes)
					for(size_t j = 0; j < material.passes.size(); ++j)
					{
						auto pass = material.passes[j];
						
						if(pass.state.transparent())
						{
							transparent_renderables.push_back(detail::renderable_state_reference{e,(int)j,(int)i});
						}
						else
						{
							solid_renderables.push_back(detail::renderable_state_reference{e,(int)j,(int)i});
						}
					}
				}
				catch(...)
				{
					//this submesh has no material, so just skip it
				}
			}
		}

		std::sort(solid_renderables.begin(),solid_renderables.end(),
		[](const detail::renderable_state_reference& a, const detail::renderable_state_reference& b){
			return
				std::tie(a.entity.getComponent<mesh_component>().materials[a.submesh].passes[a.pass].program,
						 a.entity.getComponent<mesh_component>().materials[a.submesh].passes[a.pass],
						 a.entity.getComponent<mesh_component>().mesh)
				<
				std::tie(b.entity.getComponent<mesh_component>().materials[b.submesh].passes[a.pass].program,
						 b.entity.getComponent<mesh_component>().materials[b.submesh].passes[a.pass],
						 b.entity.getComponent<mesh_component>().mesh);
		});

		std::sort(transparent_renderables.begin(),transparent_renderables.end(),
				  [](const detail::renderable_state_reference& a, const detail::renderable_state_reference& b){
					  return
					  std::tie(a.entity.getComponent<mesh_component>().materials[a.submesh].passes[a.pass].program,
							   a.entity.getComponent<mesh_component>().materials[a.submesh].passes[a.pass],
							   a.entity.getComponent<mesh_component>().mesh)
					  <
					  std::tie(b.entity.getComponent<mesh_component>().materials[b.submesh].passes[a.pass].program,
							   b.entity.getComponent<mesh_component>().materials[b.submesh].passes[a.pass],
							   b.entity.getComponent<mesh_component>().mesh);
				  });

		auto solids = sort_solids(solid_renderables);
		auto transparents = sort_solids(transparent_renderables);

		for(auto& solid : solids)
		{
			auto program_id = solid.p->id;
			glUseProgram(program_id);
			glUniformMatrix4fv(glGetUniformLocation(program_id, "view"), 1, GL_FALSE, glm::value_ptr(mat_view));
			glUniformMatrix4fv(glGetUniformLocation(program_id, "projection"), 1, GL_FALSE, glm::value_ptr(mat_projection));
			glUniform3fv(glGetUniformLocation(program_id, "camera_position"), 1, glm::value_ptr(c_spatial.position));

			for(auto& state : solid.states)
			{
				glPushAttrib(GL_ENABLE_BIT);
				state.s.set_state();

				for(auto& meshs : state.meshs)
				{
					auto mesh = meshs.m;
					
					glEnableVertexAttribArray(glGetAttribLocation(program_id, "position"));
					glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[luck::mesh::buffer_type::VERTEX]);
					glVertexAttribPointer(glGetAttribLocation(program_id, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(mesh_data_resource::vertex), (void*)offsetof(mesh_data_resource::vertex, x));
					
					glEnableVertexAttribArray(glGetAttribLocation(program_id, "texcoord"));
					glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[luck::mesh::buffer_type::VERTEX]);
					glVertexAttribPointer(glGetAttribLocation(program_id, "texcoord"), 2, GL_FLOAT, GL_FALSE, sizeof(mesh_data_resource::vertex), (void*)offsetof(mesh_data_resource::vertex, u));

					for(auto& uniforms : meshs.uniforms)
					{
						auto e = uniforms.entity;
						
						auto& spatial = e.getComponent<spatial_component>();
						glm::mat4 mat_model(1.f);
						mat_model = glm::translate(mat_model, spatial.position);
						mat_model = mat_model * glm::toMat4(spatial.rotation);
						mat_model = glm::scale(mat_model, spatial.scale);
						
						glUniformMatrix4fv(glGetUniformLocation(program_id, "model"), 1, GL_FALSE, glm::value_ptr(mat_model));
						
						auto gl = GL_TEXTURE0;
						for(auto texture : uniforms.textures)
						{
							glActiveTexture(gl++);
							glBindTexture(GL_TEXTURE_2D, texture.second->id);
						}
						
						for(auto vec2 : uniforms.vec2)
						{
							glUniform2f(glGetUniformLocation(program_id, vec2.first.c_str()), vec2.second.x, vec2.second.y);
						}
						
						glUniform1f(glGetUniformLocation(program_id, "time"), (float)glfwGetTime());
						
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_list[meshs.submesh]);
						glDrawElements(GL_TRIANGLES, mesh->element_number[meshs.submesh], GL_UNSIGNED_INT, (void*)0);

						glUseProgram(0);
						//glDisable(GL_DEPTH_TEST);
						glPushMatrix();
						
						glLoadMatrixf(glm::value_ptr(mat_projection * mat_view * glm::translate(glm::mat4(1.f), spatial.position)));
						
						auto draw_line = [](glm::vec3 from, glm::vec3 to, glm::vec4 color)
						{
							glColor3f(color.x, color.y, color.z);
							glVertex3d(from.x, from.y, from.z);
							glVertex3d(to.x, to.y, to.z);
						};
						
						glm::aabb aabb = spatial.aabb;
						
						//for(int i = 0; i < 2; ++i)
						{
							//glRotatef(i * 180, 0, 1, 0);
							glBegin(GL_LINES);
							{
								draw_line(aabb.min, glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z), glm::vec4(1, 0, 0, 1));
								draw_line(glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z), glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z), glm::vec4(1, 0, 0, 1));
								draw_line(glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z), glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z), glm::vec4(1, 0, 0, 1));
								draw_line(glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z), aabb.min, glm::vec4(1, 0, 0, 1));
								
								draw_line(aabb.min, glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z), glm::vec4(1, 0, 0, 1));
								draw_line(glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z), glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z), glm::vec4(1, 0, 0, 1));
								draw_line(glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z), glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z), glm::vec4(1, 0, 0, 1));
							}
							glEnd();
						}
						glPopMatrix();
						//glEnable(GL_DEPTH_TEST);
						glUseProgram(program_id);
						
					}
				}
				glPopAttrib();
			}
			glUseProgram(0);
		}
		//Transparents
		/*for(auto& solid : transparents)
		{
			auto program_id = solid.p->id;
			glUseProgram(program_id);
			glUniformMatrix4fv(glGetUniformLocation(program_id, "view"), 1, GL_FALSE, glm::value_ptr(mat_view));
			glUniformMatrix4fv(glGetUniformLocation(program_id, "projection"), 1, GL_FALSE, glm::value_ptr(mat_projection));
			glUniform3fv(glGetUniformLocation(program_id, "camera_position"), 1, glm::value_ptr(c_spatial.position));
			
			for(auto& state : solid.states)
			{
				glPushAttrib(GL_ENABLE_BIT);
				state.s.set_state();
				
				for(auto& meshs : state.meshs)
				{
					auto mesh = meshs.m;
					
					glEnableVertexAttribArray(glGetAttribLocation(program_id, "position"));
					glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[luck::mesh::buffer_type::VERTEX]);
					glVertexAttribPointer(glGetAttribLocation(program_id, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(mesh_data_resource::vertex), (void*)offsetof(mesh_data_resource::vertex, x));
					
					glEnableVertexAttribArray(glGetAttribLocation(program_id, "texcoord"));
					glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[luck::mesh::buffer_type::VERTEX]);
					glVertexAttribPointer(glGetAttribLocation(program_id, "texcoord"), 2, GL_FLOAT, GL_FALSE, sizeof(mesh_data_resource::vertex), (void*)offsetof(mesh_data_resource::vertex, u));

					std::sort(meshs.uniforms.begin(), meshs.uniforms.end(), sort_renderables2(c_spatial.position, state.s.transparent()));
					
					for(auto& uniforms : meshs.uniforms)
					{
						auto e = uniforms.entity;
						
						auto& spatial = e.getComponent<spatial_component>();
						glm::mat4 mat_model(1.f);
						mat_model = glm::translate(mat_model, spatial.position);
						mat_model = mat_model * glm::toMat4(spatial.rotation);
						mat_model = glm::scale(mat_model, spatial.scale);
						
						glUniformMatrix4fv(glGetUniformLocation(program_id, "model"), 1, GL_FALSE, glm::value_ptr(mat_model));
						
						auto gl = GL_TEXTURE0;
						for(auto texture : uniforms.textures)
						{
							glActiveTexture(gl++);
							glBindTexture(GL_TEXTURE_2D, texture.second->id);
						}
						
						for(auto vec2 : uniforms.vec2)
						{
							glUniform2f(glGetUniformLocation(program_id, vec2.first.c_str()), vec2.second.x, vec2.second.y);
						}
						
						glUniform1f(glGetUniformLocation(program_id, "time"), glfwGetTime());
						
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_list[meshs.submesh]);
						glDrawElements(GL_TRIANGLES, mesh->element_number[meshs.submesh], GL_UNSIGNED_INT, (void*)0);
					}
				}
				glPopAttrib();
			}
			glUseProgram(0);
		}*/
	}

	std::vector<detail::program> renderable_system::sort_solids(const std::vector<detail::renderable_state_reference>& solid_renderables)
	{
		std::vector<detail::program> solids;
		
		if(solid_renderables.size())
		{
			solids.emplace_back();
			
			auto renderable = solid_renderables[0];
			auto mesh = renderable.entity.getComponent<luck::mesh_component>();
			
			solids.back().p = mesh.materials[renderable.submesh].passes[renderable.pass].program;
			solids.back().states.emplace_back();
			solids.back().states.back().s = mesh.materials[renderable.submesh].passes[renderable.pass].state;
			solids.back().states.back().meshs.emplace_back();
			solids.back().states.back().meshs.back().m = mesh.mesh;
			solids.back().states.back().meshs.back().submesh = renderable.submesh;
			solids.back().states.back().meshs.back().uniforms.emplace_back();
			solids.back().states.back().meshs.back().uniforms.back().entity = renderable.entity;
			solids.back().states.back().meshs.back().uniforms.back().textures = mesh.materials[renderable.submesh].passes[renderable.pass].textures;
			solids.back().states.back().meshs.back().uniforms.back().vec2 = mesh.materials[renderable.submesh].passes[renderable.pass].vec2;
			
			for(size_t i = 1; i < solid_renderables.size(); ++i)
			{
				auto renderable = solid_renderables[i];
				auto mesh = renderable.entity.getComponent<luck::mesh_component>();
				auto pass = mesh.materials[renderable.submesh].passes[renderable.pass];
				auto program = pass.program;
				auto state = pass.state;
				
				if(solids.back().p == program) //same program, check the state
				{
					if(!(solids.back().states.back().s < state) && !(state < solids.back().states.back().s)) //same state, check the mesh
					{
						if(solids.back().states.back().meshs.back().m == mesh.mesh &&
							solids.back().states.back().meshs.back().submesh == renderable.submesh) //same mesh, add uniforms to the list
						{
							solids.back().states.back().meshs.back().uniforms.emplace_back();
							solids.back().states.back().meshs.back().uniforms.back().entity = renderable.entity;
							solids.back().states.back().meshs.back().uniforms.back().textures = pass.textures;
							solids.back().states.back().meshs.back().uniforms.back().vec2 = pass.vec2;
						}
						else //different mesh
						{
							solids.back().states.back().meshs.emplace_back();
							solids.back().states.back().meshs.back().m = mesh.mesh;
							solids.back().states.back().meshs.back().submesh = renderable.submesh;
							solids.back().states.back().meshs.back().uniforms.emplace_back();
							solids.back().states.back().meshs.back().uniforms.back().entity = renderable.entity;
							solids.back().states.back().meshs.back().uniforms.back().textures = pass.textures;
							solids.back().states.back().meshs.back().uniforms.back().vec2 = pass.vec2;
						}
					}
					else //different state
					{
						solids.back().states.emplace_back();
						solids.back().states.back().s = state;
						solids.back().states.back().meshs.emplace_back();
						solids.back().states.back().meshs.back().m = mesh.mesh;
						solids.back().states.back().meshs.back().submesh = renderable.submesh;
						solids.back().states.back().meshs.back().uniforms.emplace_back();
						solids.back().states.back().meshs.back().uniforms.back().entity = renderable.entity;
						solids.back().states.back().meshs.back().uniforms.back().textures = pass.textures;
						solids.back().states.back().meshs.back().uniforms.back().vec2 = pass.vec2;
					}
				}
				else //different program
				{
					solids.emplace_back();
					solids.back().p = program;
					solids.back().states.emplace_back();
					solids.back().states.back().s = state;
					solids.back().states.back().meshs.emplace_back();
					solids.back().states.back().meshs.back().m = mesh.mesh;
					solids.back().states.back().meshs.back().submesh = renderable.submesh;
					solids.back().states.back().meshs.back().uniforms.emplace_back();
					solids.back().states.back().meshs.back().uniforms.back().entity = renderable.entity;
					solids.back().states.back().meshs.back().uniforms.back().textures = pass.textures;
					solids.back().states.back().meshs.back().uniforms.back().vec2 = pass.vec2;
				}
				
			}
		}

		return solids;
	}
}

