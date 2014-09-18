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
		e_spatial._system = this;

		glm::aabb aabb = e_spatial.aabb;
		aabb.translate(e_spatial.position);
		aabb.scale(e_spatial.scale, e_spatial.position);

		int32_t proxy = _tree.create_proxy(aabb, e);
		e_spatial._proxy = proxy;
	}

	void spatial_system::onEntityRemoved(luck::entity& /*e*/)
	{

	}

	void spatial_system::onSpatialDestroy(spatial_component& c)
	{
		_tree.destroy_proxy(c._proxy);
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
			spatial._difference(position, rotation, scale, aabb);
			aabb.translate(spatial.position);
			aabb.scale(spatial.scale, spatial.position);
			_tree.move_proxy(e.getComponent<spatial_component>()._proxy, aabb, position);
		}
	}
}
