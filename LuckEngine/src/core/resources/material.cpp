#include <core/resources/material.hpp>

#include <cstring>
#include <utility>

namespace luck
{

	opengl_state opengl_state::initial_state() ///@todo if the initial_state is not the same as opengl's initial state, we gotta disable stuff instead of enable
	{
		opengl_state state {};
		std::memset(&state, 0, sizeof(opengl_state));

		state.alpha_ref = GL_ALWAYS;
		state.alpha_func = 0;
		state.blend_sfactor = GL_ONE;
		state.blend_dfactor = GL_ZERO;
		state.cull_mode = GL_BACK;
		state.depth_func = GL_LESS;
		state.alpha = false;
		state.blend = false;
		state.cull = true;
		state.depth = true;

		return state;
	}

	bool opengl_state::operator<(const opengl_state& other)
	{
		return std::tie(alpha, blend) < std::tie(other.alpha, other.blend);
	}

	void opengl_state::set_state() const
	{
		if(alpha)
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(alpha_func, alpha_ref);
		}
		if(blend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(blend_sfactor, blend_dfactor);
		}
		if(cull)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(cull_mode);
		}
		if(depth)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(depth_func);
		}
	}

	bool render_pass::operator<(const render_pass& other)
	{
		return state < other.state;
	}

}
