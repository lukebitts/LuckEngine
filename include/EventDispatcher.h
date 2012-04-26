#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H
#include "IFunctor.h"
namespace luck { namespace event
{
    class EventDispatcher
    {
        protected:
            map<string,vector<IFunctor*>> _listeners;
        public:
            void dispatchEvent(string type, Event* e = nullptr);
            void addEventListener(string type, IFunctor* callback);
            //void removeEventListener(string type);
            void removeEventListener(IFunctor* callback);
            virtual ~EventDispatcher()
            {
                _listeners.clear();
            }
    };
}}
#endif
