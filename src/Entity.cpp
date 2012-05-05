#include "Entity.h"

using namespace luck;
using namespace core;

void Entity::_add(string name)
{
    detail::ComponentRegistry& reg = detail::getComponentRegistry();
    detail::ComponentRegistry::iterator it = reg.find(name);

    if (it == reg.end())// This happens when there is no component registered to this name.
    {
        _components[name] = new FlagComponent();
        return;
    }

    detail::CreateComponentFunc func = it->second;
    IComponent* c = func();
    c->owner = this;
    c->init();
    if(_components[name]) remove(name);
    _components[name] = c;
}

void Entity::handleEvent(string s, event::Event const& e)
{
    dispatchEvent(s, e);
}

Entity* Entity::add(string components)
{
    vector<string> x = split(components, ' ');
    for(u16 i = 0; i < x.size(); i++)
    {
        _add(x[i]);
    }
    return this;
}

Entity* Entity::remove(string name)
{
    map<string, IComponent*>::iterator it = _components.find(name);
    if(it == _components.end()) return this;
    //removeEventListener(it->second);
    delete _components[it->first];
    _components.erase(it);

    return this;
}

Entity* Entity::requires(string components)
{
    vector<string> x = split(components, ' ');
    for(u16 i = 0; i < x.size(); i++)
    {
        if(!has(x[i]))
            _add(x[i]);
    }
    return this;
}

Entity::Entity(u64 id, string components)
{
    this->id = id;
    add(components);
}

Entity::~Entity()
{
    ///@todo Send destroy event
    /// Is sending a destroy event worth it? Because we are calling the
    /// deconstructor on each component, that should be enough to warn then
    /// that their life is over
    ///@todo Delete every listener
    //_listeners.clear();
    for(auto it = _components.begin(); it != _components.end(); it++)
        delete _components[it->first];
    _components.clear();
}
