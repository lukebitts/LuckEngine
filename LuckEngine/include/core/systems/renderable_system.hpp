#ifndef _RENDERABLE_SYSTEM_HPP_
#define _RENDERABLE_SYSTEM_HPP_

#include "../../luck.hpp"
#include "../resources/resources.hpp"
#include "../resources/resource.hpp"
#include "../resources/material.hpp"

namespace luck
{
	struct spatial_component;
	struct mesh_component;
	class program_resource;
	class spatial_system;
	struct texture;
	struct mesh;
	struct program;

	namespace detail
	{
		struct renderable_state_reference
		{
			luck::entity entity;
			int pass;
			int submesh;
		};
		
		struct uniform
		{
			luck::entity entity;
			std::unordered_map<std::string, luck::texture*> textures;
			std::unordered_map<std::string, glm::vec2> vec2;
		};
		
		struct mesh
		{
			luck::mesh* m;
			int submesh;
			std::vector<detail::uniform> uniforms;
		};
		
		struct state
		{
			luck::opengl_state s;
			std::vector<detail::mesh> meshs;
		};
		
		struct program
		{
			luck::program* p;
			std::vector<detail::state> states;
		};
	}

	class renderable_system : public luck::system<renderable_system>
	{
		private:
			spatial_system* _spatial_system;
		public:
			//resource_handle<ProgramResource> _p;
			renderable_system(spatial_system* s);
			void render(luck::entity current_camera);
			void onEntityAdded(luck::entity& e) override;
			static bool sort_renderables(const luck::entity& a, const luck::entity& b);
			static std::vector<detail::program> sort_solids(const std::vector<detail::renderable_state_reference>& solid_renderables);
	};
}
#endif // _RENDERABLE_SYSTEM_HPP_
