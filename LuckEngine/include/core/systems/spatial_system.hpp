#ifndef SPATIAL_SYSTEM_HPP
#define SPATIAL_SYSTEM_HPP

#include "../../luck.hpp"
#include "../common/dynamic_tree.hpp"

namespace luck
{
	struct spatial_component;

	class spatial_system : public luck::system<spatial_system>
	{
		public:
			dynamic_tree m_tree;
		public:
			spatial_system();
			void onEntityAdded(luck::entity& e) override;
			void onEntityRemoved(luck::entity& e) override;
			void update();
			void onSpatialDestroy(spatial_component& c);
	};
}
#endif // SPATIAL_SYSTEM_HPP
