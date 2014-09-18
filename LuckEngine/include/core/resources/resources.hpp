#ifndef _RESOURCES_HPP_
#define _RESOURCES_HPP_

#include <unordered_map>
#include <string>
#include <memory>
#include <boost/signals2.hpp>

#include "../common/debug.hpp"

namespace luck
{

	class resources;
	struct resource;

	template <class T>
	struct resource_handle
	{
		private:
			const resources* _r = nullptr;
			std::string _path = "";
		public:
			resource_handle(const resources* r, std::string path) : _r(r), _path(path) {}
			resource_handle() {}
			T& get() const;
			resource_handle<T> wait_load() const;
	};

	class resources
	{
		private:
			std::unordered_map<std::string, std::unique_ptr<resource>> _resources;
			template<class T>
			inline T* _get(std::string path) const
			{
				if(_resources.find(path) != _resources.end())
					return static_cast<T*>(_resources.at(path).get());
				return nullptr;
			}
			template <class T>
			friend struct resource_handle;
		public:
			boost::signals2::signal<void(std::string)> on_load;
			boost::signals2::signal<void(std::string)> on_load_error;
			template <class T, class ... Params>
			inline bool load(std::string path, Params && ... params)
			{
				/*if(_resources.find(path) != _resources.end())
				{
					on_load(path);
					return true;
				}*/
					
				std::unique_ptr<T> t {new T{}};
				if(T::load(t.get(), path, std::forward<Params>(params)...))
				{
					_resources[path] = std::move(t);
					on_load(path);
					return true;
				}
				on_load_error(path);
				return false;
			}
			template <class T>
			inline resource_handle<T> get(std::string path) const
			{
				return resource_handle<T>(this, path);
			}
	};

	template<class T>
	T& resource_handle<T>::get() const
	{
		static T default_return {};
		auto resource = _r ? static_cast<T*>(_r->_get<T>(_path)) : nullptr;
		WARN_IF(!resource, "Could not find resource at ", _path);
		return resource ? *resource : default_return;
	}

	template<class T>
	resource_handle<T> resource_handle<T>::wait_load() const
	{
		while(_r->_get<T>(_path) == nullptr);
		return resource_handle<T>(*this);
	}

}

#endif //_RESOURCES_HPP_
