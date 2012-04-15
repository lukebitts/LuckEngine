#ifndef POSITIONCOMPONENT
#define POSITIONCOMPONENT
#include "luck.h"
#include "IComponent.h"
#include "Entity.h"
#include "Vector3.h"
namespace luck { namespace core
{
    //TODO: Add a parent to the component, so it will be in relative position this parent
    Component(Position)
    {
        Vector3<f32> _position;
        Vector3<f32> _rotation;
        void init(){}
        void handleEvent(string name, event::Event* e){}
        Position* position(Vector3<f32> position){ _position = position; return this; }
        Position* rotation(Vector3<f32> rotation){ _rotation = rotation; return this; }
        Position* lookAt(Vector3<f32> target)
        {
            //TODO
            return this;
        }
    };
}}
#endif
