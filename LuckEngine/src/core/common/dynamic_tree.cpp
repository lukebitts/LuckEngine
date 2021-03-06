#include <core/common/dynamic_tree.hpp>

#include <cstring>
#include <vector>

#include <core/common/debug.hpp>
#include <core/components/spatial_component.hpp>

namespace luck
{
	///@todo fix these functions names and location(?)
	const int32_t tree_node::null_node = -1;

	enum
	{
		Left, Right, Bottom, Top, Far, Near
	};

	int halfPlaneTest(const glm::vec3& p, const glm::vec3& normal, float offset)
	{
		float dist = glm::dot(p, normal) + offset;
		if(dist > 0.02)   // Point is in front of plane
			return 1;
		else if(dist < -0.02)   // Point is behind plane
			return 0;
		return 2; // Point is on plane
	}

	enum
	{
		OUTSIDE = 0,
		INTERSECT = 3,
		INSIDE = 1
	};

	int vectorToIndex(const glm::vec3& v)
	{
		int idx = 0;
		if(v.z >= 0) idx |= 1;
		if(v.y >= 0) idx |= 2;
		if(v.x >= 0) idx |= 4;
		return idx;
	}

	int isBoxInFrustum(const glm::vec3& origin, const glm::vec3& halfDim, glm::vec4 planes[6])
	{
		//int isBoxInFrustum(const glm::aabb& aabb, const glm::vec4& planes ) const {
		static const glm::vec3 cornerOffsets[] =
		{
			glm::vec3(-1.f, -1.f, -1.f),
			glm::vec3(-1.f, -1.f, 1.f),
			glm::vec3(-1.f, 1.f, -1.f),
			glm::vec3(-1.f, 1.f, 1.f),
			glm::vec3(1.f, -1.f, -1.f),
			glm::vec3(1.f, -1.f, 1.f),
			glm::vec3(1.f, 1.f, -1.f),
			glm::vec3(1.f, 1.f, 1.f)
		};
		int ret = 1;
		for(int i = 0; i < 6; i++)
		{
			glm::vec3 planeNormal = glm::vec3(planes[i]);
			int idx = vectorToIndex(planeNormal);

			// Test the farthest point of the box from the plane
			// if it's behind the plane, then the entire box will be.
			glm::vec3 testPoint = origin + halfDim * cornerOffsets[idx];

			if(halfPlaneTest(testPoint, planeNormal, planes[i].w) == 0)
			{
				ret = 0;
				break;
			}

			// Now, test the closest point to the plane
			// If it's behind the plane, then the box is partially inside, otherwise it is entirely inside.
			idx = vectorToIndex(-planeNormal);
			testPoint = origin + halfDim * cornerOffsets[idx];

			if(halfPlaneTest(testPoint, planeNormal, planes[i].w) == 0)
			{
				ret |= 2;
			}
		}
		return ret;
	}

	std::vector<luck::entity> dynamic_tree::query_frustum(const glm::mat4& matrix) const
	{
		std::vector<luck::entity> ret;

		glm::vec4 planes[6];
		planes[Right ] = glm::vec4(matrix[0][3] - matrix[0][0],
		                           matrix[1][3] - matrix[1][0],
		                           matrix[2][3] - matrix[2][0],
		                           matrix[3][3] - matrix[3][0]);

		planes[Left  ] = glm::vec4(matrix[0][3] + matrix[0][0],
		                           matrix[1][3] + matrix[1][0],
		                           matrix[2][3] + matrix[2][0],
		                           matrix[3][3] + matrix[3][0]);

		planes[Bottom] = glm::vec4(matrix[0][3] + matrix[0][1],
		                           matrix[1][3] + matrix[1][1],
		                           matrix[2][3] + matrix[2][1],
		                           matrix[3][3] + matrix[3][1]);

		planes[Top   ] = glm::vec4(matrix[0][3] - matrix[0][1],
		                           matrix[1][3] - matrix[1][1],
		                           matrix[2][3] - matrix[2][1],
		                           matrix[3][3] - matrix[3][1]);

		planes[Far   ] = glm::vec4(matrix[0][3] - matrix[0][2],
		                           matrix[1][3] - matrix[1][2],
		                           matrix[2][3] - matrix[2][2],
		                           matrix[3][3] - matrix[3][2]);

		planes[Near  ] = glm::vec4(matrix[0][3] + matrix[0][2],
		                           matrix[1][3] + matrix[1][2],
		                           matrix[2][3] + matrix[2][2],
		                           matrix[3][3] + matrix[3][2]);
		// Normalize them
		for(int i = 0; i < 6; i++)
		{
			float invl = sqrt(planes[i].x * planes[i].x +
			                  planes[i].y * planes[i].y +
			                  planes[i].z * planes[i].z);
			planes[i] /= invl;
		}

		std::stack<int32_t> stack;
		stack.push(m_root);

		while(stack.size())
		{
			int32_t node_id = stack.top();
			stack.pop();
			if(node_id == tree_node::null_node)
			{
				continue;
			}

			const tree_node* node = m_nodes + node_id;

			glm::vec3 pos = node->aabb.getCenter();

			int result = isBoxInFrustum(pos, node->aabb.getDiagonal() / 2.f, planes);

			if(result == INSIDE)
			{
				std::stack<int32_t> all;
				all.push(node_id);

				while(all.size())
				{
					auto cur = m_nodes + all.top();
					all.pop();

					if(cur->is_leaf())
					{
						ret.push_back(cur->user_data);
					}
					else
					{
						all.push(cur->child1);
						all.push(cur->child2);
					}
				}
			}
			else if(result == INTERSECT)
			{
				if(node->is_leaf())
				{
					ret.push_back(node->user_data);
				}
				else
				{
					stack.push(node->child1);
					stack.push(node->child2);
				}
			}
		}

		return ret;
	}

