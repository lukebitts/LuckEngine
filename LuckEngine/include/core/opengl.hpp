#ifndef OPENGL_HPP
#define OPENGL_HPP

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <OpenGL/glext.h>
#else
#ifdef _WIN32
#include <GL/glext.h>
#endif //def _WIN32
#endif //def __APPLE__

#endif //OPENGL_HPP
