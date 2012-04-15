#ifndef ENTITY_H
#define ENTITY_H
#include "luck.h"
#include "EventDispatcher.h"
#include "IComponent.h"
#include "Split.h"
namespace luck { namespace core
{
    class Entity : public event::EventDispatcher, public event::IEventReceiver
    {
        private:
            map<string, IComponent*> _components;
            void _add(string name);
        public:
            u64 id;
            void handleEvent(string s, event::Event* e = nullptr);
            Entity* add(string components);
            Entity* remove(string name);
            template <class T>
            T* get(string name){ return (T*)(_components[name]); }
            bool has(string name)
            {
                return _components[name];
            }
            Entity(u64 id, string components);
            ~Entity();
    };
}}
#endif