	dynamic_tree::dynamic_tree()
	{
		m_root = tree_node::null_node;

		m_node_capacity = 16;
		m_node_count = 0;
		m_nodes = new tree_node[m_node_capacity];
		std::memset(m_nodes, 0, m_node_capacity * sizeof(tree_node));

		for(int32_t i = 0; i < m_node_capacity; ++i)
		{
			m_nodes[i].next = i + 1;
			m_nodes[i].height = -1;
		}
		m_nodes[m_node_capacity - 1].next = tree_node::null_node;
		m_nodes[m_node_capacity - 1].height = -1;
		m_free_list = 0;
		m_path = 0;
		m_insertion_count = 0;
	}

	dynamic_tree::~dynamic_tree()
	{
		delete[] m_nodes;
	}

	int32_t dynamic_tree::m_allocate_node()
	{
		if(m_free_list == tree_node::null_node)
		{
			ASSERT(m_node_count == m_node_capacity, "");

			tree_node* old_nodes = m_nodes;
			m_node_capacity *= 2;
			m_nodes = new tree_node[m_node_capacity];
			std::memcpy(m_nodes, old_nodes, m_node_count * sizeof(tree_node));
			delete[] old_nodes;

			for(int32_t i = m_node_count; i < m_node_capacity - 1; ++i)
			{
				m_nodes[i].next = i + 1;
				m_nodes[i].height = -1;
			}
			m_nodes[m_node_capacity - 1].next = tree_node::null_node;
			m_nodes[m_node_capacity - 1].height = -1;
			m_free_list = m_node_count;
		}

		int32_t node_id = m_free_list;
		m_free_list = m_nodes[node_id].next;
		m_nodes[node_id].parent = tree_node::null_node;
		m_nodes[node_id].child1 = tree_node::null_node;
		m_nodes[node_id].child2 = tree_node::null_node;
		m_nodes[node_id].height = 0;
		m_nodes[node_id].user_data = luck::entity {};
		++m_node_count;
		return node_id;
	}

	void dynamic_tree::m_free_node(int32_t node_id)
	{
		ASSERT(0 <= node_id && node_id < m_node_capacity, "");
		ASSERT(0 < m_node_count, "");

		m_nodes[node_id].next = m_free_list;
		m_nodes[node_id].height = -1;
		m_free_list = node_id;
		--m_node_count;
	}

	int32_t dynamic_tree::create_proxy(const glm::aabb& aabb, luck::entity user_data)
	{
		int32_t proxy_id = m_allocate_node();

		const float aabb_extension = 0.1f; //b2_aabbExtension

		glm::vec3 r {aabb_extension, aabb_extension, aabb_extension};
		m_nodes[proxy_id].aabb.min = aabb.min - r;
		m_nodes[proxy_id].aabb.max = aabb.max + r;
		m_nodes[proxy_id].user_data = user_data;
		m_nodes[proxy_id].height = 0;

		m_insert_leaf(proxy_id);

		return proxy_id;
	}

	void dynamic_tree::destroy_proxy(int32_t proxy_id)
	{
		ASSERT(0 <= proxy_id && proxy_id < m_node_capacity, "");
		ASSERT(m_nodes[proxy_id].is_leaf(), "");

		m_remove_leaf(proxy_id);
		m_free_node(proxy_id);
	}

