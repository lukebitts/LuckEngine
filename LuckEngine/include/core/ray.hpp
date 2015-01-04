#ifndef RAY_HPP
#define RAY_HPP

#include "../luck.hpp"
#include "common/glm.hpp"

namespace luck
{
	struct ray
	{
		glm::vec3 origin;
		glm::vec3 direction;
		ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}
	};

	struct raycast_hit
	{
		luck::entity hit_entity;
		operator bool() const
		{
			return hit_entity.isValid();
		}
	};
}

#endif