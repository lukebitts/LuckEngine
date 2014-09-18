#ifndef _SCREEN_HPP_
#define _SCREEN_HPP_

#include <boost/signals2.hpp>
#include "opengl.hpp"
#include "common/glm.hpp"
#include "common/debug.hpp"

namespace luck
{
	class screen
	{
		private:
			static GLFWwindow* _window;
			friend class input;
			friend class application;
		public:
			inline screen(int width, int height, bool fullscreen)
			{
				glfwSetErrorCallback([](int error, const char* description){
					LOG("GLFW Error: ", description);
				});
				ASSERT(_window == nullptr, "Only one window is allowed to be opened.");
				auto init = glfwInit();
				ASSERT(init == GL_TRUE, "There was a problem initializing the window manager.");
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
				_window = glfwCreateWindow(width, height, "", fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
				ASSERT(_window, "There was a problem opening the window.");
				glfwMakeContextCurrent(_window);
				glfwSwapInterval(0);
			}
			inline static glm::vec2 size()
			{
				ASSERT(_window,"No window is open");
				int x, y;
				glfwGetWindowSize(_window,&x, &y);
				return glm::vec2(x,y);
			}
			inline static void size(glm::vec2 size)
			{
				ASSERT(_window,"No window is open");
				glfwSetWindowSize(_window,(int)size.x,(int)size.y);
			}
			inline static glm::vec2 cursor_pos() ///@todo should this be here or on the input class?
			{
				ASSERT(_window,"No window is open");
				double x, y;
				glfwGetCursorPos(_window,&x,&y);
				return glm::vec2(x,y);
			}
			inline static void cursor_pos(glm::vec2 pos)
			{
				ASSERT(_window,"No window is open");
				glfwSetCursorPos(_window,pos.x,pos.y);
			}
			inline static void poll_events()
			{
				glfwPollEvents();
			}
			inline static void swap_buffers()
			{
				glfwSwapBuffers(_window);
			}
			inline static int should_close()
			{
				return glfwWindowShouldClose(_window);
			}
			inline ~screen()
			{
				glfwDestroyWindow(_window);
				glfwTerminate();
				_window = nullptr;
			}
	};

	class input
	{
		public:
			static boost::signals2::signal<void(int, int)> on_mouse_down;
			static void initialize()
			{
				ASSERT(screen::_window,"No window is open");
				glfwSetMouseButtonCallback(screen::_window,[](GLFWwindow*, int button, int action, int x){
					input::on_mouse_down(button, action);
				});
			}
			static int key(int k)
			{
				ASSERT(screen::_window,"No window is open");
				return glfwGetKey(screen::_window,k);
			}
			static glm::vec2 mouse_pos()
			{
				ASSERT(screen::_window,"No window is open");
				double x, y;
				glfwGetCursorPos(screen::_window,&x,&y);
				return glm::vec2(x,y);
			}
	};
}

#endif // _SCREEN_HPP_
