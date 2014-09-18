#ifndef _PROGRAM_HPP_
#define _PROGRAM_HPP_

#include "../opengl.hpp"
#include "shader.hpp"

namespace luck
{
	///@todo put the implementation in a cpp file
	struct program
	{
		GLuint id = 0;
		inline program() {}
		inline program(GLuint vid, GLuint fid) : id(glCreateProgram())
		{
			glAttachShader(id, vid);
			glAttachShader(id, fid);
			glLinkProgram(id);
		}
		inline program(const shader& vertex_shader, const shader& fragment_shader)
			: program(vertex_shader.id, fragment_shader.id)
		{

		}
		inline program& operator=(program && other)
		{
			id = other.id;
			other.id = 0;

			return *this;
		}
		inline program(program && other)
		{
			id = other.id;
			other.id = 0;
		}
		program& operator=(const program&) = delete;
		program(const program&) = delete;
		inline ~program()
		{
			glDeleteProgram(id);
		}
	};
}

#endif // _PROGRAM_HPP_
