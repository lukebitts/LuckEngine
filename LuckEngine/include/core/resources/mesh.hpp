#ifndef MESH_HPP
#define MESH_HPP

#include <array>
#include "../opengl.hpp"
#include "../common/aabb.hpp"

namespace luck
{
	///@todo write a mesh.cpp file for the implementation
	///@todo Allow vertex editing/etc
	struct mesh
	{
		public:
			std::vector<mesh_data_resource::vertex> vertices;
			std::vector<std::vector<GLuint>> submeshes;

			enum buffer_type { VERTEX = 0, ELEMENT = 1 };
			GLuint buffers[ELEMENT + 1];// {0, 0};
			std::vector<GLuint> index_list;
			std::vector<GLsizei> element_number;
			
			glm::aabb aabb;

			inline mesh() {}
			///@todo bring this back later:
			/*inline mesh(unsigned vertex_number, GLfloat* vertex_list, unsigned element_number, GLuint* element_list) : element_number(element_number)
			{
				glGenBuffers(2, &buffers[0]);

				glBindBuffer(GL_ARRAY_BUFFER, buffers[(int)VERTEX]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * vertex_number, vertex_list, GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[(int)ELEMENT]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*element_number, element_list, GL_STATIC_DRAW);
			}*/
			inline mesh(std::vector<mesh_data_resource::vertex> vertex_list, std::vector<std::vector<GLuint>> element_list) //: element_number((GLuint)element_list.size())
			{
				glGenBuffers(2, &buffers[0]);

				glBindBuffer(GL_ARRAY_BUFFER, buffers[(int)VERTEX]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_data_resource::vertex)*vertex_list.size(), &vertex_list[0], GL_STATIC_DRAW);

				for(size_t i = 0; i < element_list.size(); ++i)
				{
					index_list.push_back(0);
					element_number.push_back(0);
					
					glGenBuffers(1, &index_list[i]); ///@todo delete these buffers
					element_number[i] = static_cast<GLsizei>(element_list[i].size());
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_list[i]);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*element_list[i].size(), &element_list[i][0], GL_STATIC_DRAW);
				}

				vertices = std::move(vertex_list);
				element_list = std::move(element_list);

				calculate_aabb();
			}
			inline mesh(const resource_handle<mesh_data_resource>& resource)
				: mesh(resource.get().vertices, resource.get().submeshes)
			{
				//aabb = resource.get().aabb;
			}
			void mesh::calculate_aabb()
			{
				ASSERT(vertices.size() > 0, "You can't calculate the aabb of an empty mesh.'");

				glm::vec3 min{ vertices[0].x, vertices[0].y, vertices[0].z };
				glm::vec3 max{ min };

				for (mesh_data_resource::vertex v : vertices)
				{
					min = glm::min(min, glm::vec3{ v.x, v.y, v.z });
					max = glm::max(max, glm::vec3{ v.x, v.y, v.z });
				}

				aabb = glm::aabb{ min, max };
			}
			inline mesh& operator=(mesh && other)
			{
				/*buffers[0] = other.buffers[0];
				buffers[1] = other.buffers[1];
				element_number = other.element_number;

				other.buffers[0] = 0;
				other.buffers[1] = 0;
				other.element_number = 0;*/

				return *this;
			}
			inline mesh(mesh && other)
			{
				/*buffers[0] = other.buffers[0];
				buffers[1] = other.buffers[1];
				element_number = other.element_number;

				other.buffers[0] = 0;
				other.buffers[1] = 0;
				other.element_number = 0;*/
			}
			mesh& operator=(const mesh&) = delete;
			mesh(const mesh&) = delete;
			inline ~mesh()
			{
				glDeleteBuffers(2, &buffers[0]);
			}
	};

}

#endif // _MESH_HPP_
