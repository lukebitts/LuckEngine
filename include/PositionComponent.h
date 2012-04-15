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
        Vector3<f32> position;
        Vector3<f32> rotation;
        void init(){}
        void handleEvent(string name, event::Event* e){}
        Vector3<f32> pos(){ return position; }
        Position* pos(Vector3<f32> position){ this->position = position; return this; }
        Vector3<f32> rot(){ return rotation; }
        Position* rot(Vector3<f32> rotation){ this->rotation = rotation; return this; }
        Position* lookAt(Vector3<f32> target)
        {
            //TODO
            return this;
        }
    };
}}
#endif