	bool dynamic_tree::move_proxy(int32_t proxy_id, const glm::aabb& aabb, const glm::vec3& displacement)
	{
		ASSERT(0 <= proxy_id && proxy_id < m_node_capacity, "");
		ASSERT(m_nodes[proxy_id].is_leaf(), "");

		if(m_nodes[proxy_id].aabb.contains(aabb))
		{
			return false;
		}

		m_remove_leaf(proxy_id);

		const float aabb_extension = 0.1f; //b2_aabbExtension
		const float aabb_multipler = 2.0f; //b2_aabbMultiplier

		glm::aabb b = aabb;
		glm::vec3 r {aabb_extension, aabb_extension, aabb_extension};
		b.min = b.min - r;
		b.max = b.max + r;

		glm::vec3 d = aabb_multipler * displacement;

		if(d.x < 0.0f)
		{
			b.min.x += d.x;
		}
		else
		{
			b.max.x += d.x;
		}

		if(d.y < 0.0f)
		{
			b.min.y += d.y;
		}
		else
		{
			b.max.y += d.y;
		}

		m_nodes[proxy_id].aabb = b;
		m_insert_leaf(proxy_id);

		return true;
	}

	void dynamic_tree::m_insert_leaf(int32_t leaf)
	{
		++m_insertion_count;

		if(m_root == tree_node::null_node)
		{
			m_root = leaf;
			m_nodes[m_root].parent = tree_node::null_node;
			return;
		}

		glm::aabb leaf_aabb = m_nodes[leaf].aabb;
		int32_t index = m_root;
		while(m_nodes[index].is_leaf() == false)
		{
			int32_t child1 = m_nodes[index].child1;
			int32_t child2 = m_nodes[index].child2;

			float area = m_nodes[index].aabb.getPerimeter();

			glm::aabb combined_aabb;
			combined_aabb.combine(m_nodes[index].aabb, leaf_aabb);
			float combined_area = combined_aabb.getPerimeter();

			float cost = 2.0f * combined_area;
			float inheritance_cost = 2.0f * (combined_area - area);

			float cost1;
			if(m_nodes[child1].is_leaf())
			{
				glm::aabb aabb;
				aabb.combine(leaf_aabb, m_nodes[child1].aabb);
				cost1 = aabb.getPerimeter() + inheritance_cost;
			}
			else
			{
				glm::aabb aabb;
				aabb.combine(leaf_aabb, m_nodes[child1].aabb);
				float old_area = m_nodes[child1].aabb.getPerimeter();
				float new_area = aabb.getPerimeter();
				cost1 = (new_area - old_area) + inheritance_cost;
			}

			float cost2;
			if(m_nodes[child2].is_leaf())
			{
				glm::aabb aabb;
				aabb.combine(leaf_aabb, m_nodes[child2].aabb);
				cost2 = aabb.getPerimeter() + inheritance_cost;
			}
			else
			{
				glm::aabb aabb;
				aabb.combine(leaf_aabb, m_nodes[child2].aabb);
				float old_area = m_nodes[child2].aabb.getPerimeter();
				float new_area = aabb.getPerimeter();
				cost2 = new_area - old_area + inheritance_cost;
			}

			if(cost < cost1 && cost < cost2)
				break;
			if(cost1 < cost2)
				index = child1;
			else
				index = child2;
		}

		int32_t sibling = index;

		int32_t old_parent = m_nodes[sibling].parent;
		int32_t new_parent = m_allocate_node();
		m_nodes[new_parent].parent = old_parent;
		m_nodes[new_parent].user_data = luck::entity {};
		m_nodes[new_parent].aabb.combine(leaf_aabb, m_nodes[sibling].aabb);
		m_nodes[new_parent].height = m_nodes[sibling].height + 1;

		if(old_parent != tree_node::null_node)
		{
			if(m_nodes[old_parent].child1 == sibling)
				m_nodes[old_parent].child1 = new_parent;
			else
				m_nodes[old_parent].child2 = new_parent;

			m_nodes[new_parent].child1 = sibling;
			m_nodes[new_parent].child2 = leaf;
			m_nodes[sibling].parent = new_parent;
			m_nodes[leaf].parent = new_parent;
		}
		else
		{
			m_nodes[new_parent].child1 = sibling;
			m_nodes[new_parent].child2 = leaf;
			m_nodes[sibling].parent = new_parent;
			m_nodes[leaf].parent = new_parent;
			m_root = new_parent;
		}

		index = m_nodes[leaf].parent;
		while(index != tree_node::null_node)
		{
			index = m_balance(index);

			int32_t child1 = m_nodes[index].child1;
			int32_t child2 = m_nodes[index].child2;

			ASSERT(child1 != tree_node::null_node, "");
			ASSERT(child2 != tree_node::null_node, "");

			m_nodes[index].height = 1 + std::max(m_nodes[child1].height, m_nodes[child2].height);
			m_nodes[index].aabb.combine(m_nodes[child1].aabb, m_nodes[child2].aabb);

			index = m_nodes[index].parent;
		}
	}

