#include <core/screen.hpp>

namespace luck
{
	GLFWwindow* screen::_window = nullptr;

	boost::signals2::signal<void(int, int)> input::on_mouse_down{};
}