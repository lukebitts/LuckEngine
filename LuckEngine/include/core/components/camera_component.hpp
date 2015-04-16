#ifndef CAMERA_COMPONENT_HPP
#define CAMERA_COMPONENT_HPP

#include "../../luck.hpp"

namespace luck
{
	struct camera_component : public component<camera_component>
	{
		public:
			enum Type { PERSPECTIVE, ORTHOGRAPHIC };
			enum ClearFlags { NONE = 0, COLOR = 1, DEPTH = 2, ALL = COLOR | DEPTH };
			float fov = 45.f;
			float near = 0.1f;
			float far = 100.f;
			int depth = 0;
			glm::vec3 clearColor{ 0.f, 0.f, 0.f };
			glm::vec2 sensor = glm::vec2{};
			Type type = Type::PERSPECTIVE;
			ClearFlags clearFlags = ClearFlags::ALL; ///@todo use a bitfield here
			inline camera_component() { }
			static entity main;
	};
}
#endif //CAMERA_COMPONENT_HPP
