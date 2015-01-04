#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>

#include <boost/algorithm/string.hpp>

#include <luck.hpp>
#include <core/screen.hpp>
#include <core/application.hpp>

#include <core/common/glm.hpp>

#include <tools/image_converter.hpp>
#include <tools/mesh_converter.hpp>
#include <core/resources/resources.hpp>
#include <core/resources/resource.hpp>

#include <core/resources/material.hpp>
#include <core/resources/mesh.hpp>
#include <core/resources/shader.hpp>
#include <core/resources/program.hpp>
#include <core/resources/texture.hpp>

#include <core/components/spatial_component.hpp>
#include <core/components/mesh_component.hpp>
#include <core/components/camera_component.hpp>
#include <core/components/shape_component.hpp>
#include <core/components/rigid_body_component.hpp>
#include <core/systems/camera_system.hpp>
#include <core/systems/renderable_system.hpp>
#include <core/systems/spatial_system.hpp>
#include <core/systems/bullet_system.hpp>

#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <extras/kinematic_character_controller.hpp>

namespace luck
{
	struct tps_controller_component : public luck::component<tps_controller_component>
	{
		luck::entity to_follow;
		float height;
		float distance;
	};

	class tps_controller_system : public luck::system<tps_controller_system>
	{
		private:
			bool right_down = false;
			bool left_down = false;
			float scroll_amount = 0.f;
			glm::vec2 last_mouse_pos;
		public:
			tps_controller_system() : Base(luck::component_filter().requires<spatial_component, tps_controller_component>())
			{
				input::on_mouse_down.connect([this](int button, int action){
					if(button == GLFW_MOUSE_BUTTON_RIGHT)
					{
						right_down = (action != 0);
						if(action) last_mouse_pos = input::mouse_pos();
					}
					if (button == GLFW_MOUSE_BUTTON_LEFT)
					{
						left_down = (action != 0);
					}
				});

				input::on_mouse_wheel.connect([this](float x, float y){
					scroll_amount = y;
				});
			}
			void update()
			{
				auto entities = getEntities();

				glm::vec2 distance = last_mouse_pos - input::mouse_pos();

				for(auto e : entities)
				{
					auto& e_spatial = e.getComponent<luck::spatial_component>();
					auto& t_spatial = e.getComponent<luck::tps_controller_component>().to_follow.getComponent<luck::spatial_component>();
					
					e_spatial.position = t_spatial.position;
					e_spatial.position.y += e.getComponent<luck::tps_controller_component>().height;
					e_spatial.position.z += e.getComponent<luck::tps_controller_component>().distance;

					glm::vec2 pos{ t_spatial.position.x, t_spatial.position.z };
					/*pos += last_mouse_pos;
					pos /= 2.f;*/

					/*if (left_down) {
						e.getComponent<luck::tps_controller_component>().height += 0.2f;
					}
					if (right_down) {
						e.getComponent<luck::tps_controller_component>().height -= 0.2f;
					}

					e.getComponent<luck::tps_controller_component>().distance += scroll_amount;*/

					e_spatial.look_at(glm::vec3{ pos.x, t_spatial.position.y, pos.y }); ///@todo copy unity's cool camera :3
				}

				scroll_amount = 0.f;
				last_mouse_pos = input::mouse_pos();
			}
	};

	struct fps_controller_component: public luck::component<fps_controller_component>
	{
		public:
			float move_speed = 0.2f;
			float sensitivity = 10.f;
	};

