#ifndef BULLET_SYSTEM_HPP
#define BULLET_SYSTEM_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <boost/signals2.hpp>
#include "../../luck.hpp"
#include "../../extras/gl_debugdrawer.hpp"
#include "../../extras/kinematic_motion_state.hpp"
#include "../../extras/motion_state.hpp"
#include "../components/shape_component.hpp"
#include "../components/rigid_body_component.hpp"
#include "../ray.hpp"

namespace luck
{
	class bullet_system : public luck::system<bullet_system> ///@todo add to a separate file
	{
	private:
		btDbvtBroadphase m_broadphase{};
		btDefaultCollisionConfiguration m_collision_configuration{};
		btCollisionDispatcher m_collision_dispatcher;
		btSequentialImpulseConstraintSolver m_solver{};
		btDiscreteDynamicsWorld m_world;
		gl_debugdrawer m_debug_drawer{};
		void m_on_collision(btDynamicsWorld* world, float time)
		{
			//LOG("ON COLLISION: ",world->getDispatcher()->getNumManifolds());
		}
		static void m_tick_callback(btDynamicsWorld* world, float time)
		{
			bullet_system* s = static_cast<bullet_system*>(world->getWorldUserInfo());
			s->m_on_collision(world, time);
		}
	public:
		bool raycast(glm::vec3 origin, glm::vec3 direction, luck::raycast_hit& hit_info, float distance = std::numeric_limits<float>::max())
		{
			//direction = glm::min(direction*distance,glm::vec3(INFINITY));

			direction *= 1000.f;

			btCollisionWorld::ClosestRayResultCallback RayCallback(
				btVector3(origin.x, origin.y, origin.z),
				btVector3(direction.x, direction.y, direction.z)
				);
			world()->rayTest(
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
		}

		btDiscreteDynamicsWorld* world() { return &m_world; }
		boost::signals2::signal<void(btDynamicsWorld*, float)> on_collision;
		bullet_system() : Base(luck::component_filter().requires<spatial_component, base_shape_component, rigid_body_component>()),
			m_collision_dispatcher(&m_collision_configuration),
			m_world(&m_collision_dispatcher, &m_broadphase, &m_solver, &m_collision_configuration)
		{
			m_debug_drawer.setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
			m_world.setDebugDrawer(&m_debug_drawer);
			m_world.setGravity(btVector3(0, -10, 0));

			m_world.setInternalTickCallback(m_tick_callback, static_cast<void*>(this));

			input::on_mouse_down.connect([this](int button, int action){
				auto down = (action != 0);
				luck::raycast_hit hit_info;
				if (raycast(camera_system::screen_pos_to_ray(camera_component::main, input::mouse_pos()), hit_info))
				{
					if (down)
						hit_info.hit_entity.getComponent<rigid_body_component>().on_mouse_down(button);
					else
						hit_info.hit_entity.getComponent<rigid_body_component>().on_mouse_up(button);
				}
			});
		}
		virtual void onEntityAdded(luck::entity& e) override
			///@todo implementation file
		{
			spatial_component& e_spatial = e.getComponent<spatial_component>();
			rigid_body_component& e_rigidbody = e.getComponent<rigid_body_component>();
			base_shape_component& e_shape = e.getComponent<base_shape_component>();

			e_rigidbody.world = &m_world;
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

			btRigidBody::btRigidBodyConstructionInfo info{ e_rigidbody.mass, e_rigidbody.motion_state.get(), e_shape.shape(), inertia };
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

			e_rigidbody.rigid_body->setUserPointer(new luck::entity(e));

			m_world.addRigidBody(e_rigidbody.rigid_body.get());
		}
		virtual void onEntityRemoved(luck::entity& e) override
		{
			rigid_body_component& e_rigidbody = e.getComponent<rigid_body_component>();

			delete e_rigidbody.rigid_body->getUserPointer();
			e_rigidbody.motion_state.reset(nullptr);

			m_world.removeRigidBody(e_rigidbody.rigid_body.get());
			e_rigidbody.rigid_body.reset(nullptr);
		}
		float m_accum = 0;
		void update()
		{
			m_accum += luck::application::delta;
			if(m_accum >= 1.f/60.f)
			{
				m_world.stepSimulation(1 / 60.f, 10);
				m_accum = 0;
			}
			//m_world.stepSimulation(luck::application::delta, 10);
		}
		void debug_draw()
		{
			m_world.debugDrawWorld();
		}
	};
}

#endif