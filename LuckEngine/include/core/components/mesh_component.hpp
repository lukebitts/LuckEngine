#ifndef MESH_COMPONENT_HPP
#define MESH_COMPONENT_HPP

#include "../../luck.hpp"
#include "../resources/material.hpp"

namespace luck
{
	struct mesh;

	struct mesh_component : public anax::Component<mesh_component>
	{
		luck::mesh* mesh = nullptr;
		std::vector<luck::material> materials;
		inline mesh_component(luck::mesh* mesh) : mesh(mesh) {}
		inline mesh_component(luck::mesh* mesh, luck::material material) : mesh(mesh), materials({ material }) {}
		inline mesh_component(luck::mesh* mesh, std::vector<luck::material> materials) : mesh(mesh), materials(materials) {}
	};
}
#endif // MESH_COMPONENT_HPP_