	class fps_controller_system : public luck::system<fps_controller_system>
	{
		public:
			fps_controller_system() : Base(luck::component_filter().requires<spatial_component, fps_controller_component>())
			{

			}
			void update()
			{
				///@from http://stackoverflow.com/questions/19738805/opengl-camera-control-with-arrow-keys/19739154#19739154

				auto entities = getEntities();

				if(entities.size() == 0) return;

				float frametime = luck::application::delta;

				//int x, y;
				//glfwGetCursorPos(&x, &y);
				glm::vec2 cursor_pos = luck::screen::cursor_pos();
				glm::vec2 screen_size = luck::screen::size();
				
				int x_diff = (int)(cursor_pos.x - screen_size.x / 2);
				int y_diff = (int)(cursor_pos.y - screen_size.y / 2);

				glm::vec3 directions;
				glm::vec2 rotations;

				if(input::key('D')) directions.y = -1;
				if(input::key('A')) directions.y = 1;
				if(input::key('W')) directions.x = 1;
				if(input::key('S')) directions.x = -1;
				if(input::key('E')) directions.z = 1;
				if(input::key('Q')) directions.z = -1;

				for(luck::entity e : entities)
				{
					auto& spatial = e.getComponent<spatial_component>();
					auto& controller = e.getComponent<fps_controller_component>();

					rotations.x = x_diff * controller.sensitivity;
					rotations.y = y_diff * controller.sensitivity;

					auto pitch = glm::quat(glm::vec3(-rotations.y, 0, 0.f));
					auto yaw = glm::quat(glm::vec3(0, -rotations.x, 0.f));

					spatial.rotation = glm::normalize(yaw * spatial.rotation * pitch);

					auto camera_roll_direction = spatial.rotation * glm::vec3(0, 0, -1);
					auto camera_pitch_direction = spatial.rotation * glm::vec3(-1, 0, 0);

					spatial.position += directions[0] * camera_roll_direction * frametime * controller.move_speed;
					spatial.position += directions[1] * camera_pitch_direction * frametime * controller.move_speed;
					spatial.position.y += directions[2] * frametime * controller.move_speed;
				}

				luck::screen::cursor_pos({screen_size.x/2,screen_size.y/2});
			}
			void onEntityAdded(luck::entity&) override
			{

			}
	};

	struct character_component : public luck::component<character_component>
	{
		float speed = 8.5f;
		float time = 0.f;
		std::unique_ptr<KinematicCharacterController> controller;
		character_component()
		{
			
		}
	};
	
	class character_system : public luck::system<character_system>
	{
		btDiscreteDynamicsWorld* m_world = nullptr;
		public:
			character_system(btDiscreteDynamicsWorld* world) :
				Base(luck::component_filter().requires<spatial_component, base_shape_component, /*rigid_body_component,*/ character_component>()),
				m_world(world)
			{

			}
			void update()
			{
				auto entities = getEntities();
				
				int x = luck::input::key('A') ? -1 : luck::input::key('D') ? 1 : 0;
				int z = luck::input::key('W') ? -1 : luck::input::key('S') ? 1 : 0;
				bool jump = luck::input::key(GLFW_KEY_SPACE) == GLFW_PRESS;
				
				for(auto e : entities)
				{
					auto& controller = e.getComponent<character_component>();

					if (x == 0 && z == 0)
					{
						controller.time = 0.f;
					}

					float speed = controller.speed * controller.time * application::delta;
					float div = 1.f;
					if (x && z)
						div = 1.41f;

					controller.controller->setWalkDirection(btVector3(float(x)/div * speed, 0, float(z)/div * speed));

					controller.time = std::min(1.f, controller.time + application::delta * 3.f);

					if(jump)
						controller.controller->jump();
					
					btVector3 position = controller.controller->getGhostObject()->getWorldTransform().getOrigin();
					e.getComponent<spatial_component>().position = glm::vec3(position.x(),position.y(),position.z());
					
					controller.controller->updateAction(m_world, application::delta);
					
					/*auto& body = e.getComponent<rigid_body_component>().rigid_body;
					float y = jump ? 0 : body->getLinearVelocity().y();
					body->setLinearVelocity(btVector3(float(x) * 10, y, float(z) * 10));*/
				}
			}
			virtual void onEntityAdded(luck::entity& e)
			{
				/*auto& body = e.getComponent<rigid_body_component>().rigid_body;
				body->setSleepingThresholds(0.f, 0.f);
				body->setAngularFactor(0.f);*/
				
				character_component& c = e.getComponent<character_component>();
				
				btVector3 start_vector{e.getComponent<spatial_component>().position.x,e.getComponent<spatial_component>().position.y,e.getComponent<spatial_component>().position.z};
				btTransform start_transform;
				start_transform.setIdentity ();
				start_transform.setOrigin(start_vector);
				
				btPairCachingGhostObject* m_ghostObject = new btPairCachingGhostObject();  //ghost Object
				m_ghostObject->setWorldTransform( start_transform );
				m_world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback( new btGhostPairCallback() );
				//m_ghostObject->setUserPointer( (void*)&e );
				m_ghostObject->setCollisionShape( e.getComponent<base_shape_component>().shape());
				m_ghostObject->setCollisionFlags( btCollisionObject::CF_CHARACTER_OBJECT );
				m_ghostObject->setActivationState(DISABLE_DEACTIVATION);
				
				c.controller = std::unique_ptr<KinematicCharacterController>(
					new KinematicCharacterController(m_ghostObject, (btConvexShape*)e.getComponent<base_shape_component>().shape(), 0.5f));
				
				/*c.controller->setGravity(9.8f*0.8f);
				c.controller->setMaxJumpHeight(1.f);
				c.controller->setFallSpeed(5.f);
				c.controller->setJumpSpeed(15.f);*/
				//c.controller->setMaxSlope(btRadians(5.0));
				//c.controller->setUseGhostSweepTest(false);

				m_ghostObject->setUserPointer(new luck::entity(e)); //@todo delete this
				
				m_world->addCollisionObject(m_ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
				m_world->addAction(c.controller.get());
			}
	};

