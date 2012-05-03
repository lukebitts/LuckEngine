#include "LuckEngine.h"

#include <time.h>
#include <math.h>
#include <iostream>
#include <cstdlib>

using namespace luck;
using namespace core;
using namespace event;
using namespace scene;
using namespace asset;

Component(Test)
{
    void init()
    {
        owner->requires("Keyboard");
        owner->addEventListener("EnterFrame",eventCallback(this,&Test::handleEnterFrame));
    }
    void handleEnterFrame(Event const& e)
    {
        EnterFrameEvent const& ef = (EnterFrameEvent const&)e;
        Keyboard* k = owner->get<Keyboard>("Keyboard");
        Position* p = owner->get<Position>("Position");
        if(k->isDown(GLFW_KEY_DOWN))
        {
            p->_rotation.z += 200.f * ef.deltaTime;
        }
    }
    ~Test(){ }
};

Component(FPSControl)
{
    void init()
    {
        owner->requires("Keyboard Mouse");
        owner->addEventListener("EnterFrame",eventCallback(this,&FPSControl::handleEnterFrame));
        owner->addEventListener("KeyDown",eventCallback(this,&FPSControl::handleKeyDown));
        owner->addEventListener("MouseMove",eventCallback(this,&FPSControl::handleMouseMove));
    }
    void handleEnterFrame(Event const& e)
    {
        EnterFrameEvent const& ef = (EnterFrameEvent const&)e;
        Position* pos = owner->get<Position>("Position");
        Keyboard* k = owner->get<Keyboard>("Keyboard");

        if(k->isDown('D'))
        {
            pos->_position.x += cosf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime;
            pos->_position.z += sinf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime;
        }
        if(k->isDown('A'))
        {
            pos->_position.x -= cosf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime;
            pos->_position.z -= sinf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime;
        }
        if(k->isDown('S'))
        {
            pos->_position.x -= sinf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime;
            pos->_position.z += cosf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime;
        }
        if(k->isDown('W'))
        {
            pos->_position.x += sinf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime;
            pos->_position.z -= cosf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime;
        }
    }
    void handleKeyDown(Event const& e)
    {
        KeyEvent const& ek = (KeyEvent const&)e;
        if(ek.keyCode == 'R')
        {
            Position* pos = owner->get<Position>("Position");
            pos->_position = Vector3<f32>();
            pos->_rotation = Vector3<f32>();
        }
    }
    void handleMouseMove(Event const& e)
    {
        MouseEvent const& em = (MouseEvent const&)e;
        Position* pos = owner->get<Position>("Position");

        s16 xDiff = em.position.x - 1024/2;
        s16 yDiff = em.position.y - 768/2;

        pos->_rotation.y += xDiff/4;
        pos->_rotation.x = std::max(std::min(pos->_rotation.x + yDiff/4,60.f),-60.f);

        /// @todo think a better way to wrap the glfwSetMousePos (SceneManager or MouseComponent?)
        glfwSetMousePos(1024/2,768/2);
    }
};

Component(Model)
{
    /// @todo Make a file loader to manage stuff in the memory (like the .obj model)
    Mesh* _model;
    void init()
    {
        owner->addEventListener("Draw",eventCallback(this,&Model::handleDraw));
        owner->requires("VertexBuffer");
    }
    Model* model(Mesh* model)
    {
        _model = model;
    }
    void handleDraw(Event const& e)
    {
        VertexBuffer* vb = owner->get<VertexBuffer>("VertexBuffer");
        vb->setVertexBuffer(&_model->vertexList[0],&_model->faceList[0],_model->faceList.size()/3,GL_TRIANGLES);
    }
};

int main(int argc, char* argv[])
{
    LuckWindow* lkw = createLuckWindow(1024,768);
    if(!lkw) return -1;
    lkw->setWindowCaption("LuckEngine");

    SceneManager* smgr = SceneManager::getInstance();
    AssetManager* assets = AssetManager::getInstance();

    assets->addToLoadQueue("assets/cube.obj",assetType::ASSET_MESH);
    assets->addToLoadQueue("assets/cube_tri.obj",assetType::ASSET_MESH);
    assets->addToLoadQueue("assets/monkey.obj",assetType::ASSET_MESH);
    assets->addToLoadQueue("assets/monkey_tri.obj",assetType::ASSET_MESH);
    assets->addToLoadQueue("assets/monkey_high-1.obj",assetType::ASSET_MESH);
    assets->addToLoadQueue("assets/monkey_high0.obj",assetType::ASSET_MESH);

    assets->load([](Event const& e) -> void {
        std::cout<<e.text<<"\n";
    });

    smgr->createEntity("PLAYER")
        ->add("Test Model")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-5.f,0.f))->owner
        ->get<Model>("Model")->model(assets->getLoadedMesh("assets/monkey_high-1.obj"));

    smgr->createEntity("Model Test")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-7.f,0.f))->owner
        ->get<Model>("Model")->model(assets->getLoadedMesh("assets/cube.obj"));

    smgr->createEntity("Camera FPSControl")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,0.f,0.f))->lookAt(Vector3<f32>(0.f,0.f,-1.f))
        ->owner
        ->get<Camera>("Camera")->fov(85.f)->near(0.1f)->far(500.f)
        ->owner;

    smgr->addCamera("cam", smgr->find("Camera")[0]);

    /// @todo create a FPS counter
    while(lkw->isRunning())
    {
        smgr->updateScene();
        smgr->drawScene(Color4(100,101,140,255));
    }

    glfwTerminate();

    return 0;
}

/*
Componentes Básicos

Image (2D apenas)
Texture
BoundingBox (3D e 2D(será que vale a pena fazer 2D também, o foco da engine não é 2D [por enquanto anyway] ) )*
Luzes
Billboard
*/
