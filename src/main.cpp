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
        EnterFrameEvent const& ef = (EnterFrameEvent const&)e;;
        Keyboard* k = owner->get<Keyboard>("Keyboard");
        Position* p = owner->get<Position>("Position");
        if(k->isDown(GLFW_KEY_DOWN))
        {
            //p->_rotation.z += 200.f * ef.deltaTime;
        }
        if(k->isDown(GLFW_KEY_UP))
        {
            p->_rotation.x += 50.5f * ef.deltaTime;
        }
        if(k->isDown(GLFW_KEY_RIGHT))
        {
            //p->_rotation.x += 200.f * ef.deltaTime;
        }
    }
    ~Test(){ }
};

Component(FPSControl)
{
    void init()
    {
        _mouseDown = false;
        owner->requires("Keyboard Mouse");
        owner->addEventListener("EnterFrame",eventCallback(this,&FPSControl::handleEnterFrame));
        owner->addEventListener("KeyDown",eventCallback(this,&FPSControl::handleKeyDown));
        owner->addEventListener("MouseMove",eventCallback(this,&FPSControl::handleMouseMove));
        owner->addEventListener("MouseDown",eventCallback(this,&FPSControl::handleMouseDown));
        owner->addEventListener("MouseUp",eventCallback(this,&FPSControl::handleMouseUp));
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
            pos->_position.x -= sinf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime * cosf(pos->_rotation.x*3.14/180);
            pos->_position.z += cosf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime * cosf(pos->_rotation.x*3.14/180);
            pos->_position.y += sinf(pos->_rotation.x*3.14/180) * 5.f * ef.deltaTime;
        }
        if(k->isDown('W'))
        {
            pos->_position.x += sinf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime * cosf(pos->_rotation.x*3.14/180);
            pos->_position.z -= cosf(pos->_rotation.y*3.14/180) * 5.f * ef.deltaTime * cosf(pos->_rotation.x*3.14/180);
            pos->_position.y -= sinf(pos->_rotation.x*3.14/180) * 5.f * ef.deltaTime;
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
        if(!_mouseDown) return;
        MouseEvent const& em = (MouseEvent const&)e;
        Position* pos = owner->get<Position>("Position");

        s16 xDiff = em.position.x - 1024/2;
        s16 yDiff = em.position.y - 768/2;

        pos->_rotation.y += xDiff/4;
        pos->_rotation.x = std::max(std::min(pos->_rotation.x + yDiff/4,90.f),-90.f);

        /// @todo think a better way to wrap the glfwSetMousePos (SceneManager or MouseComponent?)
        glfwSetMousePos(1024/2,768/2);
    }
    void handleMouseDown(Event const& e)
    {
        MouseEvent const& em = (MouseEvent const&)e;
        _mouseDown = true;
        glfwSetMousePos(1024/2,768/2);
    }
    void handleMouseUp(Event const& e)
    {
        MouseEvent const& em = (MouseEvent const&)e;
        _mouseDown = false;
    }
    private:
        bool _mouseDown;
};

Component(GoUp)
{
    void init()
    {
        smgr = SceneManager::getInstance();
        owner->requires("Position");
        owner->addEventListener("EnterFrame",eventCallback(this,&GoUp::handleEnterFrame));
        createdAt = 0;
    }
    void handleEnterFrame(Event const& e)
    {
        EnterFrameEvent const& ef = (EnterFrameEvent const&)e;
        if(createdAt == 0) createdAt = ef.currentFrame;
        Position* pos = owner->get<Position>("Position");
        pos->_position.y += 0.5f * ef.deltaTime;
        pos->_rotation.y += 50.f * ef.deltaTime;
        if(ef.currentFrame - createdAt > 150)
        {
            smgr->destroyEntity(owner);
        }
    }
    f32 createdAt;
    private:
        SceneManager* smgr;
};

Component(ParticleSpring)
{
    void init()
    {
        owner->requires("Keyboard");
        smgr = SceneManager::getInstance();
        assets = AssetManager::getInstance();
        owner->addEventListener("EnterFrame",eventCallback(this,&ParticleSpring::handleEnterFrame));
    }
    void handleEnterFrame(Event const& e)
    {
        Keyboard* k = owner->get<Keyboard>("Keyboard");
        //if(!k->isDown('X')) return;
        EnterFrameEvent const& ef = (EnterFrameEvent const&)e;
        //if(!(int(ef.currentFrame) % 10 == 0)) return;
        smgr->createEntity("PARTICLE")
            ->add("Test Model GoUp")
            ->get<Position>("Position")->position(Vector3<f32>(0.f,0.f,0.f))->parent(owner)->owner
            ->get<Model>("Model")->model(assets->getLoadedMesh("assets/cube_uv.obj"))->texture(assets->getLoadedImage("assets/tex1.png"));
    }
    private:
        SceneManager* smgr;
        AssetManager* assets;
};

