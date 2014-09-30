#ifndef MOTION_STATE_HPP
#define MOTION_STATE_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include "../luck.hpp"
#include "../core/components/spatial_component.hpp"

namespace luck
{
	class motion_state : public btMotionState
	{
	private:
		luck::entity m_e;
		btTransform m_initial_position;
	public:
		motion_state(const btTransform& initial_position, luck::entity e)
		{
			m_e = e;
			m_initial_position = initial_position;
		}
		virtual ~motion_state()
		{
		}
		void setNode(luck::entity e)
		{
			m_e = e;
		}
		virtual void getWorldTransform(btTransform& world_trans) const override
		{
			world_trans = m_initial_position;
		}
		virtual void setWorldTransform(const btTransform& world_trans) override
			///@todo: add this to the implementation file
		{
			if (!m_e.isValid())
				return;

			btQuaternion rot = world_trans.getRotation();
			m_e.getComponent<spatial_component>().rotation = glm::quat(rot.x(), rot.y(), rot.z(), rot.w());
			btVector3 pos = world_trans.getOrigin();
			m_e.getComponent<spatial_component>().position = glm::vec3(pos.x(), pos.y(), pos.z());
		}
	};
}

#endif