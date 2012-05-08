#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H
#include "luck.h"
#include "IComponent.h"
#include "Vector3.h"
#include "PositionComponent.h"
namespace luck { namespace core
{
    Component(Camera)
    {
        f32 _fov;
        f32 _aspect;
        f32 _near;
        f32 _far;
        void init()
        {
            owner->requires("Position");
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
        Camera()
        {
            LuckWindow* lkw = LuckWindow::getInstance();
            _fov = 65.f;
            _aspect = lkw->width/lkw->height;
            _near = 1.f;
            _far = 100.f;
        }
    };
}}
#endif