	class test_system : public luck::system<test_system>
	{
		public:
			test_system() :
				Base(luck::component_filter().requires<rigid_body_component>())
			{

			}
			void onEntityAdded(luck::entity& e) override
			{
				auto conn = e.getComponent<rigid_body_component>().on_mouse_down.connect([e](int button){
					LOG(e.getId());
				});

				e.getComponent<rigid_body_component>().on_mouse_down.disconnect(conn); //@todo find out why this doesn't work
			}
	};

	/*luck::bullet_system* physics;

	bool raycast(glm::vec3 origin, glm::vec3 direction, luck::raycast_hit& hit_info, float distance = std::numeric_limits<float>::max()) 
	{
		//direction = glm::min(direction*distance,glm::vec3(INFINITY));

		direction *= 1000.f;

		btCollisionWorld::ClosestRayResultCallback RayCallback(
			btVector3(origin.x, origin.y, origin.z),
			btVector3(direction.x, direction.y, direction.z)
			);
		physics->world()->rayTest(
			btVector3(origin.x, origin.y, origin.z),
			btVector3(direction.x, direction.y, direction.z),
			RayCallback
			);

		if (RayCallback.hasHit()) {
			luck::entity* e = static_cast<luck::entity*>(RayCallback.m_collisionObject->getUserPointer());
			if (e == nullptr) {
				return false;
			}

			hit_info.hit_entity = luck::entity(*e);
			return true;
		}
		else{
			return false;
		}
	}
	bool raycast(glm::vec3 origin, glm::vec3 direction, float distance = std::numeric_limits<float>::max())
	{
		luck::raycast_hit hit_info;
		return raycast(origin, direction, hit_info, distance);
	}
	bool raycast(luck::ray ray, float distance = std::numeric_limits<float>::max())
	{
		return raycast(ray.origin, ray.direction, distance);
	}
	bool raycast(luck::ray ray, luck::raycast_hit& hit_info, float distance = std::numeric_limits<float>::max())
	{
		return raycast(ray.origin, ray.direction, hit_info, distance);
	}*/

