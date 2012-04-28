#include "EventDispatcher.h"
using namespace luck;
using namespace event;

void EventDispatcher::dispatchEvent(string type, Event const& e)
{
    for(u16 i = 0; i < _listeners[type].size(); i++)
    {
        (*_listeners[type][i])(e);
    }
}

void EventDispatcher::addEventListener(string type, IFunctor* callback)
{
    _listeners[type].push_back(callback);
}

void EventDispatcher::removeEventListener(IFunctor* callback)
{
    //recursively remove the callback from the _listeners list
    for(auto mIt = _listeners.begin(); mIt != _listeners.end(); mIt++)
        for(auto it = _listeners[mIt->first].begin(); it != _listeners[mIt->first].end(); it++)
            if(*it == callback)
            {
                _listeners[mIt->first].erase(it);
                return;
            }
}
