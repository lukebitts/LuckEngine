#ifndef CAMERA_SYSTEM_HPP
#define CAMERA_SYSTEM_HPP

#include "../../luck.hpp"
#include "../common/glm.hpp"
#include "../ray.hpp"
#include "../screen.hpp"

namespace luck
{
	struct spatial_component;
	struct camera_component;
	class renderable_system;

	namespace detail {
		struct frustum {
			float x2, x1, y2, y1, camnear, camfar;
		};
	}

///@todo camera system just gives an easy access to cameras instead of having a render loop
///@todo add camera depth and clear flags (like unity)
	class camera_system : public luck::system<camera_system>
	{
		private:
			renderable_system& m_renderable_system;
		public:
			camera_system(renderable_system& renderable_system);
			void render();
			static glm::mat4 calculate_view(luck::entity);
			static glm::mat4 calculate_projection(luck::entity);
			static ray screen_pos_to_ray(const luck::entity& camera, glm::vec2 position)
			{
				//from: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/

				float mouseY = (float)screen::size().y - position.y;

				glm::vec4 lRayStart_NDC(
					((float)position.x / (float)screen::size().x - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
					((float)mouseY / (float)screen::size().y - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
					-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
					1.0f
					);

				glm::vec4 lRayEnd_NDC(
					((float)position.x / (float)screen::size().x - 0.5f) * 2.0f,
					((float)mouseY / (float)screen::size().y - 0.5f) * 2.0f,
					0.0,
					1.0f
					);

				glm::mat4 ProjectionMatrix = luck::camera_system::calculate_projection(camera);
				glm::mat4 ViewMatrix = luck::camera_system::calculate_view(camera);

				// The Projection matrix goes from Camera Space to NDC.
				// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
				glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

				// The View Matrix goes from World Space to Camera Space.
				// So inverse(ViewMatrix) goes from Camera Space to World Space.
				glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

				// Faster way (just one inverse)
				glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
				glm::vec4 lRayStart_world = M * lRayStart_NDC;
				lRayStart_world /= lRayStart_world.w;
				glm::vec4 lRayEnd_world = M * lRayEnd_NDC;
				lRayEnd_world /= lRayEnd_world.w;


				glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
				lRayDir_world = glm::normalize(lRayDir_world);

				return luck::ray{ glm::vec3(lRayStart_world), glm::normalize(lRayDir_world) };
			}
			static detail::frustum compute_default_frustum(
				const float camnear,
				const float camfar,
				const float lens,
				const float sensor_x, const float sensor_y,
				const short sensor_fit,
				const float design_aspect_ratio
				);
			static detail::frustum compute_default_ortho(
				const float camnear,
				const float camfar,
				const float scale,
				const float design_aspect_ratio,
				const short sensor_fit
				);
	};
}
#endif // CAMERA_SYSTEM_HPP
