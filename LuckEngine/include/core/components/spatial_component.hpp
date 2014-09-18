#ifndef _SPATIAL_COMPONENT_HPP_
#define _SPATIAL_COMPONENT_HPP_

#include "../../luck.hpp"
#include "../common/glm.hpp"
#include "../common/aabb.hpp"

namespace luck
{
	class spatial_system;

	struct spatial_component : public luck::component<spatial_component>
	{
			glm::vec3 position {};
			glm::quat rotation {};
			glm::vec3 scale{1, 1, 1};
			glm::aabb aabb{glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1)};
			inline spatial_component(glm::vec3 position = glm::vec3(), glm::quat rotation = glm::quat (),
			                         glm::vec3 scale = glm::vec3(1, 1, 1), glm::aabb aabb = glm::aabb (glm::vec3( -1, -1, -1), glm::vec3(1, 1, 1)))
				: position(position), rotation(rotation), scale(scale), aabb(aabb), _position(position), _rotation(rotation), _scale(scale), _aabb(aabb)
			{
			}
			spatial_component(const spatial_component&) = delete;
			spatial_component& operator=(const spatial_component&) = delete;
			void look_at(glm::vec3 look_vector, glm::vec3 up_vector = glm::vec3(0,1,0));
			virtual ~spatial_component();
		private:
			friend class spatial_system;
			spatial_system* _system = nullptr;
			int _proxy = -1;
			glm::vec3 _position {};
			glm::quat _rotation {};
			glm::vec3 _scale {1, 1, 1};
			glm::aabb _aabb {glm::vec3{ -1, -1, -1}, glm::vec3{1, 1, 1}};
			inline void _difference(glm::vec3& position, glm::quat& rotation, glm::vec3& scale, glm::aabb& aabb)
			{
				position = this->position - _position;
				rotation = this->rotation;
				scale = this->scale - _scale;
				aabb = this->aabb;

				_position = this->position;
				_rotation = this->rotation;
				_scale = this->scale;
				_aabb = this->aabb;
			}
	};
}
#endif // _SPATIAL_COMPONENT_HPP_
