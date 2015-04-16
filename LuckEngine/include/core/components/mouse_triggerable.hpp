#ifndef MOUSE_TRIGGERABLE_HPP
#define MOUSE_TRIGGERABLE_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <boost/signals2.hpp>
#include "../../luck.hpp"
#include "../common/glm.hpp"

namespace luck
{
	struct mouse_triggerable : luck::component<mouse_triggerable>
	{
		boost::signals2::signal<void(int)> on_mouse_down;
		boost::signals2::signal<void(int)> on_mouse_up;
	};
}

#endif