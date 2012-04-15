#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H
#include "luck.h"
#include "IComponent.h"
#include "Vector3.h"
namespace luck { namespace core
{
    Component(Camera)
    {
        Vector3<f32> up;
        f32 fov;
        f32 aspect;
        f32 near;
        f32 far;
        void init()
        {
            owner->requires("Position");
        }
        void handleEvent(string name, event::Event* e)
        {

        }
        Vector3<f32> calculateLookAtFromRotation()
        {

        }
        Camera* set(f32 fov, f32 aspect, f32 near, f32 far)
        {
            this->fov = fov;
            this->aspect = aspect;
            this->near = near;
            this->far = far;
            return this;
        }
        Camera* setUp(Vector3<f32> up)
        {
            this->up = up;
            return this;
        }
        Camera()
        {
            LuckWindow* lkw = LuckWindow::getInstance();
            up = Vector3<f32>();
            fov = 65.f;
            aspect = lkw->width/lkw->height;
            near = 1.f;
            far = 100.f;
        }
    };
}}
#endif
