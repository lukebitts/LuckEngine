#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "../opengl.hpp"

#include <unordered_map>
#include <string>
#include <vector>

#include "../common/glm.hpp"

namespace luck
{
	struct texture;
	struct program;

	struct opengl_state
	{
		GLclampf alpha_ref;
		GLenum alpha_func;
		GLenum blend_sfactor;
		GLenum blend_dfactor;
		GLenum cull_mode;
		GLenum depth_func;
		bool alpha;
		bool blend;
		bool cull;
		bool depth;
		opengl_state() = default;
		static opengl_state initial_state();
		bool operator<(const opengl_state& other);
		void set_state() const;
		inline bool transparent()
		{
			return alpha || blend;
		}
	};

	struct render_pass
	{
		opengl_state state = opengl_state::initial_state();
		luck::program* program = nullptr;
		std::unordered_map<std::string, texture*> textures;
		std::unordered_map<std::string, glm::vec2> vec2;
		render_pass(luck::program* program) : program(program) {}
		bool operator<(const render_pass& other);
	};

	struct material
	{
		std::vector<render_pass> passes;
		material(render_pass pass1)
		{
			passes.push_back(pass1);
		}
	};

}

#endif // MATERIAL_HPP
