#include <core/components/spatial_component.hpp>
#include <core/systems/spatial_system.hpp>
#include <cmath>

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = glm::normalize(start);
	dest = glm::normalize(dest);
	
	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;
	
	if (cosTheta < -1 + 0.001f){
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length(rotationAxis) < 0.01 )
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}
	
	rotationAxis = glm::cross(start, dest);
	
	float s = sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;
	
	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

namespace luck
{
	void spatial_component::look_at(glm::vec3 look_at, glm::vec3 up_direction)
	{
		glm::vec3 delta = -(look_at-position);
		glm::vec3 desiredUp(0,1,0.00001);
		glm::quat rot1 = RotationBetweenVectors(glm::vec3(0,0,1), delta);
		glm::vec3 right = glm::cross(delta, desiredUp);
		desiredUp = glm::cross(right, delta);
		glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
		glm::quat rot2 = RotationBetweenVectors(newUp, desiredUp);
		glm::quat targetOrientation = rot2 * rot1;

		rotation = targetOrientation;
		/*rotation = glm::quat(0,0,0,1);
		
		glm::vec3 vector_to(glm::normalize(look_at-position));

		glm::vec3 local_vector = rotation * glm::vec3(0,0,-1);
		glm::vec3 axis = glm::cross(vector_to,local_vector);
		float angle = acos(glm::dot(vector_to,local_vector));
		glm::vec3 third_vec = glm::cross(axis,local_vector);

		if (glm::dot(third_vec,vector_to) < 0)
		{
			angle = -angle;
		}

		glm::quat axis_angle;
		axis = glm::normalize(axis);
		axis_angle = glm::angleAxis(angle, glm::vec3(axis.x, axis.y, axis.z));

		rotation = axis_angle * rotation;
		
		glm::mat4 mat(1);
		//mat = glm::translate(mat, position);
		mat = glm::lookAt(position-position,look_at-position,up_direction);*/

		//rotation = glm::quat(mat);
	}

	spatial_component::~spatial_component()
	{
		_system->onSpatialDestroy(*this);
	}
}
