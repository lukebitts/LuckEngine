#ifndef POSITIONCOMPONENT
#define POSITIONCOMPONENT
#include "luck.h"
#include "IComponent.h"
#include "Entity.h"
#include "Vector3.h"
#include "Quaternion.h"
#include <math.h>
#include <iostream>
namespace luck { namespace core
{
    ///@todo Add a parent to the component, so it will be in relative position this parent
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
            /// http://img.anongallery.org/img/4/1/i-have-no-idea-what-im-doing-dog.jpg
            _rotation.x = atan(tanf((target.z - _position.z)/(target.y - _position.y))) * 180/3.14;
            _rotation.y = atan(tanf((target.x - _position.x)/(target.z - _position.z))) * 180/3.14;
            _rotation.z = 0;// atan(tanf((target.y - _position.y)/(target.x - _position.x))) * 180/3.14;

            Vector3<f32> res = Vector3<f32>::cross(Vector3<f32>::normalize(_rotation),Vector3<f32>::normalize(target));

            std::cout<<res.x<<" "<<res.y<<" "<<res.z<<"\n";
            return this;
        }
    };
}}
#endif
