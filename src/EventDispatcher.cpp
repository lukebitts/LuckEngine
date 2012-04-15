#include "EventDispatcher.h"

using namespace luck;
using namespace event;

void EventDispatcher::dispatchEvent(string type, Event* e)
{
    vector<IEventReceiver*> v = _listeners[type];
    for(auto it = v.begin(); it != v.end(); it++)
    {
        (*it)->handleEvent(type,e);
    }
}

void EventDispatcher::addEventListener(string type, IEventReceiver* callback)
{
    _listeners[type].push_back(callback);
    callback->subscribeTo(this);
}

void EventDispatcher::removeEventListener(IEventReceiver* callback)
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
