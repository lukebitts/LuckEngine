#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include <GL/glfw.h>
#include "luck.h"
#include "Entity.h"
#include "LuckWindow.h"
#include "Color4.h"
namespace luck { namespace scene
{
    class SceneManager : public event::EventDispatcher, public event::IEventReceiver
    {
        private:
            //singleton
            u64 _currentId;
            f64 _currentFrame;
            u16 _FPS;
            f64 _lastTime;
            map<u64, core::Entity*> _entities;
            map<string, core::Entity*> _cameras;
            std::string _activeCamera;
            static SceneManager* _instance;
            SceneManager()
            {
                _currentId = 0;
                _currentFrame = 0;
                _activeCamera = "";
                _FPS = 60;

                LuckWindow* lkw = LuckWindow::getInstance();
                lkw->addEventListener("KeyDown",this);
                lkw->addEventListener("KeyUp",this);
                //lkw->addEventListener("MouseMove",this);
            }
        public:
            static SceneManager* getInstance()
            {
                if(!_instance) _instance = new SceneManager();
                return _instance;
            }
            void handleEvent(string name, event::Event* e);
            void setFPS(u16 FPS){ _FPS = FPS; }
            core::Entity* createEntity(string components);
            void destroyEntity(core::Entity* e);
            void destroyEntity(u64 id);
            core::Entity* find(u64 id);
            vector<core::Entity*> find(string components);
            void addCamera(string name, core::Entity* cam);
            void setActiveCamera(string name);
            void updateScene();
            void drawScene(core::Color4 clearColor);
    };
}}
#endif
