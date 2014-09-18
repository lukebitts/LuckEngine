#ifndef _CAMERA_SYSTEM_HPP_
#define _CAMERA_SYSTEM_HPP_

#include "../../luck.hpp"
#include "../common/glm.hpp"

namespace luck
{
	struct spatial_component;
	struct camera_component;
	class renderable_system;

///@todo camera system just gives an easy access to cameras instead of having a render loop
	class camera_system : public luck::system<camera_system>
	{
		private:
			renderable_system& _renderable_system;
		public:
			camera_system(renderable_system& renderable_system);
			void render();
			static glm::mat4 calculate_view(luck::entity);
			static glm::mat4 calculate_projection(luck::entity);
	};
}
#endif // _CAMERA_SYSTEM_HPP_
