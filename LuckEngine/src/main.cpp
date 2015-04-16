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

	struct test_component : public luck::component<test_component>
	{
		MEMBERFY(test_component, int, test_number);
		MEMBERFY(test_component, int, test_number2);
		public:
			boost::signals2::connection conn;
	};

	class test_system : public luck::system<test_system>
	{
		public:
			test_system() :
				Base(luck::component_filter().requires<mouse_triggerable, test_component>())
			{

			}
			void onEntityAdded(luck::entity& e) override
			{
				auto l = [e](int button) mutable {
					LOG(e.getId());
					e.getComponent<test_component>().conn.disconnect();
					//delete conn;
					//e.kill();
				};
				e.getComponent<test_component>().conn = e.getComponent<mouse_triggerable>().on_mouse_down.connect(l);
				/*e.getComponent<rigid_body_component>().on_collision.connect([e](luck::entity& other) mutable {
					//LOG(e.getId(), " ", other.getId());
				});*/

				//e.getComponent<rigid_body_component>().on_mouse_down.disconnect(conn);
			}
	};
}

void load_scene(luck::world& world, luck::resources& resources, std::string scene_file, luck::program* program);
void load_scene2(luck::world& world, luck::resources& resources, std::string scene_file, luck::program* program);

int main()
{	
	luck::screen screen(950, 540, false);
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

	renderable_system.render_aabbs = true;

	luck::world w{};
	w.addSystem(renderable_system);
	w.addSystem(bullet_system);
	w.addSystem(spatial_system);
	w.addSystem(camera_system);
	w.addSystem(fps_controller_system);
	w.addSystem(character_system);
	w.addSystem(tps_controller_system);
	w.addSystem(test_system);

	resources.on_load.connect([&](std::string path){
		LOG(path);
	});
	resources.on_load_error.connect([&](std::string path){
		LOG(path," (Error)");
	});

	load_scene2(w, resources, "assets/scene_test3/scene.lsc", &program1);

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
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void load_scene2(luck::world& world, luck::resources& resources, std::string scene_file, luck::program* program)
{
	resources.load<luck::text_resource>(scene_file);
	std::string scene_data = resources.get<luck::text_resource>(scene_file).get().text;
	std::string path = luck::tools::get_file_path(scene_file);
	std::string mesh_name = "";

	std::vector<std::string> objects;
	boost::split(objects, scene_data, boost::is_any_of("\n"));

	std::unordered_map<std::string, luck::entity> entities;
	std::unordered_map<std::string, luck::texture*> loaded_textures;
	std::unordered_map<std::string, luck::mesh*> loaded_meshs;

	std::string entity_name = objects[0].substr(1);
	luck::entity e;

	for (size_t i = 1; i < objects.size(); ++i)
	{
		if (entities.find(entity_name) == entities.end()) {
			e = world.createEntity();
			entities[entity_name] = e;
		}

		if (objects[i].find("#") == 0)
		{
			entity_name = objects[i].substr(1);
			continue;
		}

		std::vector<std::string> component;
		boost::split(component, objects[i], boost::is_any_of(":"));

		if (component[0] == "Spatial")
		{
			luck::spatial_component& c = e.addComponent<luck::spatial_component>();

			std::vector<std::string> data;
			boost::split(data, component[1], boost::is_any_of(","));

			glm::vec3 position = glm::vec3(::atof(data[0].c_str()), ::atof(data[1].c_str()), ::atof(data[2].c_str()));
			glm::quat rotation = glm::quat(glm::vec3(glm::radians(::atof(data[3].c_str())), glm::radians(::atof(data[4].c_str())), glm::radians(::atof(data[5].c_str()))));
			glm::vec3 scale = glm::vec3(::atof(data[6].c_str()), ::atof(data[7].c_str()), ::atof(data[8].c_str()));

			c.position = position;
			c.scale = scale;
			c.rotation = rotation;
			c.entity = e;

			if (data[9] != "-")
			{
				c.parent(entities[data[9]]);
			}
		}
		else if (component[0] == "Mesh")
		{
			std::vector<std::string> data;
			boost::split(data, component[1], boost::is_any_of(","));

			mesh_name = data[0];

			luck::mesh* mesh;
			if (loaded_meshs[path + "/" + luck::tools::get_file_name(data[0]) + ".l3d"] != nullptr)
			{
				mesh = loaded_meshs[path + "/" + luck::tools::get_file_name(data[0]) + ".l3d"];
			}
			else
			{
				resources.load<luck::mesh_data_resource>(luck::tools::mesh::convert(path + "/" + data[0])[0]);
				mesh = new luck::mesh(resources.get<luck::mesh_data_resource>(path + "/" + luck::tools::get_file_name(data[0]) + ".l3d"));
				loaded_meshs[path + "/" + luck::tools::get_file_name(data[0]) + ".l3d"] = mesh;
			}

			e.addComponent<luck::mesh_component>(mesh);

			for (size_t j = 1; j < data.size(); ++j) 
			{
				luck::texture* texture;
				if (loaded_textures[path + "/" + luck::tools::get_file_name(data[j]) + "lif"] != nullptr)
				{
					texture = loaded_textures[path + "/" + luck::tools::get_file_name(data[j]) + "lif"];
				}
				else
				{
					resources.load<luck::image_resource>(luck::tools::image::convert(path + "/" + data[j]));
					texture = new luck::texture(resources.get<luck::image_resource>(path + "/" + luck::tools::get_file_name(data[j]) + ".lif"));
					loaded_textures[path + "/" + luck::tools::get_file_name(data[j]) + "lif"] = texture;
				}

				luck::material mat = luck::material(luck::render_pass(program));
				mat.passes[0].textures["albedo"] = texture;
				mat.passes[0].vec2["divide"] = glm::vec2(1.0f, 1.0f);
				mat.passes[0].vec2["move_direction"] = glm::vec2(0, 0);

				e.getComponent<luck::mesh_component>().materials.push_back(mat);
			}
		}
		else if (component[0] == "Camera")
		{
			std::vector<std::string> data;
			boost::split(data, component[1], boost::is_any_of(","));

			luck::camera_component& c = e.addComponent<luck::camera_component>();

			if (data[0] == "PERSP") c.type = luck::camera_component::Type::PERSPECTIVE;
			else if (data[0] == "ORTHO") c.type = luck::camera_component::Type::ORTHOGRAPHIC;

			c.fov = (float)::atof(data[1].c_str());
			c.near = (float)::atof(data[2].c_str());
			c.far = (float)::atof(data[3].c_str());
			c.sensor = glm::vec2((float)::atof(data[4].c_str()), (float)::atof(data[5].c_str()));
			c.depth = ::atoi(data[6].c_str());

			if (data[7] == "NONE") c.clearFlags = luck::camera_component::NONE;
			else if (data[7] == "COLOR") c.clearFlags = luck::camera_component::COLOR;
			else if (data[7] == "DEPTH") c.clearFlags = luck::camera_component::DEPTH;
			else c.clearFlags = luck::camera_component::ALL;

			c.clearColor = glm::vec3((float)::atof(data[8].c_str()), (float)::atof(data[9].c_str()), (float)::atof(data[10].c_str()));

			if (data[10] == "True")
				luck::camera_component::main = e;
		}
		else if (component[0] == "Shape")
		{
			std::vector<std::string> data;
			boost::split(data, component[1], boost::is_any_of(","));

			if (data[0] == "BOX")
			{
				e.addComponent<luck::box_shape_component>(glm::vec3((float)::atof(data[1].c_str()), (float)::atof(data[2].c_str()), (float)::atof(data[3].c_str())));
			}
			else if (data[0] == "SPHERE") 
			{
				e.addComponent<luck::sphere_shape_component>((float)::atof(data[1].c_str()));
			}
			else if (data[0] == "MESH")
			{
				e.addComponent<luck::static_triangle_mesh_shape_component>(resources.get<luck::mesh_data_resource>(path + "/" + luck::tools::get_file_name(mesh_name) + ".l3d"));
			}
			else if (data[0] == "CONVEX_HULL")
			{
				e.addComponent<luck::convex_hull_shape_component>(resources.get<luck::mesh_data_resource>(path + "/" + luck::tools::get_file_name(mesh_name) + ".l3d"));
			}
			else if (data[0] == "CONE")
			{
				
			}
			else if (data[0] == "CYLINDER")
			{

			}
			else if (data[0] == "CAPSULE")
			{
				e.addComponent<luck::capsule_shape_component>((float)::atof(data[1].c_str()), (float)::atof(data[2].c_str()));
			}
		}
		else if (component[0] == "RigidBody")
		{
			std::vector<std::string> data;
			boost::split(data, component[1], boost::is_any_of(","));

			auto& rb = e.addComponent<luck::rigid_body_component>((float)::atof(data[1].c_str()), false);
			if (data[0] == "STATIC")
				rb.type = luck::rigid_body_component::body_type::STATIC;
			else if (data[0] == "DYNAMIC")
				rb.type = luck::rigid_body_component::body_type::DYNAMIC;
			else if (data[0] == "KINEMATIC")
				rb.type = luck::rigid_body_component::body_type::KINEMATIC;
			rb.friction = (float)::atof(data[2].c_str());
			rb.restitution = (float)::atof(data[3].c_str());
			rb.linear_damping = (float)::atof(data[4].c_str());
			rb.angular_damping = (float)::atof(data[5].c_str());

			e.addComponent<luck::mouse_triggerable>();
		}
		else if (component[0] == "FpsController")
		{
			std::vector<std::string> data;
			boost::split(data, component[1], boost::is_any_of(","));

			auto& f = e.addComponent<luck::fps_controller_component>();
			f.move_speed = (float)::atof(data[0].c_str());
			f.sensitivity = (float)::atof(data[1].c_str());
		}
		else if (component[0] == "TpsController")
		{
			std::vector<std::string> data;
			boost::split(data, component[1], boost::is_any_of(","));

			auto& t = e.addComponent<luck::tps_controller_component>();
			if (entities.find(data[0]) != entities.end())
				t.to_follow = entities[data[0]];
			///@todo: else, defer that to later

			t.height = (float)::atof(data[1].c_str());
			t.distance = (float)::atof(data[2].c_str());
		}
		else if (component[0] == "CharacterController")
		{
			std::vector<std::string> data;
			boost::split(data, component[1], boost::is_any_of(","));

			e.addComponent<luck::character_component>().speed = (float)::atof(data[0].c_str());
		}
	}

	for (auto& entity : entities)
		entity.second.activate();
}