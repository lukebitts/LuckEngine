#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifdef DEBUG

#include <cassert>
#include <vector>
#include <iostream>
#include <iomanip>
#include "glm.hpp"
#include "../../tools/tools.hpp"

#endif

namespace luck
{
#ifdef DEBUG
	struct debug
	{
		private:
			template <class First, class ... Rest>
			static void m_log(First f, Rest ... r)
			{
				m_tlog<First>(f);
				m_log(r...);
			}
			static void m_log()
			{
				std::cout << std::endl;
			}
			template <class T>
			static void m_tlog(T t)
			{
				std::cout << t;
			}
		public:
			template <class ... T>
			static void log(T ... t)
			{
				m_log(t...);
			}
	};

	template <>
	inline void debug::m_tlog(glm::vec3 v)
	{
		std::cout << "vec3 (" << v.x << "," << v.y << "," << v.z << ")";
	}

	template <>
	inline void debug::m_tlog(glm::quat v)
	{
		std::cout << "quat (" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
	}

	template <>
	inline void debug::m_tlog(std::vector<std::string> v)
	{
		m_tlog("vector (");
		for(size_t i = 0; i < v.size(); ++i)
		{
			m_tlog(v[i]);
			if(i+1 < v.size()) m_tlog(",");
		}
		m_tlog(")");
	}
	
#endif
}

#ifdef DEBUG

#ifdef _WIN32
#define LOG(...) luck::debug::log(luck::tools::wget_file_from_path(__FILE__),"[",__LINE__, "]: ", __VA_ARGS__)
#else //!_WIN32
#ifdef __APPLE__
#define LOG(...) luck::debug::log(luck::tools::get_file_from_path(__FILE__),"[",__LINE__, "]: ", __VA_ARGS__)
#endif //_WIN32
#endif //__MAC__

#define STATIC_ASSERT(x,message) static_assert(x, message)
#define ASSERT(x,message) assert((x) && message)
#define WARN_IF(x,...) {if(x) LOG(__VA_ARGS__);}

#else //!DEBUG

#define LOG(...)
#define STATIC_ASSERT(x,message)
#define ASSERT(x,message)
#define WARN_IF(x,...)

#endif //DEBUG

#endif // DEBUG_HPP
