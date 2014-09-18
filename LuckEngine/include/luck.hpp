#ifndef _LUCK_HPP_
#define _LUCK_HPP_

#include <anax/anax.hpp>

namespace luck
{
	//anax types
	typedef anax::Entity entity;
	template <class T>
	using component = anax::Component<T>;
	template <class T>
	using system = anax::System<T>;
	typedef anax::ComponentFilter component_filter;
	typedef anax::World world;
}

#endif //_LUCK_HPP_
