#ifndef KINEMATIC_MOTION_STATE_HPP
#define KINEMATIC_MOTION_STATE_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include "../luck.hpp"
#include "../core/components/spatial_component.hpp"

namespace luck
{
	class kinematic_motion_state : public btMotionState
	{
	private:
		luck::entity m_e;
		btTransform m_initial_position;
	public:
		kinematic_motion_state(const btTransform& initial_position, luck::entity e)
		{
			m_e = e;
			m_initial_position = initial_position;
		}
		virtual ~kinematic_motion_state()
		{

		}
		void setNode(luck::entity e)
		{
			m_e = e;
		}
		virtual void getWorldTransform(btTransform& world_trans) const override
		{
			glm::vec3 pos = m_e.getComponent<spatial_component>().position;
			glm::quat rot = m_e.getComponent<spatial_component>().rotation;
			world_trans = btTransform(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
		}
		virtual void setWorldTransform(const btTransform& worldTrans) override {}
		void* operator new(size_t size) throw()
		{
			return _aligned_malloc(size, 16);
		}
			void operator delete(void *ptr) throw()
		{
			_aligned_free(ptr);
		}
	};
}

#endif