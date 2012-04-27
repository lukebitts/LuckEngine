#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include <GL/glfw.h>
#include "luck.h"
#include "Entity.h"
#include "LuckWindow.h"
#include "Color4.h"
namespace luck { namespace scene
{
    class SceneManager : public event::EventDispatcher
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
                lkw->addEventListener("KeyDown",event::eventCallback(this,&SceneManager::handleKeyDown));
                lkw->addEventListener("KeyUp",event::eventCallback(this,&SceneManager::handleKeyUp));

                lkw->addEventListener("MouseMove",event::eventCallback(this,&SceneManager::handleMouseMove));
                lkw->addEventListener("MouseDown",event::eventCallback(this,&SceneManager::handleMouseDown));
                lkw->addEventListener("MouseUp",event::eventCallback(this,&SceneManager::handleMouseUp));
                lkw->addEventListener("MouseWheel",event::eventCallback(this,&SceneManager::handleMouseWheel));
            }
        public:
            static SceneManager* getInstance()
            {
                if(!_instance) _instance = new SceneManager();
                return _instance;
            }
            void handleKeyDown(event::Event* e);
            void handleKeyUp(event::Event* e);
            void handleMouseMove(event::Event* e);
            void handleMouseDown(event::Event* e);
            void handleMouseUp(event::Event* e);
            void handleMouseWheel(event::Event* e);
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
