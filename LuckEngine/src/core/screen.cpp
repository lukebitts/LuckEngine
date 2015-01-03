#include <core/screen.hpp>

namespace luck
{
	GLFWwindow* screen::m_window = nullptr;

	boost::signals2::signal<void(int, int)> input::on_mouse_down{};
	boost::signals2::signal<void(float, float)> input::on_mouse_wheel{};
}