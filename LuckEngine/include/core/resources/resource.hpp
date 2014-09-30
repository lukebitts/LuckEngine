#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <vector>
#include <string>

#include "../opengl.hpp"
#include "../common/aabb.hpp"

namespace luck
{

	struct resource
	{
		static bool load(resource*, std::string);
		resource() {}
		virtual ~resource() {}
		resource& operator=(const resource&) = delete;
		resource(const resource&) = delete;
	};

	struct text_resource : public resource
	{
		public:
			std::string text = "";
			static bool load(text_resource* r, std::string path);
	};

	struct image_resource : public resource
	{
		public:
			glm::vec2 size;
			std::vector<unsigned char> pixels;
			static bool load(image_resource* r, std::string path);
	};

	struct mesh_data_resource : public resource
	{
		public:
			struct vertex
			{
				GLfloat x, y, z;
				GLfloat u, v;
				GLfloat nx, ny, nz;
				GLfloat tx, ty, tz;
				GLfloat bx, by, bz;
			};
			std::vector<vertex> vertices;
			//std::vector<GLuint> indices;
			std::vector<std::vector<GLuint>> submeshes;
			glm::aabb aabb;
			static bool load(mesh_data_resource* r, std::string path);
			void calculate_aabb();
	};

}

#endif //RESOURCE_HPP