int main(int argc, char* argv[])
{
    LuckWindow* lkw = createLuckWindow(1024,768);
    if(!lkw)
    {
        std::cout<<"Could not create a Window;";
        return 1;
    }

    if(!GLEW_ARB_vertex_buffer_object)
    {
        std::cout<<"Vertex Buffer support not found."<<"\n";
        return 2;
    }

    SceneManager* smgr = SceneManager::getInstance();
    AssetManager* assets = AssetManager::getInstance();

    assets->addToLoadQueue("assets/cube_uv.obj",assetType::ASSET_MESH);
    assets->addToLoadQueue("assets/monkey_high1.obj",assetType::ASSET_MESH);
    assets->addToLoadQueue("assets/lars_armature.obj",assetType::ASSET_MESH);
    assets->addToLoadQueue("assets/tex1.png",assetType::ASSET_IMAGE);
    assets->addToLoadQueue("assets/tex_body2.png",assetType::ASSET_IMAGE);

    assets->load([](LoadProgressEvent const& e) -> void {
        std::cout<<(e.percent - (e.percent - int(e.percent)))<<"% loaded: "<<e.file<<"\n";
    });

    smgr->createEntity("PLAYER")
        ->add("Test Model ParticleSpring")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-5.f,1.2f))->owner
        ->get<Model>("Model")->model(assets->getLoadedMesh("assets/cube_uv.obj"))->texture(assets->getLoadedImage("assets/tex1.png"));

    smgr->createEntity("HEX Test Model")
        ->get<Position>("Position")->position(Vector3<f32>(1.f,1.5f,1.0f))->parent(nullptr)->owner
        ->get<Model>("Model")->model(assets->getLoadedMesh("assets/monkey_high1.obj"))->texture(assets->getLoadedImage("assets/tex1.png"));

    Mesh* square = new Mesh();
    square->vertexList.push_back(Vertex(-1,-1,0 , 1,0,0,1 , 0.25,0.25));
    square->vertexList.push_back(Vertex( 1, 1,0 , 0,1,0,1 , 0.75,0.75));
    square->vertexList.push_back(Vertex( 1,-1,0 , 0,0,1,1 , 0.75,0.25));
    square->vertexList.push_back(Vertex(-1, 1,0 , 1,0,1,1 , 0.25,0.75));
    square->faceList.push_back(0);
    square->faceList.push_back(2);
    square->faceList.push_back(1);
    square->faceList.push_back(0);
    square->faceList.push_back(1);
    square->faceList.push_back(3);

    smgr->createEntity("Position Model")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-2.f,0.f))->owner
        ->get<Model>("Model")->model(square)->texture(assets->getLoadedImage("assets/tex1.png"));

    smgr->createEntity("Camera FPSControl")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,0.f,3.f))->lookAt(Vector3<f32>(0.f,0.f,0.f))->owner
        ->get<Camera>("Camera")->fov(65.f)->near(0.5f)->far(10.f);
    smgr->addCamera("cam", smgr->find("Camera")[0]);

    f64 time = clock()/CLOCKS_PER_SEC;
    f64 frameNum = 0;

    f64 avgft = 0;
    f64 lastTime = glfwGetTime();
    s32 nbFrames = 0;

    try
    {
        while(lkw->isRunning())
        {
            smgr->updateScene();
            smgr->drawScene(Color4(100.f/255,101.f/255,140.f/255,1));

            f64 currentTime = glfwGetTime();
            nbFrames++;
            if ( currentTime - lastTime >= 1.0 )
            {
                avgft = 1000.0/f64(nbFrames);
                nbFrames = 0;
                lastTime += 1.0;
            }

            std::stringstream _fps;
            f64 fps = frameNum++/time;
            _fps<<"LuckEngine - [FPS: "<<(fps - (fps-s32(fps)))<<"] [AVGFT: "<<avgft<<"]";
            lkw->setWindowCaption(_fps.str());

            time = clock()/CLOCKS_PER_SEC;
        }
    }
    catch (std::exception e)
    {
        glfwTerminate();
        return -1;
    }

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
