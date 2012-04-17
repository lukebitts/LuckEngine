#include "LuckWindow.h"
#include "KeyEvent.h"
#include <GL/glfw.h>
//#include <iostream>
using namespace luck;

LuckWindow* LuckWindow::_instance = nullptr;

LuckWindow::LuckWindow()
{
    glfwInit();
    _running = true;
}

void LuckWindow::keyCallback(s32 key, s32 action)
{
    _instance->dispatchEvent(action ? "KeyDown" : "KeyUp", new event::KeyEvent(key, action));
}

bool LuckWindow::isRunning()
{
    glfwPollEvents();
    _running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
    return _running;
}

void LuckWindow::setWindowCaption(string caption)
{
    glfwSetWindowTitle(caption.c_str());
}

void LuckWindow::updateWindowSize()
{
    glfwGetWindowSize( &width, &height );
}

LuckWindow* createLuckWindow(u16 width, u16 height, u16 redbits, u16 greenbits, u16 bluebits,
               u16 alphabits, u16 depthbits, u16 stencilbits)
{
    LuckWindow* lkw = LuckWindow::getInstance();
    if(glfwOpenWindow( width, height, redbits, greenbits, bluebits, alphabits, depthbits, stencilbits, GLFW_WINDOW ))
    {
        lkw->width = width;
        lkw->height = height;
        glfwSetKeyCallback(LuckWindow::keyCallback);
        glEnable(GL_DEPTH_TEST);
        return lkw;
    }
    return nullptr;
}
