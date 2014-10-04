#ifndef LUCK_HPP
#define LUCK_HPP

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

	///@todo finish the memberfy macro and use it inside components
	#define MEMBERFY(SELF, TYPE, NAME) private: TYPE m_##NAME; public: TYPE NAME() { return m_##NAME; }  SELF& NAME(TYPE value) { m_##NAME = value; return *this; }
}

#endif //LUCK_HPP
