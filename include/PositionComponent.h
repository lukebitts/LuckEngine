#ifndef POSITIONCOMPONENT
#define POSITIONCOMPONENT
#include "luck.h"
#include "IComponent.h"
#include "Entity.h"
#include "Vector3.h"
#include <math.h>
#include <iostream>

namespace luck { namespace core
{
    Component(Position)
    {
        Vector3<f32> _position;
        Vector3<f32> _rotation;
        Entity* _parent;
        vector<Entity*> _children;
        void init()
        {
            _position = Vector3<f32>();
            _rotation = Vector3<f32>();
            _parent = nullptr;
        }
        Position* parent(Entity* p)
        {
            /// @todo handle the destruction of the parent
            ///       -> make it remove itself (or add itself) from the root nodes in the drawables list
            /// @todo make it possible for non-models to becomed children and parents (like a camera)
            if(_parent)
            {
                vector<Entity*>& children = _parent->get<Position>("Position")->_children;
                for(u16 i = 0; i < children.size(); i++)
                {
                    if(children[i] == owner)
                    {
                        children.erase(children.begin()+i);
                    }
                }
            }
            if(p && p->has("Position"))
            {
                _parent = p;
                p->get<Position>("Position")->_children.push_back(owner);
            }
            return this;
        }
        ~Position()
        {
            parent(nullptr);
        }
        Position* position(Vector3<f32> position){ _position = position; return this; }
        Position* rotation(Vector3<f32> rotation){ _rotation = rotation; return this; }
        Position* lookAt(Vector3<f32> target)
        {
            /// http://img.anongallery.org/img/4/1/i-have-no-idea-what-im-doing-dog.jpg
            /// angle(v1, v2) = acos( dot(v1, v2) / (norm(v1) * norm(v2)) )
            /// http://stackoverflow.com/questions/3441782/how-to-calculate-the-angle-of-a-vector-from-the-vertical
            /// @todo : I should use Vector2 here for each _rotation.
            /// angle(v1, v2) = acos( (v1x * v2x + v1y * v2y) / (sqrt(v1x^2+v1y^2) * sqrt(v2x^2+v2y^2)) )
            //_rotation.y = acos( (_position.x * target.x + _position.z * target.z) / ( sqrtf(_position.x*_position.x+_position.z*_position.z) * sqrtf(target.x*target.x+target.z*target.z) ) );
            //_rotation.x = acos( (_position.z * target.z + _position.y * target.y) / ( sqrtf(_position.z*_position.z+_position.y*_position.y) * sqrtf(target.z*target.z+target.y*target.y) ) );

            //std::cout<<" :: "<<_rotation.y*180/3.14<<"\n";
            //std::cout<<atan2(6-5,6-9)*180/3.14<<"\n";

            //_rotation.y *= 180/3.14;
            //_rotation.x *= 180/3.14;

            return this;
        }
    };
}}
#endif
