#ifndef SHAPE_COMPONENT_HPP
#define SHAPE_COMPONENT_HPP

#include <bullet/btBulletDynamicsCommon.h>

namespace luck
{
	struct base_shape_component : public luck::component<base_shape_component>
	{
		///@todo create the other shapeComponents allowed by Bullet
	protected:
		btCollisionShape* m_shape = nullptr;
		base_shape_component(btCollisionShape* shape, glm::vec3 offset = glm::vec3{0.f,0.f,0.f}) //: m_shape(shape) 
		{
			this->shape(shape, offset);
		}
		virtual ~base_shape_component()
		{
			delete m_shape;
		}
		void shape(btCollisionShape* shape, glm::vec3 offset = glm::vec3{ 0.f, 0.f, 0.f })
		{
			if(!shape) return;

			delete m_shape;

			m_shape = shape;

			/*btCompoundShape* cmpShape = new btCompoundShape();
			btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3(offset.x,offset.y,offset.z));
			cmpShape->addChildShape(t, shape);

			m_shape = cmpShape;*/
		}
	public:
		btCollisionShape* shape() const
		{
			return m_shape;
		}
	};

	struct sphere_shape_component : base_shape_component
	{
		sphere_shape_component(float radius = 1.f) : base_shape_component(new btSphereShape(radius)) {}
	};

	struct capsule_shape_component : base_shape_component
	{
		capsule_shape_component(float radius = 1.f, float height = 1.f, glm::vec3 offset = glm::vec3{ 0.f, 0.f, 0.f }) : base_shape_component(new btCapsuleShape(radius, height), offset) {}
	};

	struct box_shape_component : base_shape_component
	{
		box_shape_component(glm::vec3 halfExtents = glm::vec3(1.f, 1.f, 1.f)) : base_shape_component(new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z))) {}
	};

	struct static_plane_shape_component : base_shape_component
	{
		static_plane_shape_component() : base_shape_component(new btStaticPlaneShape(btVector3(0, 1, 0), 0)) {}
	};

	struct static_triangle_mesh_shape_component : base_shape_component
	{
		btTriangleMesh* m_mesh = nullptr;
		static_triangle_mesh_shape_component(luck::resource_handle<luck::mesh_data_resource> mesh) : base_shape_component(nullptr), m_mesh(triangle_mesh_from_mesh(mesh))
		{
			//m_shape = new btBvhTriangleMeshShape(m_mesh, false);
			this->shape(new btBvhTriangleMeshShape(m_mesh, false));
		}
		static btTriangleMesh* triangle_mesh_from_mesh(luck::resource_handle<luck::mesh_data_resource> mesh)
		{
			auto tmesh = new btTriangleMesh();

			for (auto faces : mesh.get().submeshes)
			{
				for (size_t i = 0; i < faces.size(); i += 3)
				{
					auto v1 = btVector3(mesh.get().vertices[faces[i]].x, mesh.get().vertices[faces[i]].y, mesh.get().vertices[faces[i]].z);
					auto v2 = btVector3(mesh.get().vertices[faces[i + 1]].x, mesh.get().vertices[faces[i + 1]].y, mesh.get().vertices[faces[i + 1]].z);
					auto v3 = btVector3(mesh.get().vertices[faces[i + 2]].x, mesh.get().vertices[faces[i + 2]].y, mesh.get().vertices[faces[i + 2]].z);
					tmesh->addTriangle(v1, v2, v3, false);
				}
			}

			return tmesh;
		}
		virtual ~static_triangle_mesh_shape_component()
		{
			delete m_mesh;
		}
	};
}

#endif