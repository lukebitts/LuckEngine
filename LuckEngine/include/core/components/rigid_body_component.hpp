#ifndef RIGID_BODY_COMPONENT_HPP
#define RIGID_BODY_COMPONENT_HPP

///@todo: find actual includes?

namespace luck
{
	struct rigid_body_component : luck::component<rigid_body_component>
	{
		enum body_type { STATIC, DYNAMIC, KINEMATIC };
		body_type type = STATIC;

		boost::signals2::signal<void(int)> on_mouse_down;
		boost::signals2::signal<void(int)> on_mouse_up;

		std::unique_ptr<btMotionState> motion_state = nullptr;
		std::unique_ptr<btRigidBody> rigid_body = nullptr;
		btDiscreteDynamicsWorld* world = nullptr;

		float mass = 0.f;
		glm::vec3 local_inertia = glm::vec3(0, 0, 0);
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
			if (rigid_body.get() != nullptr)
				world->removeRigidBody(rigid_body.get());
		}
	};
}

#endif