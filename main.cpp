#include "LuckEngine.h"

#include <time.h>
#include <math.h>
#include <iostream>
#include <cstdlib>

using namespace luck;
using namespace core;
using namespace event;
using namespace scene;

Component(Test)
{
    Vertex vertexList[4];
    s32 faceList[6];
    void init()
    {
        owner->requires("Keyboard VertexBuffer");
        owner->addEventListener("EnterFrame",eventCallback(this,&Test::handleEnterFrame));
        owner->addEventListener("Draw",eventCallback(this,&Test::handleDraw));

        vertexList[0] = Vertex(Vector3<f32>(-3.5f, 0.f, -3.5f),Color4(255,0,0,255));
        vertexList[1] = Vertex(Vector3<f32>( 3.5f, 0.f, -3.5f),Color4(0,255,0,255));
        vertexList[2] = Vertex(Vector3<f32>( 3.5f, 0.f,  3.5f),Color4(0,0,255,255));
        vertexList[3] = Vertex(Vector3<f32>(-3.5f, 0.f,  3.5f),Color4(255,255,0,255));

        faceList[0] = 0;
        faceList[1] = 1;
        faceList[2] = 2;
        faceList[3] = 0;
        faceList[4] = 2;
        faceList[5] = 3;
    }
    void handleEnterFrame(Event const& e)
    {
        EnterFrameEvent const& ef = (EnterFrameEvent const&)e;
        Keyboard* k = owner->get<Keyboard>("Keyboard");
        Position* p = owner->get<Position>("Position");
        if(k->isDown(GLFW_KEY_DOWN))
        {
            p->_position.x += 3.f * ef.deltaTime;
            p->_rotation.y += 50.f * ef.deltaTime;
        }
    }
    void handleDraw(Event const& e)
    {
        VertexBuffer* vb = owner->get<VertexBuffer>("VertexBuffer");
        vb->setVertexBuffer(&vertexList[0],&faceList[0],2,GL_TRIANGLES);
    }
    ~Test(){ }
};

Component(FixedBounds)
{
    Vertex vertexList[4][2];
    s32 faceList[4][2];
    void init()
    {
        owner->requires("VertexBuffer");
        owner->addEventListener("Draw",eventCallback(this,&FixedBounds::handleDraw));

        vertexList[0][0] = Vertex(Vector3<f32>(-10.f,0.f,-10.f),Color4(255,255,255,255));
        vertexList[0][1] = Vertex(Vector3<f32>( 10.f,0.f,-10.f),Color4(255,255,255,255));

        vertexList[1][0] = Vertex(Vector3<f32>(-10.f,0.f,10.f),Color4(255,255,255,255));
        vertexList[1][1] = Vertex(Vector3<f32>( 10.f,0.f,10.f),Color4(255,255,255,255));

        faceList[0][0] = 0;
        faceList[0][1] = 1;

        faceList[1][0] = 0;
        faceList[1][1] = 1;
    }
    void handleDraw(Event const& e)
    {
        VertexBuffer* vb = owner->get<VertexBuffer>("VertexBuffer");
        vb->setVertexBuffer(&vertexList[0][0],&faceList[0][0],1,GL_LINES);
        vb->setVertexBuffer(&vertexList[1][0],&faceList[1][0],1,GL_LINES);
        //vb->setVertexBuffer(&vertexList[2][0],&faceList[2][0],1,GL_LINES);
        //vb->setVertexBuffer(&vertexList[3][0],&faceList[3][0],1,GL_LINES);
    }
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

int main(int argc, char* argv[])
{
    LuckWindow* lkw = createLuckWindow(1024,768);
    if(!lkw) return -1;
    lkw->setWindowCaption("LuckEngine");

    SceneManager* smgr = SceneManager::getInstance();

    smgr->createEntity("FixedBounds")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-5.f,0.f));

    Entity* player = smgr->createEntity("PLAYER")
        ->add("Test Position")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-5.f,0.f))->owner;

    Entity* camera = smgr->createEntity("Camera FPSControl")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,0.f,0.f))->lookAt(Vector3<f32>(0.f,0.f,-1.f))
        ->owner
        ->get<Camera>("Camera")->fov(85.f)->near(0.1f)->far(500.f)
        ->owner;

    smgr->addCamera("cam", camera);

    while(lkw->isRunning())
    {
        smgr->updateScene();
        smgr->drawScene(Color4(100,101,140,255));
    }

    glfwTerminate();

    return 0;
}

/*glPushMatrix();
f32 line = 10.f;
glBegin( GL_LINES );
    for(u16 i = 0; i <= line; i++)
    {
        glColor3f(1.f,0.f,0.f);
        glVertex3f(-line, i-line/2, -line);
        glVertex3f( line, i-line/2, -line);
        glColor3f(1.f,1.f,1.f);
        glVertex3f(i*2-line,-line/2, -line);
        glVertex3f(i*2-line, line/2, -line);

        glColor3f(0.f,1.f,0.f);
        glVertex3f(line,i-line/2,-line);
        glVertex3f(line,i-line/2, line);
        glColor3f(1.f,1.f,1.f);
        glVertex3f(line,-line/2,i*2-line);
        glVertex3f(line, line/2,i*2-line);

        glColor3f(0.f,0.f,1.f);
        glVertex3f(-line, i-line/2,line);
        glVertex3f( line, i-line/2,line);
        glColor3f(1.f,1.f,1.f);
        glVertex3d(i*2-line,-line/2,line);
        glVertex3d(i*2-line, line/2,line);

        glColor3f(1.f,1.f,0.f);
        glVertex3d(-line,i-line/2,-line);
        glVertex3d(-line,i-line/2, line);
        glColor3f(1.f,1.f,1.f);
        glVertex3f(-line,-line/2,i*2-line);
        glVertex3f(-line, line/2,i*2-line);
    }
glEnd();
glPopMatrix();*/

/*

Componentes Básicos

Position (x, y e z, podem ser usados para 2D ou 3D, em 2D o z é o z-index)
Image (2D apenas) *
VertexList
- Ele é capaz de se desenhar também
Texture *
BoundingBox (3D e 2D(será que vale a pena fazer 2D também, o foco da engine não é 2D [por enquanto anyway] ) )*
Keyboard
Mouse (em 3D vai ser fodinha fazer, mas é necessário)
- Para funcionar em 3D precisa de uma bounding box além da VertexList
Luzes
Billboard

*/
