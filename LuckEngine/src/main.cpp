#include <iostream>
#include <sstream>
#include <cstdio>

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
#include <core/systems/camera_system.hpp>
#include <core/systems/renderable_system.hpp>
#include <core/systems/spatial_system.hpp>

#include <bullet/btBulletDynamicsCommon.h>

namespace luck
{
	class gl_debugdrawer : public btIDebugDraw
	{
		private:
			int m_debugMode;
		public:
			gl_debugdrawer() {}
			virtual ~gl_debugdrawer() {}
			virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& from_color, const btVector3& to_color)
			{
				if(!camera_component::main.isValid()) return;
				glPushMatrix();

				glm::mat4 mat_projection = camera_system::calculate_projection(camera_component::main);
				glm::mat4 mat_view = camera_system::calculate_view(camera_component::main);

				glLoadMatrixf(glm::value_ptr(mat_projection * mat_view));

				glBegin(GL_LINES);
				glColor3f(from_color.getX(), from_color.getY(), from_color.getZ());
				glVertex3d(from.getX(), from.getY(), from.getZ());
				glColor3f(to_color.getX(), to_color.getY(), to_color.getZ());
				glVertex3d(to.getX(), to.getY(), to.getZ());
				glEnd();

				glPopMatrix();
			}
			virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
			{
				drawLine(from, to, color, color);
			}
			virtual void drawSphere(const btVector3&, btScalar, const btVector3&)
			{
				
			}
			virtual void drawTriangle(const btVector3&, const btVector3&, const btVector3&, const btVector3&, btScalar)
			{
				
			}
			virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&)
			{
				
			}
			virtual void reportErrorWarning(const char* warning_string)
			{
				LOG(warning_string);
			}
			virtual void draw3dText(const btVector3&, const char*)
			{
				
			}
			virtual void setDebugMode(int debugMode)
			{
				m_debugMode = debugMode;
			}
			virtual int getDebugMode() const
			{
				return m_debugMode;
			}
	};

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
			glm::vec2 last_mouse_pos;
		public:
			tps_controller_system() : Base(luck::component_filter().requires<spatial_component, tps_controller_component>())
			{
				input::on_mouse_down.connect([this](int button, int action){
					if(button == GLFW_MOUSE_BUTTON_RIGHT && action)
					{
						right_down = true;
						last_mouse_pos = input::mouse_pos();
					}
					if(button == GLFW_MOUSE_BUTTON_RIGHT && !action)
					{
						right_down = false;
					}
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

					e_spatial.look_at(glm::vec3{ pos.x, t_spatial.position.y, pos.y }); ///@todo copy unity's cool camera :3
				}

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

	class motion_state : public btMotionState
	{
		private:
			luck::entity _e;
			btTransform _initial_position;
		public:
			motion_state(const btTransform& initial_position, luck::entity e)
			{
				_e = e;
				_initial_position = initial_position;
			}
			virtual ~motion_state()
			{
			}
			void setNode(luck::entity e)
			{
				_e = e;
			}
			virtual void getWorldTransform(btTransform& world_trans) const override
			{
				world_trans = _initial_position;
			}
			virtual void setWorldTransform(const btTransform& world_trans) override
			{
				if(!_e.isValid())
					return;

				btQuaternion rot = world_trans.getRotation();
				_e.getComponent<spatial_component>().rotation = glm::quat(rot.x(), rot.y(), rot.z(), rot.w());
				btVector3 pos = world_trans.getOrigin();
				_e.getComponent<spatial_component>().position = glm::vec3(pos.x(), pos.y(), pos.z());
			}
	};

	class kinematic_motion_state : public btMotionState
	{
		private:
			luck::entity _e;
			btTransform _initial_position;
		public:
			kinematic_motion_state(const btTransform& initial_position, luck::entity e)
			{
				_e = e;
				_initial_position = initial_position;
			}
			virtual ~kinematic_motion_state()
			{

			}
			void setNode(luck::entity e)
			{
				_e = e;
			}
			virtual void getWorldTransform(btTransform& world_trans) const override
			{
			 glm::vec3 pos = _e.getComponent<spatial_component>().position;
			 glm::quat rot = _e.getComponent<spatial_component>().rotation;
			 world_trans = btTransform(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
			}
			virtual void setWorldTransform(const btTransform& worldTrans) override {}
	};

	struct base_shape_component : luck::component<base_shape_component>
	{
		///@todo create the other shapeComponents allowed by Bullet
		protected:
			btCollisionShape* _shape = nullptr;
			base_shape_component(btCollisionShape* shape) : _shape(shape) {}
			virtual ~base_shape_component()
			{
				delete _shape;
			}
		public:
			btCollisionShape* shape() const
			{
				return _shape;
			}
	};

	struct sphere_shape_component : base_shape_component
	{
		sphere_shape_component(float radius = 1.f) : base_shape_component(new btSphereShape(radius)) {}
	};

	struct capsule_shape_component : base_shape_component
	{
		capsule_shape_component(float radius = 1.f, float height = 1.f) : base_shape_component(new btCapsuleShape(radius,height)) {}
	};

	struct box_shape_component : base_shape_component
	{
		box_shape_component(glm::vec3 halfExtents = glm::vec3(1.f, 1.f, 1.f)) : base_shape_component(new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z))) {}
	};

	struct static_plane_shape_component : base_shape_component
	{
		static_plane_shape_component() : base_shape_component(new btStaticPlaneShape(btVector3(0, 1, 0), 0)) {}
	};

	struct static_triangle_mesh_shape_component : base_shape_component
	{
		btTriangleMesh* _mesh = nullptr;
		static_triangle_mesh_shape_component(luck::resource_handle<luck::mesh_data_resource> mesh) : base_shape_component(nullptr), _mesh(triangle_mesh_from_mesh(mesh))
		{
			_shape = new btBvhTriangleMeshShape(_mesh, false);
		}
		static btTriangleMesh* triangle_mesh_from_mesh(luck::resource_handle<luck::mesh_data_resource> mesh)
		{
			auto tmesh = new btTriangleMesh();

			for (auto faces : mesh.get().submeshes)
			{
				for (size_t i = 0; i < faces.size(); i += 3)
				{
					auto v1 = btVector3(mesh.get().vertices[faces[i]].x, mesh.get().vertices[faces[i]].y, mesh.get().vertices[faces[i]].z);
					auto v2 = btVector3(mesh.get().vertices[faces[i + 1]].x, mesh.get().vertices[faces[i + 1]].y, mesh.get().vertices[faces[i + 1]].z);
					auto v3 = btVector3(mesh.get().vertices[faces[i + 2]].x, mesh.get().vertices[faces[i + 2]].y, mesh.get().vertices[faces[i + 2]].z);
					tmesh->addTriangle(v1, v2, v3, false);
				}
			}

			return tmesh;
		}
		virtual ~static_triangle_mesh_shape_component()
		{
			delete _mesh;
		}
	};

	struct rigid_body_component : luck::component<rigid_body_component>
	{
		enum body_type { STATIC, DYNAMIC, KINEMATIC };
		body_type type = STATIC;

		std::unique_ptr<btMotionState> motion_state = nullptr;
		std::unique_ptr<btRigidBody> rigid_body = nullptr;
		btDiscreteDynamicsWorld* world = nullptr;

		float mass = 0.f;
		glm::vec3 local_inertia = glm::vec3(0,0,0);
		float linear_damping = 0.f;
		float angular_damping = 0.f;
		float friction = 0.5f;
		float rolling_friction = 0.f;
		float restitution = 0.f;
		float linear_sleeping_threshold = 0.8f;
		float angular_sleeping_threshold = 1.f;
		bool additional_damping = false;
		float additional_damping_factor = 0.005f;
		float additional_linear_damping_threshold_sqr = 0.01f;
		float additional_angular_damping_threshold_sqr = 0.01f;
		float additional_angular_damping_factor = 0.01f;

		rigid_body_component(float mass = 0.f, bool kinematic = false) : mass(mass), type(kinematic ? KINEMATIC : STATIC)
		{

		}
		~rigid_body_component()
		{
			world->removeRigidBody(rigid_body.get());
		}
	};

	class bullet_system : public luck::system<bullet_system> ///@todo add to a separate file
	{
		private:
			btDbvtBroadphase _broadphase {};
			btDefaultCollisionConfiguration _collision_configuration {};
			btCollisionDispatcher _collision_dispatcher;
			btSequentialImpulseConstraintSolver _solver{};
			btDiscreteDynamicsWorld _world;
			gl_debugdrawer _debug_drawer{};
		public:
			bullet_system() : Base(luck::component_filter().requires<spatial_component, base_shape_component, rigid_body_component>()),
				_collision_dispatcher(&_collision_configuration),
				_world(&_collision_dispatcher, &_broadphase, &_solver, &_collision_configuration)
			{
				_debug_drawer.setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
				_world.setDebugDrawer(&_debug_drawer);
				_world.setGravity(btVector3(0, -10, 0));

			}
			virtual void onEntityAdded(luck::entity& e) override
			{
				spatial_component& e_spatial = e.getComponent<spatial_component>();
				rigid_body_component& e_rigidbody = e.getComponent<rigid_body_component>();
				base_shape_component& e_shape = e.getComponent<base_shape_component>();

				e_rigidbody.world = &_world;
				if (e_rigidbody.type == rigid_body_component::body_type::KINEMATIC)
				{
					e_rigidbody.motion_state = std::unique_ptr<kinematic_motion_state>(new kinematic_motion_state(
					btTransform(btQuaternion(e_spatial.rotation.x, e_spatial.rotation.y, e_spatial.rotation.z, e_spatial.rotation.w),
					btVector3(e_spatial.position.x, e_spatial.position.y, e_spatial.position.z)), e));
				}
				else
				{
					e_rigidbody.motion_state = std::unique_ptr<motion_state>(new motion_state(
					btTransform(btQuaternion(e_spatial.rotation.x, e_spatial.rotation.y, e_spatial.rotation.z, e_spatial.rotation.w),
					btVector3(e_spatial.position.x, e_spatial.position.y, e_spatial.position.z)), e));
				}

				btVector3 inertia(0, 0, 0);
				if (e_rigidbody.type == rigid_body_component::body_type::DYNAMIC || e_rigidbody.mass > 0)
					e_shape.shape()->calculateLocalInertia(e_rigidbody.mass, inertia);

				btRigidBody::btRigidBodyConstructionInfo info{e_rigidbody.mass, e_rigidbody.motion_state.get(), e_shape.shape(), inertia};
				info.m_mass = e_rigidbody.mass;
				//info.m_localInertia = btVector3(e_rigidbody.local_inertia.x, e_rigidbody.local_inertia.y, e_rigidbody.local_inertia.z);
				info.m_linearDamping = e_rigidbody.linear_damping;
				info.m_angularDamping = e_rigidbody.angular_damping;
				info.m_friction = e_rigidbody.friction;
				info.m_rollingFriction = e_rigidbody.rolling_friction;
				info.m_restitution = e_rigidbody.restitution;
				info.m_linearSleepingThreshold = e_rigidbody.linear_sleeping_threshold;
				info.m_angularSleepingThreshold = e_rigidbody.angular_sleeping_threshold;
				info.m_additionalDamping = e_rigidbody.additional_damping;
				info.m_additionalDampingFactor = e_rigidbody.additional_damping_factor;
				info.m_additionalLinearDampingThresholdSqr = e_rigidbody.additional_linear_damping_threshold_sqr;
				info.m_additionalAngularDampingThresholdSqr = e_rigidbody.additional_angular_damping_threshold_sqr;
				info.m_additionalAngularDampingFactor = e_rigidbody.additional_damping_factor;

				e_rigidbody.rigid_body = std::unique_ptr<btRigidBody>(new btRigidBody(info));

				if (e_rigidbody.type == rigid_body_component::body_type::KINEMATIC)
				{
					e_rigidbody.rigid_body->setCollisionFlags(e_rigidbody.rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
					e_rigidbody.rigid_body->setActivationState(DISABLE_DEACTIVATION);
				}

				_world.addRigidBody(e_rigidbody.rigid_body.get());
			}
			virtual void onEntityRemoved(luck::entity&) override
			{
				///@todo remove from the world and destroy the rigidbody here?
				/// reinitialize the RigidBodyComponent to clear it from references?
			}
			void update()
			{
				_world.stepSimulation(1 / 60.f, 10);
			}
			void debug_draw()
			{
				_world.debugDrawWorld();
			}
	};

	struct character_component : public luck::component<character_component>
	{
		
	};
	
	class character_system : public luck::system<character_system>
	{
		public:
			character_system() : Base(luck::component_filter().requires<spatial_component, base_shape_component, rigid_body_component, character_component>())
			{

			}
			void update()
			{
				auto entities = getEntities();
				
				int x = luck::input::key('A') ? -1 : luck::input::key('D') ? 1 : 0;
				int z = luck::input::key('W') ? -1 : luck::input::key('S') ? 1 : 0;
				
				for(auto e : entities)
				{
					auto& body = e.getComponent<luck::rigid_body_component>();
					body.rigid_body->activate();
					float y = body.rigid_body->getLinearVelocity().y();
					
					body.rigid_body->setLinearVelocity(btVector3(x*1000*application::delta,y,z*1000*application::delta));
				}
			}
	};
}

void load_scene(luck::world& world, luck::resources& resources, std::string scene_file, luck::program* program);

int main()
{
	luck::screen screen(800, 600, false);
	luck::input::initialize();
	glClearColor(0.3f,0.65f,0.95f,1.f);

	luck::resources resources;
	resources.load<luck::text_resource>("assets/shaders/shader1.vs");
	resources.load<luck::text_resource>("assets/shaders/shader1.fs");
	
	luck::shader vertex_shader(resources.get<luck::text_resource>("assets/shaders/shader1.vs"), GL_VERTEX_SHADER);
	luck::shader fragment_shader(resources.get<luck::text_resource>("assets/shaders/shader1.fs"), GL_FRAGMENT_SHADER);
	luck::program program1(vertex_shader, fragment_shader);
	
	luck::spatial_system s{};
	luck::renderable_system r{&s};
	luck::camera_system c{r};
	luck::fps_controller_system f{};
	luck::bullet_system b{};
	luck::character_system casc;
	luck::tps_controller_system tps{};

	luck::world w{};
	w.addSystem(s);
	w.addSystem(r);
	w.addSystem(c);
	w.addSystem(f);
	w.addSystem(b);
	w.addSystem(casc);
	w.addSystem(tps);

	size_t resource_count = 36;
	size_t resources_loaded = 0;
	resources.on_load.connect([&](std::string path){
		resources_loaded++;
		LOG(std::setfill('0'),std::setw(3),(int)(((float)resources_loaded/(float)resource_count)*100),"%\t",path);
	});
	resources.on_load_error.connect([&](std::string path){
		resources_loaded++;
		LOG(std::setfill('0'),std::setw(3),(int)(((float)resources_loaded/(float)resource_count)*100),"%\t",path," (Error)");
	});

	load_scene(w, resources, "assets/scenes/scene.lsc", &program1);

	/*e_rigidbody.rigid_body->setFriction(1.5f);
	e_rigidbody.rigid_body->setRestitution(0.6f);
	e_rigidbody.rigid_body->setRollingFriction(1.f);*/

	luck::entity character = w.createEntity();
	character.addComponent<luck::spatial_component>(glm::vec3(0,20.f,80.f));
	character.addComponent<luck::rigid_body_component>(1.f);
	character.getComponent<luck::rigid_body_component>().type = luck::rigid_body_component::KINEMATIC;
	character.getComponent<luck::rigid_body_component>().friction = 1.5f;
	character.getComponent<luck::rigid_body_component>().restitution = 0.6f;
	character.getComponent<luck::rigid_body_component>().rolling_friction = 1.0f;
	character.addComponent<luck::capsule_shape_component>();
	character.addComponent<luck::character_component>();
	character.activate();
	
	luck::entity camera = w.createEntity();
	camera.addComponent<luck::spatial_component>(glm::vec3(0,20,70.f));
	camera.addComponent<luck::camera_component>();
	camera.getComponent<luck::camera_component>().fov = 45.f;
	camera.getComponent<luck::camera_component>().near = 0.1f;
	camera.getComponent<luck::camera_component>().far = 1000.f;
	/*camera.addComponent<luck::fps_controller_component>();
	camera.getComponent<luck::fps_controller_component>().sensitivity = 0.0025f;
	camera.getComponent<luck::fps_controller_component>().move_speed = 50.f;*/
	camera.addComponent<luck::tps_controller_component>();
	camera.getComponent<luck::tps_controller_component>().to_follow = character;
	camera.getComponent<luck::tps_controller_component>().height = 20.f;
	camera.getComponent<luck::tps_controller_component>().distance = 10.f;
	camera.activate();
	
	double last_time = glfwGetTime();
	int nb_frames = 0;
	int frame = 0;
	char buffer[100];

	double accum = 0;
	int rframe = 0;
	while(!screen.should_close())
	{
		//FPS
		double current_time = glfwGetTime();
		nb_frames++;
		frame++;
		rframe++;
		if(current_time - last_time >= 1.0)
		{
			std::sprintf(buffer, "%f : %f", 1000.0 / double(nb_frames), rframe / (glfwGetTime()-accum));
			luck::application::window_title((const char*)buffer);
			nb_frames = 0;
			last_time += 1.0;
		}

		if(rframe >= 500)
			rframe = 0, accum = glfwGetTime();
		//End FPS

		screen.poll_events();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		luck::application::next_frame();
		casc.update();
		w.refresh();
		b.update();
		f.update();
		tps.update();
		s.update();
		c.render();
		b.debug_draw();

		screen.swap_buffers();
		
		if(luck::input::key(GLFW_KEY_ESCAPE)) break;
	}
	return 0;
}

void load_scene(luck::world& world, luck::resources& resources, std::string scene_file, luck::program* program)
{
	resources.load<luck::text_resource>(scene_file);
	std::string scene_data = resources.get<luck::text_resource>(scene_file).get().text;

	std::vector<std::string> objects;
	boost::split(objects,scene_data,boost::is_any_of("\n"));
	std::string path = luck::tools::get_file_path(scene_file);

	luck::entity last_entity;
	for(size_t i = 0; i < objects.size(); ++i)
	{
		boost::trim(objects[i]);
		if(objects[i] == "") continue;
		if(objects[i][0] != ':') //mesh
		{
			std::vector<std::string> object_info;
			boost::split(object_info,objects[i],boost::is_any_of("|"));
			
			resources.load<luck::mesh_data_resource>(luck::tools::mesh::convert(path+"/"+object_info[0]+".obj")[0]);
			//resources.load<luck::mesh_data_resource>(path + "/" + object_info[0] + ".l3d");
			auto mesh = new luck::mesh(resources.get<luck::mesh_data_resource>(path+"/"+object_info[0]+".l3d"));

			std::vector<std::string> position_info;
			boost::split(position_info,object_info[1],boost::is_any_of(","));
			glm::vec3 position(::atof(position_info[0].c_str()),::atof(position_info[1].c_str()),::atof(position_info[2].c_str()));

			std::vector<std::string> rotation_info;
			boost::split(rotation_info,object_info[2],boost::is_any_of(","));
			glm::quat rotation((float)::atof(rotation_info[0].c_str()), (float)::atof(rotation_info[1].c_str()), (float)::atof(rotation_info[2].c_str()), (float)::atof(rotation_info[3].c_str()));

			std::vector<std::string> scale_info;
			boost::split(scale_info,object_info[3],boost::is_any_of(","));
			glm::vec3 scale(::atof(scale_info[0].c_str()),::atof(scale_info[1].c_str()),::atof(scale_info[2].c_str()));

			glm::mat4 convert(
				-1.0000, 0.0000, 0.0000, 0.0000,
				 0.0000, 0.0000, 1.0000, 0.0000,
				 0.0000, 1.0000, 0.0000, 0.0000,
				 0.0000, 0.0000, 0.0000, 1.0000
			);
			position = glm::mat3(convert) * position;
			
			std::swap(rotation.y,rotation.z);
			rotation.z *= -1;
			rotation.y *= -1;

			std::swap(scale.y,scale.z);
			
			last_entity = world.createEntity();
			last_entity.addComponent<luck::spatial_component>();
			last_entity.addComponent<luck::mesh_component>(mesh);

			for(size_t j = 3; j < object_info.size(); ++j)
			{
				if(object_info[j] == "solid")
				{
					last_entity.addComponent<luck::rigid_body_component>();
					last_entity.addComponent<luck::static_triangle_mesh_shape_component>(resources.get<luck::mesh_data_resource>(path+"/"+object_info[0]+".l3d"));
				}
			}

			i++;
			std::vector<std::string> textures;
			boost::split(textures,objects[i],boost::is_any_of(":"));
			for(auto tex : textures)
			{
				boost::trim(tex);
				if(tex != "")
				{
					resources.load<luck::image_resource>(luck::tools::image::convert(path+"/"+tex));
					//boost::replace_all(tex,".png",".lif");
					//resources.load<luck::image_resource>(path + "/" + tex);
					auto texture = new luck::texture(resources.get<luck::image_resource>(path+"/"+tex));
					
					luck::material mat = luck::material(luck::render_pass(program));
					mat.passes[0].textures["albedo"] = texture;
					mat.passes[0].vec2["divide"] = glm::vec2(1.0f,1.0f);
					mat.passes[0].vec2["move_direction"] = glm::vec2(0,0);

					last_entity.getComponent<luck::mesh_component>().materials.push_back(mat);
				}
			}

			last_entity.activate();
		}
	}
}
