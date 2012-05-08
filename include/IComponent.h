#ifndef ICOMPONENT_H
#define ICOMPONENT_H
#include "IFunctor.h"
#include "detail.h"
namespace luck { namespace core
{
    class Entity;
    struct IComponent
    {
        Entity* owner;
        virtual void init() = 0;
        virtual ~IComponent(){}
    };

    #define COMPONENT_REGISTER(TYPE)                                          \
    namespace core {                                                          \
    namespace detail {                                                        \
    namespace                                                                 \
    {                                                                         \
        template<class T>                                                     \
        class ComponentRegistration;                                          \
                                                                              \
        template<>                                                            \
        class ComponentRegistration<TYPE>                                     \
        {                                                                     \
            static const luck::core::detail::RegistryEntry<TYPE>& reg;        \
        };                                                                    \
                                                                              \
        const luck::core::detail::RegistryEntry<TYPE>&                        \
            ComponentRegistration<TYPE>::reg =                                \
                luck::core::detail::RegistryEntry<TYPE>::Instance(#TYPE);     \
    }}}

    #define Component(TYPE)             \
    struct TYPE;                        \
    COMPONENT_REGISTER(TYPE);           \
    struct TYPE : public IComponent

    Component(FlagComponent)
    {
        void init(){}
        ~FlagComponent(){}
    };

}}
#endif
