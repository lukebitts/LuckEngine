#include <core/systems/spatial_system.hpp>
#include <core/components/spatial_component.hpp>

namespace luck
{
	spatial_system::spatial_system() : Base(anax::ComponentFilter().requires<spatial_component>())
	{

	}

	void spatial_system::onEntityAdded(luck::entity& e)
	{
		spatial_component& e_spatial = e.getComponent<spatial_component>();
		e_spatial.m_system = this;

		glm::aabb aabb = e_spatial.aabb;

		aabb.rotate(e_spatial.rotation);
		aabb.translate(e_spatial.position);
		//aabb.scale(e_spatial.scale, e_spatial.position);

		e_spatial.aabb = aabb;

		int32_t proxy = m_tree.create_proxy(aabb, e);
		e_spatial.m_proxy = proxy;
	}

	void spatial_system::onEntityRemoved(luck::entity& e)
	{
		spatial_component& c = e.getComponent<spatial_component>();
		m_tree.destroy_proxy(c.m_proxy);
	}

	void spatial_system::onSpatialDestroy(spatial_component& c)
	{
		//m_tree.destroy_proxy(c.m_proxy);
	}

	void spatial_system::update()
	{
		auto entities = getEntities();

		for(auto e : entities)
		{
			auto& spatial = e.getComponent<spatial_component>();

			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
			glm::aabb aabb;
			spatial.m_difference(position, rotation, scale, aabb);

			//aabb.rotate(rotation);
			aabb.translate(position);

			//aabb.translate(spatial.position);
			//aabb.scale(spatial.scale, spatial.position);
			bool r = m_tree.move_proxy(e.getComponent<spatial_component>().m_proxy, aabb, position);
		}
	}
}
