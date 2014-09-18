#ifndef _SPATIAL_SYSTEM_HPP_
#define _SPATIAL_SYSTEM_HPP_

#include "../../luck.hpp"
#include "../common/dynamic_tree.hpp"

namespace luck
{
	struct spatial_component;

	class spatial_system : public luck::system<spatial_system>
	{
		public:
			dynamic_tree _tree;
		public:
			spatial_system();
			void onEntityAdded(luck::entity& e) override;
			void onEntityRemoved(luck::entity& e) override;
			void update();
			void onSpatialDestroy(spatial_component& c);
	};
}
#endif // _SPATIAL_SYSTEM_HPP
