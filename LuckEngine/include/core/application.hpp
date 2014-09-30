#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "screen.hpp"

namespace luck
{
	class application
	{
		public:
			static float delta;
			static void next_frame()
			{
				static double old_time = glfwGetTime();
				double current_time = glfwGetTime();
				delta = (float)(current_time - old_time);
				old_time = current_time;
			}
			static void window_title(const char* title)
			{
				glfwSetWindowTitle(luck::screen::m_window, title);
			}
	};

}

#endif //APPLICATION_HPP
