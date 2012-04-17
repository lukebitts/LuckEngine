#ifndef IEVENTRECEIVER_H
#define IEVENTRECEIVER_H
#include "luck.h"
#include "event.h"
namespace luck { namespace event
{
    class IEventReceiver
    {
        protected:
            vector<EventDispatcher*> _subscribedTo;
        public:
            virtual void handleEvent(string type, Event* event = nullptr) = 0;
            void subscribeTo(EventDispatcher* ed);
            virtual ~IEventReceiver();
    };
}}
#endif
