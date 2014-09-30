#include <core/screen.hpp>

namespace luck
{
	GLFWwindow* screen::m_window = nullptr;

	boost::signals2::signal<void(int, int)> input::on_mouse_down{};
}