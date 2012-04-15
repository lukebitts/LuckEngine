#ifndef DETAIL_H
#define DETAIL_H
#include <utility>
#include "core.h"
namespace luck { namespace core { namespace detail
{
    typedef IComponent* (*CreateComponentFunc)();
    typedef map<string, CreateComponentFunc> ComponentRegistry;

    inline ComponentRegistry& getComponentRegistry()
    {
        static ComponentRegistry reg;
        return reg;
    }

    template<class T>
    IComponent* createComponent() {
        return new T;
    }

    template<class T>
    struct RegistryEntry
    {
      public:
        static RegistryEntry<T>& Instance(const std::string& name)
        {
            // Because I use a singleton here, even though `COMPONENT_REGISTER`
            // is expanded in multiple translation units, the constructor
            // will only be executed once. Only this cheap `Instance` function
            // (which most likely gets inlined) is executed multiple times.

            static RegistryEntry<T> inst(name);
            return inst;
        }

      private:
        RegistryEntry(const std::string& name)
        {
            ComponentRegistry& reg = getComponentRegistry();
            CreateComponentFunc func = createComponent<T>;

            std::pair<ComponentRegistry::iterator, bool> ret =
                reg.insert(ComponentRegistry::value_type(name, func));

            if (ret.second == false) {
                // This means there already is a component registered to
                // this name. You should handle this error as you see fit.
            }
        }

        RegistryEntry(const RegistryEntry<T>&) = delete; // C++11 feature
        RegistryEntry& operator=(const RegistryEntry<T>&) = delete;
    };

}}}

#endif
