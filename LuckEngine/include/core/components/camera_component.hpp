#ifndef CAMERA_COMPONENT_HPP
#define CAMERA_COMPONENT_HPP

#include "../../luck.hpp"

namespace luck
{
	struct camera_component : public component<camera_component>
	{
		public:
			enum Type { PERSPECTIVE, ORTHOGRAPHIC };
			float fov = 45.f;
			float near = 0.1f;
			float far = 100.f;
			Type type = Type::PERSPECTIVE;
			inline camera_component() { }
			static entity main;
	};
}
#endif //CAMERA_COMPONENT_HPP
