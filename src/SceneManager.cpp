#include "SceneManager.h"
#include "EnterFrameEvent.h"
#include <time.h>
#include "CameraComponent.h"
#include "PositionComponent.h"

using namespace luck;
using namespace scene;
using namespace event;
using namespace core;

SceneManager* SceneManager::_instance = nullptr;

void SceneManager::handleEvent(string name, Event* e)
{
    dispatchEvent(name,e);
}

Entity* SceneManager::createEntity(string components)
{
    core::Entity* e = new core::Entity(_currentId,components);
    _entities[_currentId] = e;
    _currentId++;
    return e;
}

void SceneManager::destroyEntity(core::Entity* e)
{
    destroyEntity(e->id);
}

void SceneManager::destroyEntity(u64 id)
{
    map<u64,core::Entity*>::iterator it = _entities.find(id);
    core::Entity* e;
    if(!(it == _entities.end()))
    {
        e = it->second;
        _entities.erase(it);
    }
    delete e;
}

Entity* SceneManager::find(u64 id)
{
    return _entities[id];
}

vector<Entity*> SceneManager::find(string components)
{
    vector<core::Entity*> ret;
    vector<string> x = core::split(components, ' ');
    for(auto it = _entities.begin(); it != _entities.end(); it++)
    {
        for(auto sit = x.begin(); sit != x.end(); sit++)
        {
            if(it->second->has(*sit)) ret.push_back(it->second);
        }
    }
    return ret;
}

void SceneManager::addCamera(string name, core::Entity* e)
{
    if(!e->has("Camera")) return;
    _cameras[name] = e;
    if(_activeCamera == "") _activeCamera = name;
}

void SceneManager::setActiveCamera(string name)
{
    if(!_cameras[name]) return;
    _activeCamera = name;
}

void SceneManager::updateScene()
{
    if(clock() - _lastTime >= CLOCKS_PER_SEC/(f64)_FPS )
    {
        f64 deltaTime = (clock() - _lastTime) / CLOCKS_PER_SEC;
        _currentFrame += 1;// + 1 * deltaTime;
        for(auto it = _entities.begin(); it != _entities.end(); it++)
        {
            EnterFrameEvent* evt = new EnterFrameEvent();
            evt->currentFrame = _currentFrame;
            evt->deltaTime = std::min(deltaTime,(f64)10);
            it->second->dispatchEvent("EnterFrame", (Event*)evt);
        }
        _lastTime = clock();
    }
}

void SceneManager::drawScene(core::Color4<u8> clearColor)
{
    LuckWindow* lkw = LuckWindow::getInstance();
    glViewport( 0, 0, lkw->width, lkw->height );

    glClearColor((f32)clearColor.r/255, (f32)clearColor.g/255, (f32)clearColor.b/255, (f32)clearColor.a/255);
    glClear( GL_COLOR_BUFFER_BIT );

    Entity* ent = _cameras[_activeCamera];

    if(!ent) return;

    Position* pos = ent->get<Position>("Position");
    Camera* cam = ent->get<Camera>("Camera");

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    ///@todo understand this code.
    GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;
    fH = tan( cam->_fov / 360 * pi ) * cam->_near;
    fW = fH * cam->_aspect;
    glFrustum( -fW, fW, -fH, fH, cam->_near, cam->_far );
    //gluPerspective( cam->_fov, cam->_aspect, cam->_near, cam->_far );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    ///@todo replace gluLookAt with my own code (and also understand it)
    gluLookAt(pos->_position.x, pos->_position.y, pos->_position.z,
              pos->_position.x + pos->_rotation.x, pos->_position.y + pos->_rotation.y, pos->_position.z + pos->_rotation.z,
              0.f, 1.f, 0.f );
}
