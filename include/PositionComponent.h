#ifndef POSITIONCOMPONENT
#define POSITIONCOMPONENT
#include "luck.h"
#include "IComponent.h"
#include "Vector3.h"
namespace luck { namespace core
{
    Component(Position)
    {
        luck::core::Vector3<f32> position;
        luck::core::Vector3<f32> rotation;
        void init(){}
        void handleEvent(string name, event::Event* e){}
        luck::core::Vector3<f32> pos(){ return position; }
        Position* pos(luck::core::Vector3<f32> position){ this->position = position; return this; }
    };
}}
#endif
