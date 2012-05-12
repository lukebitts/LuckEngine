#include <GL/glew.h>
#include "SceneManager.h"
#include "EnterFrameEvent.h"
#include <time.h>
#include <cstdlib>
#include "CameraComponent.h"
#include "PositionComponent.h"
#include "ModelComponent.h"
#include "Mesh.h"
#include "lmath.h"
#include "Matrix4x4.h"

using namespace luck;
using namespace scene;
using namespace event;
using namespace core;

///@todo removeCamera method (or have the destroyEntity method also remove it from camera role)

SceneManager* SceneManager::_instance = nullptr;

void SceneManager::handleKeyDown(Event const& e)
{
    dispatchEvent("KeyDown",e);
}

void SceneManager::handleKeyUp(Event const& e)
{
    dispatchEvent("KeyUp",e);
}

void SceneManager::handleMouseMove(Event const& e)
{
    dispatchEvent("MouseMove",e);
}

void SceneManager::handleMouseDown(Event const& e)
{
    dispatchEvent("MouseDown",e);
}

void SceneManager::handleMouseUp(Event const& e)
{
    dispatchEvent("MouseUp",e);
}

void SceneManager::handleMouseWheel(Event const& e)
{
    dispatchEvent("MouseWheel",e);
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
            EnterFrameEvent evt = EnterFrameEvent();
            evt.currentFrame = _currentFrame;
            evt.deltaTime = std::min(deltaTime,(f64)10);
            it->second->dispatchEvent("EnterFrame", (Event const&)evt);
        }
        _lastTime = clock();
    }
}

void SceneManager::drawScene(core::Color4 clearColor)
{
    LuckWindow* lkw = LuckWindow::getInstance();
    lkw->updateWindowSize();
    glViewport( 0, 0, lkw->width, lkw->height );

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Entity* ent = _cameras[_activeCamera];
    if(!ent) return;

    Position* pos = ent->get<Position>("Position");
    Camera* cam = ent->get<Camera>("Camera");

    Matrix4x4<f32> ViewMatrix =
        Matrix4x4<f32>::rotationMatrix(-pos->_rotation.x,Vector3<f32>(1,0,0))*
        Matrix4x4<f32>::rotationMatrix(-pos->_rotation.y,Vector3<f32>(0,1,0))*
        Matrix4x4<f32>::rotationMatrix(-pos->_rotation.z,Vector3<f32>(0,0,1))*
        Matrix4x4<f32>::translationMatrix(-pos->_position.x,-pos->_position.y,-pos->_position.z);
    Matrix4x4<f32> ProjectionMatrix = Matrix4x4<f32>::perspective(cam->_fov, cam->_aspect, cam->_near, cam->_far);

    vector<Entity*> drawables = find("Model");
    for(u64 i = 0; i < drawables.size(); i++)
    {
        Position* drawablePos = drawables[i]->get<Position>("Position");
        asset::Mesh* m = drawables[i]->get<Model>("Model")->_model;
        if(!m || !drawablePos) continue;

        Matrix4x4<f32> ModelMatrix =
            Matrix4x4<f32>::translationMatrix(drawablePos->_position.x,drawablePos->_position.y,drawablePos->_position.z)*
            Matrix4x4<f32>::rotationMatrix(drawablePos->_rotation.x,Vector3<f32>(1,0,0))*
            Matrix4x4<f32>::rotationMatrix(drawablePos->_rotation.y,Vector3<f32>(0,1,0))*
            Matrix4x4<f32>::rotationMatrix(drawablePos->_rotation.z,Vector3<f32>(0,0,1));

        Matrix4x4<f32> MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        glUseProgram(_defaultShader);
        glUniformMatrix4fv(glGetUniformLocation(_defaultShader, "ModelViewProjection"),1,GL_FALSE,&MVP[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m->vboID);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            false,
            sizeof(Vertex),
            (void*)0
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m->cboID);
        glVertexAttribPointer(
            1,
            4,
            GL_FLOAT,
            false,
            sizeof(Vertex),
            (void*)(sizeof(f32)*3)
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->iboID);
        glDrawElements(
            GL_TRIANGLES,
            m->faceList.size(),
            GL_UNSIGNED_INT,
            (void*)0
        );

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

    }

    glfwSwapBuffers();
}
