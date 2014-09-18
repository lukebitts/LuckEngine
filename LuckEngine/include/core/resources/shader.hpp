#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include "resources.hpp"
#include "resource.hpp"
#include "../common/debug.hpp"

namespace luck
{
	///@todo move the implementation to a cpp file
	struct shader
	{
		public:
			GLuint id = 0;
			inline shader() {}
			inline shader(const char* source, GLenum type) : id(glCreateShader(type))
			{
				glShaderSource(id, 1, &source, nullptr);
				glCompileShader(id);

#ifdef DEBUG
				GLint result = GL_FALSE;
				GLint info_log_length;

				glGetShaderiv(id, GL_COMPILE_STATUS, &result);
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_log_length);

				std::unique_ptr<char> shader_error(new char[info_log_length]);
				glGetShaderInfoLog(id, info_log_length, NULL, shader_error.get());

				WARN_IF(result == GL_FALSE, shader_error.get());
#endif
			}
			inline shader(const resource_handle<text_resource>& shader_text, GLenum type)
				: shader {shader_text.wait_load().get().text.c_str(), type}
			{

			}
			inline shader& operator=(shader && other)
			{
				id = other.id;
				other.id = 0;

				return *this;
			}
			inline shader(shader && other)
			{
				id = other.id;
				other.id = 0;
			}
			shader& operator=(const shader&) = delete;
			shader(const shader&) = delete;
			inline ~shader()
			{
				glDeleteShader(id);
			}
	};
}
#endif // _SHADER_HPP_
