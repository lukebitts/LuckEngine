#ifndef MOUSECOMPONENT_H
#define MOUSECOMPONENT_H
#include "IComponent.h"
#include "SceneManager.h"
#include "Vector2.h"
#include "MouseEvent.h"
namespace luck { namespace core
{
    Component(Mouse)
    {
        void init()
        {
            scene::SceneManager* smgr = scene::SceneManager::getInstance();

            _mouseMoveListener = event::eventCallback(this,&Mouse::handleMouseMove);
            _mouseDownListener = event::eventCallback(this,&Mouse::handleMouseDown);
            _mouseUpListener = event::eventCallback(this,&Mouse::handleMouseUp);
            _mouseWheelListener = event::eventCallback(this,&Mouse::handleMouseWheel);

            smgr->addEventListener("MouseMove", _mouseMoveListener);
            smgr->addEventListener("MouseDown", _mouseDownListener);
            smgr->addEventListener("MouseUp", _mouseUpListener);
            smgr->addEventListener("MouseWheel", _mouseWheelListener);
        }
        ~Mouse()
        {
            scene::SceneManager* smgr = scene::SceneManager::getInstance();

            smgr->removeEventListener(_mouseMoveListener);
            smgr->removeEventListener(_mouseDownListener);
            smgr->removeEventListener(_mouseUpListener);
            smgr->removeEventListener(_mouseWheelListener);

            delete _mouseMoveListener;
            delete _mouseDownListener;
            delete _mouseDownListener;
            delete _mouseDownListener;
        }
        Vector2<u16> _mousePosition;
        private:
            event::IFunctor* _mouseMoveListener;
            event::IFunctor* _mouseDownListener;
            event::IFunctor* _mouseUpListener;
            event::IFunctor* _mouseWheelListener;
            void handleMouseMove(event::Event const& e)
            {
                event::MouseEvent const& em = (event::MouseEvent const&)e;
                _mousePosition = em.position;
                owner->dispatchEvent("MouseMove", e);
            }
            void handleMouseDown(event::Event const& e)
            {
                owner->dispatchEvent("MouseDown", e);
            }
            void handleMouseUp(event::Event const& e)
            {
                owner->dispatchEvent("MouseUp", e);
            }
            void handleMouseWheel(event::Event const& e)
            {
                owner->dispatchEvent("MouseWheel", e);
            }
    };
}}
#endif

