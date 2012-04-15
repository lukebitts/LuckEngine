#include "LuckWindow.h"
#include <GL/glfw.h>
using namespace luck;

LuckWindow* LuckWindow::_instance = nullptr;

LuckWindow::LuckWindow()
{
    glfwInit();
    _running = true;
}

void LuckWindow::setWindowCaption(string caption)
{
    glfwSetWindowTitle(caption.c_str());
}

LuckWindow* createLuckWindow(u16 width, u16 height, u16 redbits, u16 greenbits, u16 bluebits,
               u16 alphabits, u16 depthbits, u16 stencilbits)
{
    LuckWindow* lkw = LuckWindow::getInstance();
    if(glfwOpenWindow( width, height, redbits, greenbits, bluebits, alphabits, depthbits, stencilbits, GLFW_WINDOW ))
    {
        lkw->width = width;
        lkw->height = height;
        return lkw;
    }
    return nullptr;
}
