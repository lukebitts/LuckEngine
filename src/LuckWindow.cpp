#include "LuckWindow.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
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

void LuckWindow::mouseMoveCallback(s32 x, s32 y)
{
    _instance->dispatchEvent("MouseMove", new event::MouseEvent(core::Vector2<u16>((u16)x,(u16)y),-1,-1,-1));
}

void LuckWindow::mouseClickCallback(s32 button, s32 action)
{
    s32 x, y;
    glfwGetMousePos(&x,&y);
    _instance->dispatchEvent(action == GLFW_RELEASE ? "MouseUp" : "MouseDown", new event::MouseEvent(core::Vector2<u16>(x,y),button,action,-1));
}

void LuckWindow::mouseWheelCallback(s32 position)
{
    s32 x, y;
    glfwGetMousePos(&x,&y);
    _instance->dispatchEvent("MouseWheel",  new event::MouseEvent(core::Vector2<u16>(x,y),-1,-1,position));
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
        glfwSetMousePosCallback(LuckWindow::mouseMoveCallback);
        glfwSetMouseButtonCallback(LuckWindow::mouseClickCallback);
        glfwSetMouseWheelCallback(LuckWindow::mouseWheelCallback);

        ///@todo make a mouse callback (André)
        glEnable(GL_DEPTH_TEST);
        return lkw;
    }
    return nullptr;
}
