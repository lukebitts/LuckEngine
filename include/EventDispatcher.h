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
            void dispatchEvent(string type, Event const& e);
            void addEventListener(string type, IFunctor* callback);
            //void removeEventListener(string type);
            void removeEventListener(IFunctor* callback);
            virtual ~EventDispatcher()
            {
                for(auto mIt = _listeners.begin();mIt != _listeners.end();mIt++)
                {
                    for(u16 i = 0; i < _listeners[mIt->first].size(); i++)
                        delete _listeners[mIt->first][i];
                    _listeners[mIt->first].clear();
                }
                _listeners.clear();
            }
    };
}}
#endif
