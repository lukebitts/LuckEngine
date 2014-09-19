#ifndef _DEBUG_HPP_
#define _DEBUG_HPP_

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
			static void _log(First f, Rest ... r)
			{
				_tlog<First>(f);
				_log(r...);
			}
			static void _log()
			{
				std::cout << std::endl;
			}
			template <class T>
			static void _tlog(T t)
			{
				std::cout << t;
			}
		public:
			template <class ... T>
			static void log(T ... t)
			{
				_log(t...);
			}
	};

	template <>
	inline void debug::_tlog(glm::vec3 v)
	{
		std::cout << "vec3 (" << v.x << "," << v.y << "," << v.z << ")";
	}

	template <>
	inline void debug::_tlog(glm::quat v)
	{
		std::cout << "quat (" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
	}

	template <>
	inline void debug::_tlog(std::vector<std::string> v)
	{
		_tlog("vector (");
		for(size_t i = 0; i < v.size(); ++i)
		{
			_tlog(v[i]);
			if(i+1 < v.size()) _tlog(",");
		}
		_tlog(")");
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

#endif // _DEBUG_HPP_
