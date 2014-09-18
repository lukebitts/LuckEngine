#include <core/resources/resource.hpp>
#include <core/common/debug.hpp>
#include <core/opengl.hpp>
#include <tools/mesh_converter.hpp>

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

namespace luck
{
	bool mesh_data_resource::load(mesh_data_resource* r, std::string path)
	{
		auto mesh = tools::mesh::load(path);
		if(mesh.vertices.size() == 0)
			return false;
		
		r->vertices = std::move(mesh.vertices);
		for(unsigned i = 0; i < mesh.indices.size(); ++i)
		{
			r->submeshes.emplace_back();
			r->submeshes[i] = std::move(mesh.indices[i].indices);
		}

		r->calculate_aabb();

		return true;
	}

	void mesh_data_resource::calculate_aabb()
	{
		ASSERT(vertices.size() > 0, "You can't calculate the aabb of an empty mesh.'");

		glm::vec3 min {vertices[0].x, vertices[0].y, vertices[0].z};
		glm::vec3 max {min};

		for(vertex v : vertices)
		{
			min = glm::min(min, glm::vec3 {v.x, v.y, v.z});
			max = glm::max(max, glm::vec3 {v.x, v.y, v.z});
		}

		aabb = glm::aabb {min, max};
	}
}
