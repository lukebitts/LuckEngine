#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <boost/signals2.hpp>
#include "opengl.hpp"
#include "common/glm.hpp"
#include "common/debug.hpp"

namespace luck
{
	class screen
	{
		private:
			static GLFWwindow* m_window;
			friend class input;
			friend class application;
		public:
			inline screen(int width, int height, bool fullscreen)
			{
				glfwSetErrorCallback([](int error, const char* description){
					LOG("GLFW Error: ", description);
				});
				ASSERT(m_window == nullptr, "Only one window is allowed to be opened.");
				auto init = glfwInit();
				ASSERT(init == GL_TRUE, "There was a problem initializing the window manager.");
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
				m_window = glfwCreateWindow(width, height, "", fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
				ASSERT(m_window, "There was a problem opening the window.");
				#ifdef __APPLE__
				///@todo: we have to hide the cursor on mac, this should be wrapped by the input class though
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				#endif
				glfwMakeContextCurrent(m_window);
				glfwSwapInterval(0);
			}
			inline static glm::vec2 size()
			{
				ASSERT(m_window,"No window is open");
				int x, y;
				glfwGetWindowSize(m_window,&x, &y);
				return glm::vec2(x,y);
			}
			inline static void size(glm::vec2 size)
			{
				ASSERT(m_window,"No window is open");
				glfwSetWindowSize(m_window,(int)size.x,(int)size.y);
			}
			inline static glm::vec2 cursor_pos() ///@todo should this be here or on the input class?
			{
				ASSERT(m_window,"No window is open");
				double x, y;
				glfwGetCursorPos(m_window,&x,&y);
				return glm::vec2(x,y);
			}
			inline static void cursor_pos(glm::vec2 pos)
			{
				ASSERT(m_window,"No window is open");
				glfwSetCursorPos(m_window,pos.x,pos.y);
			}
			inline static void poll_events()
			{
				glfwPollEvents();
			}
			inline static void swap_buffers()
			{
				glfwSwapBuffers(m_window);
			}
			inline static int should_close()
			{
				return glfwWindowShouldClose(m_window);
			}
			inline ~screen()
			{
				glfwDestroyWindow(m_window);
				glfwTerminate();
				m_window = nullptr;
			}
	};

	class input
	{
		public:
			static boost::signals2::signal<void(int, int)> on_mouse_down;
			static boost::signals2::signal<void(float, float)> on_mouse_wheel;
			static void initialize()
			{
				ASSERT(screen::m_window,"No window is open");
				glfwSetMouseButtonCallback(screen::m_window,[](GLFWwindow*, int button, int action, int x){
					input::on_mouse_down(button, action);
				});
				glfwSetScrollCallback(screen::m_window, [](GLFWwindow*, double x, double y){
					input::on_mouse_wheel((float)x, (float)y);
				});
			}
			static int key(int k)
			{
				ASSERT(screen::m_window,"No window is open");
				return glfwGetKey(screen::m_window,k);
			}
			static glm::vec2 mouse_pos()
			{
				ASSERT(screen::m_window,"No window is open");
				double x, y;
				glfwGetCursorPos(screen::m_window,&x,&y);
				return glm::vec2(x,y);
			}
	};
}

#endif // SCREEN_HPP
