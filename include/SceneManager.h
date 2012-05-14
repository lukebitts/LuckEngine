#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include <GL/glfw.h>
#include "luck.h"
#include "Entity.h"
#include "LuckWindow.h"
#include "Color4.h"
#include "ShaderComponent.h"
#include "Matrix4x4.h"
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
            u16 _defaultShader;
            map<u64, core::Entity*> _entities;
            map<string, core::Entity*> _cameras;
            std::string _activeCamera;
            void _drawModelsRecursively(core::Entity* e, const core::Matrix4x4<f32>& ProjectionMatrix, const core::Matrix4x4<f32>& ViewMatrix, core::Matrix4x4<f32> ParentModel = core::Matrix4x4<f32>(1.f));
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

                _defaultShader = LoadShaders("assets/shader.vert","assets/shader.frag");
            }
        public:
            static SceneManager* getInstance()
            {
                if(!_instance) _instance = new SceneManager();
                return _instance;
            }
            void handleKeyDown(event::Event const& e);
            void handleKeyUp(event::Event const& e);
            void handleMouseMove(event::Event const& e);
            void handleMouseDown(event::Event const& e);
            void handleMouseUp(event::Event const& e);
            void handleMouseWheel(event::Event const& e);
            void setFPS(u16 FPS){ _FPS = FPS; }
            core::Entity* createEntity(string components);
            void destroyEntity(core::Entity* e);
            void destroyEntity(u64 id);
            core::Entity* find(u64 id);
            vector<core::Entity*> find(string components);
            void addCamera(string name, core::Entity* cam);
            void removeCamera(string name);
            void setActiveCamera(string name);
            void updateScene();
            void drawScene(core::Color4 clearColor);
    };
}}
#endif
