#include <GL/glew.h>
#include "SceneManager.h"
#include "EnterFrameEvent.h"
#include <time.h>
#include <cstdlib>
#include "CameraComponent.h"
#include "PositionComponent.h"
#include "VertexBufferComponent.h"
#include "lmath.h"

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

static const s32 PositionSize = 6 * 2 * sizeof(f32);
static const f32 PositionData[] =
{
	-1.0f,-1.0f,
	 1.0f,-1.0f,
	 1.0f, 1.0f,
	 1.0f, 1.0f,
	-1.0f, 1.0f,
	-1.0f,-1.0f,
};

static const s32 ColorSize = 6 * 3 * sizeof(s16);
static const s16 ColorData[] =
{
	255,   0,   0,
	255, 255,   0,
	  0, 255,   0,
	  0, 255,   0,
	  0,   0, 255,
	255,   0,   0
};

static const s32 BufferSize = 2;
static s32 BufferName[BufferSize];

static const s32 VertexCount = 6;

enum
{
    POSITION_OBJECT = 0,
    COLOR_OBJECT = 1
};

void SceneManager::drawScene(core::Color4 clearColor)
{
    LuckWindow* lkw = LuckWindow::getInstance();
    lkw->updateWindowSize();
    glViewport( 0, 0, lkw->width, lkw->height );

    glClearColor((f32)clearColor.r/255, (f32)clearColor.g/255, (f32)clearColor.b/255, (f32)clearColor.a/255);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Entity* ent = _cameras[_activeCamera];
    if(!ent) return;

    Position* pos = ent->get<Position>("Position");
    Camera* cam = ent->get<Camera>("Camera");

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    f64 fW, fH;
    fH = tan( cam->_fov / 360 * math::PI64 ) * cam->_near;
    fW = fH * cam->_aspect;
    glFrustum( -fW, fW, -fH, fH, cam->_near, cam->_far );

    glMatrixMode( GL_MODELVIEW );

    /*glBindBuffer(GL_ARRAY_BUFFER, BufferName[COLOR_OBJECT]);
    glBufferData(GL_ARRAY_BUFFER, ColorSize, ColorData, GL_STREAM_DRAW);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, BufferName[POSITION_OBJECT]);
    glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STREAM_DRAW);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_TRIANGLES, 0, VertexCount);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);*/

    vector<Entity*> drawables = find("VertexBuffer");
    for(u64 i = 0; i < drawables.size(); i++)
    {
        drawables[i]->dispatchEvent("Draw",Event());
        vector<VertexBuffer::BufferInfo> bufferList = drawables[i]->get<VertexBuffer>("VertexBuffer")->bufferList;
        Position* drawablePos = drawables[i]->get<Position>("Position");
        for(u64 j = 0; j < bufferList.size(); j++)
        {
            glPushMatrix();
            glTranslatef(drawablePos->_position.x,drawablePos->_position.y,drawablePos->_position.z);
            glRotatef(drawablePos->_rotation.x,1,0,0);
            glRotatef(drawablePos->_rotation.y,0,1,0);
            glRotatef(drawablePos->_rotation.z,0,0,1);

            /// @todo use vertex buffer object instead of these calls
            /// @todo split the drawing code to a internal class (DrawManager or something)

            glBegin(bufferList[j].type);
            s32 amm;
            if(bufferList[j].type == GL_TRIANGLES)   amm = 3;
            else if (bufferList[j].type == GL_QUADS) amm = 4;
            else if (bufferList[j].type == GL_LINES) amm = 2;
            for(u64 v = 0; v < bufferList[j].faceAmm*amm; v+=amm)
            {
                for(u64 vi = v; vi < v+amm; vi++)
                {
                    s32 v1 = bufferList[j].faceList[vi];
                    glColor4f((f32)bufferList[j].vertexList[v1].color.r/255,
                              (f32)bufferList[j].vertexList[v1].color.g/255,
                              (f32)bufferList[j].vertexList[v1].color.b/255,
                              (f32)bufferList[j].vertexList[v1].color.a/255);
                    glVertex3f(bufferList[j].vertexList[v1].position.x,
                               bufferList[j].vertexList[v1].position.y,
                               bufferList[j].vertexList[v1].position.z);
                }
            }
            glEnd();
            glPopMatrix();
        }
        drawables[i]->get<VertexBuffer>("VertexBuffer")->bufferList.clear();
    }

    glLoadIdentity();
    glRotatef(pos->_rotation.x,1.f,0.f,0.f);
    glRotatef(pos->_rotation.y,0.f,1.f,0.f);
    glRotatef(pos->_rotation.z,0.f,0.f,1.f);
    glTranslatef(-pos->_position.x, -pos->_position.y, -pos->_position.z);

    glfwSwapBuffers();
}
