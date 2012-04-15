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

            smgr->addEventListener("KeyDown", owner);
            smgr->addEventListener("KeyUp", owner);

            owner->addEventListener("KeyDown", this);
            owner->addEventListener("KeyUp", this);
        }
        bool isDown(s32 key)
        {
            return _pressedKeys[key];
        }
        void handleEvent(string name, event::Event* e)
        {
            if(name == "KeyDown")
                handleKeyDown((event::KeyEvent*)e);
            else
                handleKeyUp((event::KeyEvent*)e);
        }
        private:
            void handleKeyDown(event::KeyEvent* e)
            {
                _pressedKeys[e->keyCode] = true;
            }
            void handleKeyUp(event::KeyEvent* e)
            {
                _pressedKeys[e->keyCode] = false;
            }
            map<s32, bool> _pressedKeys;
    };
}}
#endif
