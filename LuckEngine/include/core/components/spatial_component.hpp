#ifndef SPATIAL_COMPONENT_HPP
#define SPATIAL_COMPONENT_HPP

#include "../../luck.hpp"
#include "../common/glm.hpp"
#include "../common/aabb.hpp"

namespace luck
{
	class spatial_system;

	struct spatial_component : public luck::component<spatial_component>
	{
		public:
			glm::vec3 position {};
			glm::quat rotation {}; ///@todo rename rotation to orientation
			glm::vec3 scale{1, 1, 1};
			glm::aabb aabb{glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1)};
			inline spatial_component(glm::vec3 position = glm::vec3(), glm::quat rotation = glm::quat (),
			                         glm::vec3 scale = glm::vec3(1, 1, 1), glm::aabb aabb = glm::aabb (glm::vec3( -1, -1, -1), glm::vec3(1, 1, 1)))
				: position(position), rotation(rotation), scale(scale), aabb(aabb), m_position(position), m_rotation(rotation), m_scale(scale), m_aabb(aabb)
			{
			}
			spatial_component(const spatial_component&) = delete;
			spatial_component& operator=(const spatial_component&) = delete;
			void look_at(glm::vec3 look_vector, glm::vec3 up_vector = glm::vec3(0,1,0));
			virtual ~spatial_component();
		private:
			friend class spatial_system;
			spatial_system* m_system = nullptr;
			int m_proxy = -1;
			glm::vec3 m_position {};
			glm::quat m_rotation {};
			glm::vec3 m_scale {1, 1, 1};
			glm::aabb m_aabb {glm::vec3{ -1, -1, -1}, glm::vec3{1, 1, 1}};
			inline void m_difference(glm::vec3& position, glm::quat& rotation, glm::vec3& scale, glm::aabb& aabb)
			{
				position = this->position - m_position;
				rotation = glm::quat{0,0,0,1};//this->rotation;
				scale = this->scale - m_scale;
				aabb = this->aabb;

				this->aabb.translate(position);

				m_position = this->position;
				m_rotation = this->rotation;
				m_scale = this->scale;
				m_aabb = this->aabb;
			}
	};
}
#endif //SPATIAL_COMPONENT_HPP
