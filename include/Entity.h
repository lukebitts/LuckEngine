#ifndef ENTITY_H
#define ENTITY_H
#include <exception>
#include <iostream>
#include "luck.h"
#include "EventDispatcher.h"
#include "IComponent.h"
#include "Split.h"
namespace luck { namespace core
{
    class Entity : public event::EventDispatcher
    {
        private:
            map<string, IComponent*> _components;
            void _add(string name);
        public:
            u64 id;
            bool deleted;
            void handleEvent(string s, event::Event const& e);
            Entity* add(string components);
            Entity* remove(string name);
            Entity* requires(string components);
            template <class T>
            T* get(string name)
            {
                #ifdef _STRICT_COMPONENT_
                /// @todo extend std::exception and throw more often
                if(_components[name] == NULL)
                {
                    std::cout<<"Entity "<<id<<" doesn't have this component ("<<name<<");\n";
                    throw std::exception();
                }
                #endif
                return (T*)(_components[name]);
            }
            bool has(string name)
            {
                return _components[name];
            }
            Entity(u64 id, string components);
            ~Entity();
    };
}}
#endif
