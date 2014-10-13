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

		//LOG("1: ",aabb);

		/*glm::vec3 pTab[8];
		pTab[0] = aabb.min;
		pTab[1] = glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z);
		pTab[2] = glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z);
		pTab[3] = glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z);
		pTab[4] = glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z);
		pTab[5] = glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z);
		pTab[6] = glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z);
		pTab[7] = aabb.max;

		glm::mat4 mat_model(1.f);
		mat_model = mat_model * glm::toMat4(e_spatial.rotation);

		for (int i = 0; i < 8; ++i)
		{
			pTab[i] = glm::vec3(mat_model * glm::vec4(pTab[i],1.f));
		}

		aabb.min = pTab[0];
		aabb.max = pTab[0];

		for (int i = 0; i < 8; ++i)
		{
			aabb.min = glm::min(aabb.min, pTab[i]);
			aabb.max = glm::max(aabb.max, pTab[i]);
		}*/

		aabb.rotate(e_spatial.rotation);

		//LOG("2: ", aabb);

		aabb.translate(e_spatial.position);
		aabb.scale(e_spatial.scale, e_spatial.position);

		//LOG("3: ", aabb);

		e_spatial.aabb = aabb;

		int32_t proxy = m_tree.create_proxy(aabb, e);
		e_spatial.m_proxy = proxy;
	}

	void spatial_system::onEntityRemoved(luck::entity& /*e*/)
	{

	}

	void spatial_system::onSpatialDestroy(spatial_component& c)
	{
		m_tree.destroy_proxy(c.m_proxy);
	}

	void spatial_system::update()
	{
		/*auto entities = getEntities();

		for(auto e : entities)
		{
			auto& spatial = e.getComponent<spatial_component>();

			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
			glm::aabb aabb;
			spatial.m_difference(position, rotation, scale, aabb);
			//aabb.translate(spatial.position);
			//aabb.scale(spatial.scale, spatial.position);
			m_tree.move_proxy(e.getComponent<spatial_component>().m_proxy, aabb, position);
		}*/
	}
}
