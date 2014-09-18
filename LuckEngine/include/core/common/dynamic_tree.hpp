#ifndef _DYNAMIC_TREE_HPP
#define _DYNAMIC_TREE_HPP

#include <cstdint>
#include <stack>

#include "../../luck.hpp"
#include "../opengl.hpp"
#include "../common/glm.hpp"
#include "../common/aabb.hpp"
#include "../common/debug.hpp"

/// Modified b2dynamic_tree from Box2D's implementation of Natahanael Presson's btDbvt.
/// Changed to allow 3 dimensions and remove dependency on Box2D types

namespace luck
{

	struct tree_node
	{
		static const int32_t null_node;
		bool is_leaf() const
		{
			return child1 == null_node;
		}
		glm::aabb aabb;
		luck::entity user_data;
		union
		{
			int32_t parent;
			int32_t next;
		};
		int32_t child1;
		int32_t child2;
		int32_t height;
	};

	class dynamic_tree ///@todo finish implementing every method
	{
		public:
			dynamic_tree();
			~dynamic_tree();
			int32_t create_proxy(const glm::aabb& aabb, luck::entity user_data);
			void destroy_proxy(int32_t proxy_id);
			bool move_proxy(int32_t proxy_id, const glm::aabb& aabb1, const glm::vec3& displacement);
			luck::entity user_data(int32_t proxy_id) const;
			const glm::aabb& fat_aabb(int32_t proxy_id) const;
			template <typename T>
			void query(T callback, const glm::aabb& aabb) const;
			std::vector<luck::entity> query_frustum(const glm::mat4& matrix) const;
			//template <typename T>
			//void ray_cast(T callback, const RayCastInput& input);
			//void validate() const;
			int32_t height() const;
			int32_t max_balance() const;
			float area_ration() const;
			//void rebuild_bottom_up();
			void shift_origin(const glm::vec3& new_origin);
		private:
			int32_t _allocate_node();
			void _free_node(int32_t node);
			void _insert_leaf(int32_t node);
			void _remove_leaf(int32_t node);
			int32_t _balance(int32_t index);
			int32_t _compute_height() const;
			int32_t _compute_height(int32_t node_id) const;
			//void _validate_structure(int32_t index) const;
			//void _validate_metrics(int32_t index) const;
			int32_t _root;
			tree_node* _nodes;
			int32_t _node_count;
			int32_t _node_capacity;
			int32_t _free_list;
			uint32_t _path;
			int32_t _insertion_count;
	};

	inline luck::entity dynamic_tree::user_data(int32_t proxy_id) const
	{
		ASSERT(0 <= proxy_id && proxy_id < _node_capacity, "Invalid proxy_id");
		return _nodes[proxy_id].user_data;
	}

	inline const glm::aabb& dynamic_tree::fat_aabb(int32_t proxy_id) const
	{
		ASSERT(0 <= proxy_id && proxy_id < _node_capacity, "Invalid proxy_id");
		return _nodes[proxy_id].aabb;
	}

	template <typename T>
	inline void dynamic_tree::query(T callback, const glm::aabb& aabb) const
	{
		std::stack<int32_t> stack;
		stack.push(_root);

		while(stack.size())
		{
			int32_t node_id = stack.top();
			stack.pop();
			if(node_id == tree_node::null_node)
			{
				continue;
			}

			const tree_node* node = _nodes + node_id;

			if(node->aabb.overlaps(aabb))
			{
				if(node->is_leaf())
				{
					bool proceed = callback(node_id);
					if(!proceed)
						return;
				}
				else
				{
					stack.push(node->child1);
					stack.push(node->child2);
				}
			}
		}
	}

}

#endif // _DYNAMIC_TREE_HPP
