#ifndef KEYBOARDCOMPONENT_H
#define KEYBOARDCOMPONENT_H
#include "IComponent.h"
#include "KeyEvent.h"
#include "SceneManager.h"
namespace luck { namespace core
{
    Component(Keyboard)
    {
        void init()
        {
            scene::SceneManager* smgr = scene::SceneManager::getInstance();

            _keyDownListener = event::eventCallback(this,&Keyboard::handleKeyDown);
            _keyUpListener = event::eventCallback(this,&Keyboard::handleKeyUp);

            smgr->addEventListener("KeyDown", _keyDownListener);
            smgr->addEventListener("KeyUp", _keyUpListener);
        }
        bool isDown(s32 key)
        {
            return _pressedKeys[key];
        }
        ~Keyboard()
        {
            scene::SceneManager* smgr = scene::SceneManager::getInstance();

            smgr->removeEventListener(_keyDownListener);
            smgr->removeEventListener(_keyUpListener);
            delete _keyDownListener;
            delete _keyUpListener;
        }
        private:
            event::IFunctor* _keyDownListener;
            event::IFunctor* _keyUpListener;
            void handleKeyDown(event::Event* e)
            {
                owner->dispatchEvent("KeyDown", e);
                event::KeyEvent* ek = (event::KeyEvent*)e;
                _pressedKeys[ek->keyCode] = true;
            }
            void handleKeyUp(event::Event* e)
            {
                owner->dispatchEvent("KeyUp", e);
                event::KeyEvent* ek = (event::KeyEvent*)e;
                _pressedKeys[ek->keyCode] = false;
            }
            map<s32, bool> _pressedKeys;
    };
}}
#endif
