#ifndef IFUNCTOR_H
#define IFUNCTOR_H
#include "luck.h"
#include "event.h"
namespace luck { namespace event
{
    class IFunctor
    {
        public:
            virtual void operator()(Event* e)=0;  // call using operator
            virtual void Call(Event* e)=0;
    };

    template <class T>
    class EventCallback : public IFunctor
    {
        private:
            void (T::*fpt)(Event*);
            T* pt2Object;
        public:
            EventCallback(T* _pt2Object, void(T::*_fpt)(Event*))
            {
                pt2Object = _pt2Object;
                fpt=_fpt;
            };
            virtual void operator()(Event* e)
            {
                (*pt2Object.*fpt)(e);
            };
            virtual void Call(Event* e)
            {
                (*pt2Object.*fpt)(e);
            };
    };

    template <typename T>
    IFunctor* eventCallback(T* obj,void(T::*_fpt)(Event*))
    {
        return new EventCallback<T>(obj,_fpt);
    }
}}
#endif
