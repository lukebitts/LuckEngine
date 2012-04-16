#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H
#include "luck.h"
#include "IComponent.h"
#include "Vector3.h"
#include "PositionComponent.h"
namespace luck { namespace core
{
    ///@todo Find the up vector and lookat from the position and the rotation
    Component(Camera)
    {
        Vector3<f32> _up;
        f32 _fov;
        f32 _aspect;
        f32 _near;
        f32 _far;
        void init()
        {
            owner->requires("Position");
        }
        void handleEvent(string name, event::Event* e)
        {

        }
        Vector3<f32> calculateLookAtFromRotation()
        {
            Vector3<f32> rot = owner->get<Position>("Position")->_rotation;
            return rot;
        }
        Camera* fov(f32 fov)
        {
            _fov = fov;
            return this;
        }
        Camera* aspect(f32 aspect)
        {
            _aspect = aspect;
            return this;
        }
        Camera* near(f32 near)
        {
            _near = near;
            return this;
        }
        Camera* far(f32 far)
        {
            _far = far;
            return this;
        }
        Camera* up(Vector3<f32> up)
        {
            _up = up;
            return this;
        }
        Camera()
        {
            LuckWindow* lkw = LuckWindow::getInstance();
            _up = Vector3<f32>();
            _fov = 65.f;
            _aspect = lkw->width/lkw->height;
            _near = 1.f;
            _far = 100.f;
        }
    };
}}
#endif
