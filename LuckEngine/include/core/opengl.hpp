#ifndef _OPENGL_HPP_
#define _OPENGL_HPP_

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <OpenGL/glext.h>
#else
#ifdef _WIN32
#include <GL/glext.h>
#endif //def _WIN32
#endif //def __APPLE__

#endif //_OPENGL_HPP_