	/*ray screen_pos_to_ray(const luck::entity& camera, glm::vec2 position)
	{
		//from: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/

		float mouseY = (float)screen::size().y - position.y;

		glm::vec4 lRayStart_NDC(
			((float)position.x / (float)screen::size().x - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
			((float)mouseY / (float)screen::size().y - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
			-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
			1.0f
		);

		glm::vec4 lRayEnd_NDC(
			((float)position.x / (float)screen::size().x - 0.5f) * 2.0f,
			((float)mouseY / (float)screen::size().y - 0.5f) * 2.0f,
			0.0,
			1.0f
		);

		glm::mat4 ProjectionMatrix = luck::camera_system::calculate_projection(camera);
		glm::mat4 ViewMatrix = luck::camera_system::calculate_view(camera);

		// The Projection matrix goes from Camera Space to NDC.
		// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
		glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

		// The View Matrix goes from World Space to Camera Space.
		// So inverse(ViewMatrix) goes from Camera Space to World Space.
		glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

		// Faster way (just one inverse)
		glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
		glm::vec4 lRayStart_world = M * lRayStart_NDC; 
		lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_world   = M * lRayEnd_NDC; 
		lRayEnd_world /= lRayEnd_world.w;


		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);

		return luck::ray{glm::vec3(lRayStart_world), glm::normalize(lRayDir_world)};
	}*/
}

void load_scene(luck::world& world, luck::resources& resources, std::string scene_file, luck::program* program);

int main()
{	
	luck::screen screen(1600, 1000, false);
	luck::input::initialize();
	glClearColor(0.3f,0.65f,0.95f,1.f);

	luck::resources resources;
	resources.load<luck::text_resource>("assets/shaders/shader1.vs");
	resources.load<luck::text_resource>("assets/shaders/shader1.fs");
	
	luck::shader vertex_shader(resources.get<luck::text_resource>("assets/shaders/shader1.vs"), GL_VERTEX_SHADER);
	luck::shader fragment_shader(resources.get<luck::text_resource>("assets/shaders/shader1.fs"), GL_FRAGMENT_SHADER);
	luck::program program1(vertex_shader, fragment_shader);
	
	luck::spatial_system spatial_system{};
	luck::renderable_system renderable_system{&spatial_system};
	luck::camera_system camera_system{renderable_system};
	luck::fps_controller_system fps_controller_system{};
	luck::bullet_system bullet_system{};
	luck::character_system character_system{bullet_system.world()};
	luck::tps_controller_system tps_controller_system{};
	luck::test_system test_system{};

	//renderable_system.render_aabbs = true;

	luck::world w{};
	w.addSystem(renderable_system);
	w.addSystem(bullet_system);
	w.addSystem(spatial_system);
	w.addSystem(camera_system);
	w.addSystem(fps_controller_system);
	w.addSystem(character_system);
	w.addSystem(tps_controller_system);
	w.addSystem(test_system);

	size_t resource_count = 8;
	size_t resources_loaded = 0;
	resources.on_load.connect([&](std::string path){
		resources_loaded++;
		LOG(std::setfill('0'),std::setw(3),(int)(((float)resources_loaded/(float)resource_count)*100),"%\t",path);
	});
	resources.on_load_error.connect([&](std::string path){
		resources_loaded++;
		LOG(std::setfill('0'),std::setw(3),(int)(((float)resources_loaded/(float)resource_count)*100),"%\t",path," (Error)");
	});

	load_scene(w, resources, "assets/scene_test2/scene.lsc", &program1);

	//player resources

	resources.load<luck::mesh_data_resource>(luck::tools::mesh::convert("assets/meshs/player.obj","",true)[0]);
	luck::mesh player_mesh{ resources.get<luck::mesh_data_resource>("assets/meshs/player.l3d") };

	resources.load<luck::image_resource>(luck::tools::image::convert("assets/images/T_Indoor.png", "", true));
	luck::texture player_texture{ resources.get<luck::image_resource>("assets/images/T_Indoor.lif") };

	luck::material player_material = luck::material(luck::render_pass(&program1));
	player_material.passes[0].textures["albedo"] = &player_texture;
	player_material.passes[0].vec2["divide"] = glm::vec2(1.0f, 1.0f);
	player_material.passes[0].vec2["move_direction"] = glm::vec2(0, 0);

	luck::entity character = w.createEntity();
	character.addComponent<luck::spatial_component>(glm::vec3(1.f, 3.f, -4.5f), glm::quat(glm::vec3(0.f, glm::radians(90.f), 0.f)));
	character.addComponent<luck::mesh_component>(&player_mesh, player_material);
	character.addComponent<luck::capsule_shape_component>(0.2f,1.6f,glm::vec3(0.f,-0.4f,0.f));
	character.addComponent<luck::character_component>();
	character.activate();
	
	luck::entity camera = w.createEntity();
	camera.addComponent<luck::spatial_component>(glm::vec3(0, 20, 70.f));
	camera.addComponent<luck::camera_component>();
	camera.getComponent<luck::camera_component>().fov = 45.f;
	camera.getComponent<luck::camera_component>().near = 0.1f;
	camera.getComponent<luck::camera_component>().far = 1000.f;
	/*camera.addComponent<luck::fps_controller_component>();
	camera.getComponent<luck::fps_controller_component>().sensitivity = 0.0025f;
	camera.getComponent<luck::fps_controller_component>().move_speed = 50.f;*/
	camera.addComponent<luck::tps_controller_component>();
	camera.getComponent<luck::tps_controller_component>().to_follow = character;
	camera.getComponent<luck::tps_controller_component>().height = 8.f;
	camera.getComponent<luck::tps_controller_component>().distance = 2.f;
	camera.activate();

	double last_time = glfwGetTime();
	int nb_frames = 0;
	int frame = 0;
	char buffer[100];

	double accum = 0;
	int rframe = 0;
	while (!screen.should_close())
	{
		//FPS
		double current_time = glfwGetTime();
		nb_frames++;
		frame++;
		rframe++;
		if (current_time - last_time >= 1.0)
		{
			std::sprintf(buffer, "%f : %f", 1000.0 / double(nb_frames), rframe / (glfwGetTime() - accum));
			luck::application::window_title((const char*)buffer);
			nb_frames = 0;
			last_time += 1.0;
		}

		if (rframe >= 500)
			rframe = 0, accum = glfwGetTime();
		//End FPS

		screen.poll_events();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		luck::application::next_frame();
		w.refresh();
		character_system.update();
		bullet_system.update();
		fps_controller_system.update();
		tps_controller_system.update();
		spatial_system.update();
		camera_system.render();
		bullet_system.debug_draw();

		screen.swap_buffers();

		if (luck::input::key(GLFW_KEY_ESCAPE)) break;
	}
	return 0;
}

/*

mesh|name1|position:x,y,z|rotation:x,y,z,w|scale:x,y,z|solid:dynamic,bounding|solid:static,mesh|spawn|materials:2|texture:tex1.png;texture:tex2.png
lamp|position:x,y,z|rotation:x,y,z,w|...

collision_shape|position:x,y,z|rotation:x,y,z,w|scale:x,y,z|solid:static,mesh|materials:0
street_light.004|position:x,y,z|rotation:x,y,z,w|scale:x,y,z|solid:static,bounding|materials:2|texture:;texture:tex3.png

*/

void load_scene(luck::world& world, luck::resources& resources, std::string scene_file, luck::program* program)
{
	resources.load<luck::text_resource>(scene_file);
	std::string scene_data = resources.get<luck::text_resource>(scene_file).get().text;

	std::vector<std::string> objects;
	boost::split(objects, scene_data, boost::is_any_of("\n"));
	std::string path = luck::tools::get_file_path(scene_file);

	std::unordered_map<std::string, luck::texture*> loaded_textures;
	std::unordered_map<std::string, luck::mesh*> loaded_meshs;

	for (size_t i = 0; i < objects.size(); ++i)
	{
		boost::trim(objects[i]);
		if (objects[i] == "") continue;

		std::vector<std::string> object_info;
		boost::split(object_info, objects[i], boost::is_any_of("|"));

		if (object_info[0] == "mesh")
		{
			luck::entity entity = world.createEntity();

			std::string name = object_info[1];
			LOG(name);

			luck::mesh* mesh;

			if (loaded_meshs[path + "/" + luck::tools::get_file_name(name) + ".l3d"] != nullptr)
			{
				mesh = loaded_meshs[path + "/" + luck::tools::get_file_name(name) + ".l3d"];
			}
			else 
			{
				resources.load<luck::mesh_data_resource>(luck::tools::mesh::convert(path + "/" + name)[0]);
				mesh = new luck::mesh(resources.get<luck::mesh_data_resource>(path + "/" + luck::tools::get_file_name(name) + ".l3d"));
				loaded_meshs[path + "/" + luck::tools::get_file_name(name) + ".l3d"] = mesh;
			}
			
			entity.addComponent<luck::mesh_component>(mesh);
			
			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;
			int materials = 0;
			
			for(size_t i = 2; i < object_info.size(); ++i)
			{
				std::vector<std::string> property;
				boost::split(property,object_info[i],boost::is_any_of(":"));
				
				if(property[0] == "position")
				{
					std::vector<std::string> position_info;
					boost::split(position_info,property[1],boost::is_any_of(","));
					position = glm::vec3(::atof(position_info[0].c_str()),::atof(position_info[1].c_str()),::atof(position_info[2].c_str()));
				}
				else if(property[0] == "rotation")
				{
					std::vector<std::string> rotation_info;
					boost::split(rotation_info,property[1],boost::is_any_of(","));
					rotation = glm::vec3(glm::radians(::atof(rotation_info[0].c_str())), glm::radians(::atof(rotation_info[1].c_str())), glm::radians(::atof(rotation_info[2].c_str())));
				}
				else if(property[0] == "scale")
				{
					std::vector<std::string> scale_info;
					boost::split(scale_info,property[1],boost::is_any_of(","));
					scale = glm::vec3(::atof(scale_info[0].c_str()),::atof(scale_info[1].c_str()),::atof(scale_info[2].c_str()));
				}
				else if(property[0] == "solid")
				{
					std::vector<std::string> solid_info;
					boost::split(solid_info,property[1],boost::is_any_of(","));
					
					entity.addComponent<luck::rigid_body_component>(1.f);
					
					for(auto s : solid_info)
					{
						if(s == "dynamic")
						{
							entity.getComponent<luck::rigid_body_component>().type = luck::rigid_body_component::DYNAMIC;
						}
						else if(s == "static")
						{
							entity.getComponent<luck::rigid_body_component>().type = luck::rigid_body_component::STATIC;
							entity.getComponent<luck::rigid_body_component>().mass = 0.f;
						}
						else if (s == "mesh"/* || s == "bounding"*/)
						{
							entity.addComponent<luck::static_triangle_mesh_shape_component>(resources.get<luck::mesh_data_resource>(path+"/"+luck::tools::get_file_name(name)+".l3d"));
						}
						else if(s == "bounding")
						{
							resources.get<luck::mesh_data_resource>(path+"/"+luck::tools::get_file_name(name)+".l3d").get().calculate_aabb();
							auto h = resources.get<luck::mesh_data_resource>(path+"/"+luck::tools::get_file_name(name)+".l3d").get().aabb;
							h.rotate(glm::quat(rotation));
							entity.addComponent<luck::box_shape_component>(h.getDiagonal());
						}
					}
				}
				else if(property[0] == "materials")
				{
					materials = ::atoi(property[1].c_str());
				}
				else if(property[0] == "texture")
				{
					auto p = luck::tools::reform(property.begin(), property.end(), ':');
					std::vector<std::string> textures;
					boost::split(textures,p,boost::is_any_of(";"));
					
					for(auto t : textures)
					{
						std::vector<std::string> texture;
						boost::split(texture,t,boost::is_any_of(":"));
						
						if(texture[1] != "")
						{
							luck::texture* tex;
						
							if(loaded_textures[path+"/"+texture[1]] != nullptr)
							{
								tex = loaded_textures[path+"/"+texture[1]];
							}
							else
							{
								std::string old_path = path+"/"+texture[1];
								resources.load<luck::image_resource>(luck::tools::image::convert(path+"/"+texture[1]));
								boost::replace_all(texture[1],".png",".lif");
								loaded_textures[old_path] = new luck::texture(resources.get<luck::image_resource>(path+"/"+texture[1]));
								tex = loaded_textures[old_path];
							}
						
							resources.load<luck::image_resource>(luck::tools::image::convert(path+"/"+texture[1]));
							boost::replace_all(texture[1],".png",".lif");
							//auto tex = new luck::texture(resources.get<luck::image_resource>(path+"/"+texture[1]));
							
							luck::material mat = luck::material(luck::render_pass(program));
							mat.passes[0].textures["albedo"] = tex;
							mat.passes[0].vec2["divide"] = glm::vec2(1.0f,1.0f);
							mat.passes[0].vec2["move_direction"] = glm::vec2(0,0);

							entity.getComponent<luck::mesh_component>().materials.push_back(mat);
						}
					}
				}
			}
			
			entity.addComponent<luck::spatial_component>(position,glm::quat(rotation),scale);
			entity.activate();
			
		}
		else if(object_info[0] == "lamp")
		{
		
		}
	}
}