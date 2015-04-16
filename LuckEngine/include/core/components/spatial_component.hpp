#ifndef SPATIAL_COMPONENT_HPP
#define SPATIAL_COMPONENT_HPP

#include "../../luck.hpp"
#include "../common/glm.hpp"
#include "../common/aabb.hpp"

namespace luck
{
	class spatial_system;

	struct spatial_component : public luck::component<spatial_component>
	{
		public:
			luck::entity entity;
			glm::vec3 position {};
			glm::quat rotation {}; ///@todo rename rotation to orientation
			glm::vec3 scale{1, 1, 1};
			glm::aabb aabb{glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1)};
			inline spatial_component(glm::vec3 position = glm::vec3(), glm::quat rotation = glm::quat (),
			                         glm::vec3 scale = glm::vec3(1, 1, 1), glm::aabb aabb = glm::aabb (glm::vec3( -1, -1, -1), glm::vec3(1, 1, 1)))
				: position(position), rotation(rotation), scale(scale), aabb(aabb), m_position(position), m_rotation(rotation), m_scale(scale), m_aabb(aabb)
			{
			}
			spatial_component(const spatial_component&) = delete;
			spatial_component& operator=(const spatial_component&) = delete;
			void look_at(glm::vec3 look_vector, glm::vec3 up_vector = glm::vec3(0,1,0));
			virtual ~spatial_component();

			inline void remove_parent()
			{

			}
			inline void parent(luck::entity parent)
			{
				if (!parent.isValid() || !parent.hasComponent<luck::spatial_component>())
					return;

				auto& parent_spatial = parent.getComponent<luck::spatial_component>();
				
				this->m_parent = parent;
				parent_spatial.m_children.push_back(entity);
			}
			inline luck::entity parent() const { return m_parent; }
		private:
			luck::entity m_parent;
			std::vector<luck::entity> m_children;

			friend class spatial_system;
			friend class renderable_system;

			spatial_system* m_system = nullptr;
			int m_proxy = -1;
			glm::vec3 m_position {};
			glm::quat m_rotation {};
			glm::vec3 m_scale {1, 1, 1};
			glm::aabb m_aabb {glm::vec3{ -1, -1, -1}, glm::vec3{1, 1, 1}};
			inline void m_difference(glm::vec3& position, glm::quat& rotation, glm::vec3& scale, glm::aabb& aabb)
			{
				position = this->position - m_position;
				rotation = glm::inverse(m_rotation) * this->rotation;//glm::quat{0,0,0,1};//this->rotation;
				scale = this->scale - m_scale;

				for (auto& child : m_children) ///@todo this won't work without the rotation
				{
					auto& child_spatial = child.getComponent<luck::spatial_component>();
					//child_spatial.position += position;
					//child_spatial.rotation *= rotation;

					glm::mat4 self_model;
					self_model = glm::translate(self_model, this->m_position);
					//self_model = self_model * glm::toMat4(this->m_rotation);
					self_model = glm::scale(self_model, this->m_scale);

					glm::mat4 child_model;
					child_model = glm::translate(child_model, child_spatial.position - this->m_position + position);
					//child_model = child_model * glm::toMat4(rotation * child_spatial.rotation);
					child_model = glm::scale(child_model, child_spatial.scale - this->m_scale + scale);

					child_model = child_model * self_model;


					// extract translation
					child_spatial.position.x = child_model[3][0];
					child_spatial.position.y = child_model[3][1];
					child_spatial.position.z = child_model[3][2];

					// extract the rows of the matrix

					/*Vec3f columns[3] = {
						matrix.getColumn(0).xyz(),
						matrix.getColumn(1).xyz(),
						matrix.getColumn(2).xyz()
					};

					// extract the scaling factors
					scaling.x = columns[0].length();
					scaling.y = columns[1].length();
					scaling.z = columns[2].length();

					// and remove all scaling from the matrix
					if (scaling.x)
					{
						columns[0] /= scaling.x;
					}
					if (scaling.y)
					{
						columns[1] /= scaling.y;
					}
					if (scaling.z)
					{
						columns[2] /= scaling.z;
					}

					// build a 3x3 rotation matrix
					Matrix33f m(columns[0].x, columns[1].x, columns[2].x,
						columns[0].y, columns[1].y, columns[2].y,
						columns[0].z, columns[1].z, columns[2].z, true);

					// and generate the rotation quaternion from it
					rotation = Quatf(m);*/

				}

				this->aabb = this->m_aabb;
				this->aabb.rotate(m_rotation);
				this->aabb.translate(m_position);
				this->aabb.scale(m_scale, m_position);
				aabb = this->aabb;

				m_position = this->position;
				m_rotation = this->rotation;
				m_scale = this->scale;
			}
	};
}
#endif //SPATIAL_COMPONENT_HPP
