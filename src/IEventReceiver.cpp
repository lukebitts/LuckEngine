#include "IEventReceiver.h"

using namespace luck;
using namespace event;

void IEventReceiver::subscribeTo(EventDispatcher* ed)
{
    _subscribedTo.push_back(ed);
}