	void dynamic_tree::m_remove_leaf(int32_t /*leaf*/)
	{

	}

	int32_t dynamic_tree::m_balance(int32_t iA)
	{
		ASSERT(iA != tree_node::null_node, "");

		tree_node* A = m_nodes + iA;
		if(A->is_leaf() || A->height < 2)
			return iA;

		int32_t iB = A->child1;
		int32_t iC = A->child2;

		ASSERT(0 <= iB && iB < m_node_capacity, "");
		ASSERT(0 <= iC && iC < m_node_capacity, "");

		tree_node* B = m_nodes + iB;
		tree_node* C = m_nodes + iC;

		int32_t balance = C->height - B->height;

		if(balance > 1)
		{
			int32_t iF = C->child1;
			int32_t iG = C->child2;
			tree_node* F = m_nodes + iF;
			tree_node* G = m_nodes + iG;

			ASSERT(0 <= iF && iF < m_node_capacity, "");
			ASSERT(0 <= iG && iG < m_node_capacity, "");

			C->child1 = iA;
			C->parent = A->parent;
			A->parent = iC;

			if(C->parent != tree_node::null_node)
			{
				if(m_nodes[C->parent].child1 == iA)
					m_nodes[C->parent].child1 = iC;
				else
				{
					ASSERT(m_nodes[C->parent].child2 == iA, "");
					m_nodes[C->parent].child2 = iC;
				}
			}
			else
			{
				m_root = iC;
			}

			if(F->height > G->height)
			{
				C->child2 = iF;
				A->child2 = iG;
				G->parent = iA;
				A->aabb.combine(B->aabb, G->aabb);
				C->aabb.combine(A->aabb, F->aabb);

				A->height = 1 + std::max(B->height, G->height);
				C->height = 1 + std::max(A->height, F->height);
			}
			else
			{
				C->child2 = iG;
				A->child2 = iF;
				F->parent = iA;
				A->aabb.combine(B->aabb, F->aabb);
				C->aabb.combine(A->aabb, G->aabb);

				A->height = 1 + std::max(B->height, F->height);
				C->height = 1 + std::max(A->height, G->height);
			}

			return iC;
		}

		if(balance < -1)
		{
			int32_t iD = B->child1;
			int32_t iE = B->child2;
			tree_node* D = m_nodes + iD;
			tree_node* E = m_nodes + iE;

			ASSERT(0 <= iD && iD < m_node_capacity, "");
			ASSERT(0 <= iE && iE < m_node_capacity, "");

			B->child1 = iA;
			B->parent = A->parent;
			A->parent = iB;

			if(B->parent != tree_node::null_node)
			{
				if(m_nodes[B->parent].child1 == iA)
					m_nodes[B->parent].child1 = iB;
				else
				{
					ASSERT(m_nodes[B->parent].child2 == iA, "");
					m_nodes[B->parent].child2 = iB;
				}
			}
			else
			{
				m_root = iB;
			}

			if(D->height > E->height)
			{
				B->child2 = iD;
				A->child1 = iE;
				E->parent = iA;
				A->aabb.combine(C->aabb, E->aabb);
				B->aabb.combine(A->aabb, D->aabb);

				A->height = 1 + std::max(C->height, E->height);
				B->height = 1 + std::max(A->height, D->height);
			}
			else
			{
				B->child2 = iE;
				A->child1 = iD;
				D->parent = iA;
				A->aabb.combine(C->aabb, D->aabb);
				B->aabb.combine(A->aabb, E->aabb);

				A->height = 1 + std::max(C->height, D->height);
				B->height = 1 + std::max(A->height, E->height);
			}

			return iB;
		}

		return iA;
	}

	int32_t dynamic_tree::height() const
	{
		if(m_root == tree_node::null_node)
			return 0;

		return m_nodes[m_root].height;
	}

}











