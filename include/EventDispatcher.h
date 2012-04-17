#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H
#include "IEventReceiver.h"
namespace luck { namespace event
{
    class EventDispatcher
    {
        protected:
            map<string,vector<IEventReceiver*>> _listeners;
        public:
            void dispatchEvent(string type, Event* e = nullptr);
            void addEventListener(string type, IEventReceiver* callback);
            //void removeEventListener(string type);
            void removeEventListener(IEventReceiver* callback);
            virtual ~EventDispatcher()
            {
                _listeners.clear();
            }
    };
}}
#endif
