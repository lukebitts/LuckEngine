#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <initializer_list>
#include <utility>

#include "resources.hpp"
#include "resource.hpp"

namespace luck
{
	struct texture ///@todo remove inline from this
	{
		public:
			typedef std::initializer_list<std::pair<GLenum, GLint>> PARAMETER_I;
			GLuint id = 0;
			texture() {}
			inline texture(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data,
						   std::initializer_list<std::pair<GLenum, GLint>> parametersi = {{GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR},{GL_TEXTURE_MAG_FILTER,GL_LINEAR}})
			{
				glGenTextures(1, &id);
				glBindTexture(GL_TEXTURE_2D, id);

				for(auto && p : parametersi)
					glTexParameteri(GL_TEXTURE_2D, p.first, p.second);

				//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
				//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, 0);
			}
			inline texture(const resource_handle<image_resource>& image, std::initializer_list<std::pair<GLenum, GLint>> parametersi = {})
				: texture {0, GL_RGBA8, (GLsizei)image.get().size.x, (GLsizei)image.get().size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)(&(image.get().pixels[0])), parametersi}
			{

			}
			inline texture& operator=(texture && other)
			{
				id = other.id;
				other.id = 0;

				return *this;
			}
			inline texture(texture && other)
			{
				id = other.id;
				other.id = 0;
			}
			texture& operator=(const texture&) = delete;
			texture(const texture&) = delete;
			inline ~texture()
			{
				glDeleteTextures(1, &id);
			}
	};
}
#endif // TEXTURE_HPP
