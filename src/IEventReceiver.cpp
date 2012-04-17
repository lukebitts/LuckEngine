#include "IEventReceiver.h"

using namespace luck;
using namespace event;

void IEventReceiver::subscribeTo(EventDispatcher* ed)
{
    _subscribedTo.push_back(ed);
}

IEventReceiver::~IEventReceiver()
{
    /*for(auto it = _subscribedTo.begin(); it != _subscribedTo.end(); it++)
        (*it)->removeEventListener(this);*/
    //error: invalid use of imcomplete type 'struct luck::event::EventDispatcher'
}
