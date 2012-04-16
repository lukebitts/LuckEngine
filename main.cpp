#include "LuckEngine.h"

#include <time.h>
#include <math.h>
#include <iostream>

using namespace luck;
using namespace core;
using namespace event;
using namespace scene;

Component(Test)
{
    void init()
    {
        owner->requires("Keyboard");
        owner->addEventListener("EnterFrame",this);
    }
    void handleEvent(string type, Event* e)
    {
        if(type == "EnterFrame")
            this->handleEnterFrame((EnterFrameEvent*)e);
    }
    void handleEnterFrame(EnterFrameEvent* e)
    {
        Keyboard* k = owner->get<Keyboard>("Keyboard");
        if(k->isDown('A'))
        {
            std::cout<<"a";
        }
    }
    ~Test(){ }
};

Component(MoveCamera)
{
    void init(){ owner->addEventListener("EnterFrame",this); }
    void handleEvent(string type, Event* e)
    {
        EnterFrameEvent* ef = (EnterFrameEvent*)e;
        Position* pos = owner->get<Position>("Position");

        pos->_position.x += 5.f * ef->deltaTime;
        //pos->_position.y = cosf(ef->currentFrame/10)*5;
    }
};

int main(int argc, char* argv[])
{

    LuckWindow* lkw = createLuckWindow(1024,768);
    if(!lkw) return -1;
    lkw->setWindowCaption("LuckEngine");

    SceneManager* smgr = SceneManager::getInstance();

    smgr->createEntity("PLAYER")
        ->add("Test Position")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,1.f,0.f));

    /*Entity* camera1 = smgr->createEntity("Camera")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-10.f,0.f))
        ->owner
        ->get<Camera>("Camera")->up(Vector3<f32>(0.f,0.f,1.f))
        ->owner;*/

    Entity* camera2 = smgr->createEntity("Camera MoveCamera")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,10.f,-1.f))->lookAt(Vector3<f32>(0.f,0.f,0.f))
        ->owner
        ->get<Camera>("Camera")->fov(65.f)->near(1.f)->far(500.f)
        ->owner;

    //smgr->addCamera("cam1", camera1);
    smgr->addCamera("cam2", camera2);

    bool curcam = false;
    u64 frame = 0;

    while(lkw->isRunning())
    {
        frame++;
        if(frame % 150 == 0) curcam = !curcam;

        if(curcam) smgr->setActiveCamera("cam2");
        else       smgr->setActiveCamera("cam2");

        camera2->get<Position>("Position")->lookAt(Vector3<f32>());

        smgr->updateScene();
        smgr->drawScene(Color4<u8>(100,101,140,255));

        //camera2->get<Position>("Position")->_position.x += sinf((f32)frame*50);

        glBegin( GL_TRIANGLES );
          glColor3f(1.0f, 0.0f, 0.0f );
          glVertex3f(0.0f, 0.0f, 0.0f);
          glColor3f(0.0f, 1.0f, 0.0f );
          glVertex3f(1.0f, 0.0f, 1.0f);
          glColor3f(0.0f, 0.0f, 1.0f );
          glVertex3f(2.0f, 0.0f, 0.0f);
        glEnd();

        //glRotatef(frame, 0.25f, 1.0f, 0.75f);
        /*glBegin( GL_TRIANGLES );
          glColor3f(0.0f, 0.0f, 0.0f );
          glVertex3f(1.0f, 3.0f, -4.0f);
          glColor3f(0.0f, 1.0f, 0.0f );
          glVertex3f(3.0f, -2.0f, -4.0f);
          glColor3f(0.0f, 0.0f, 1.0f );
          glVertex3f(-3.0f, -2.0f, -4.0f);
        glEnd();
        glBegin( GL_TRIANGLES );
          glColor3f(0.0f, 0.1f, 0.0f );
          glVertex3f(0.0f, 3.0f, -3.0f);
          glColor3f(0.0f, 0.0f, 1.0f );
          glVertex3f(3.0f, -2.0f, -2.0f);
          glColor3f(1.0f, 0.0f, 0.0f );
          glVertex3f(-3.0f, -2.0f, 2.0f);
        glEnd();*/
        glfwSwapBuffers();
    }

    glfwTerminate();

    return 0;
}

/*

Componentes Básicos

Position (x, y e z, podem ser usados para 2D ou 3D, em 2D o z é o z-index)
Image (2D apenas) *
VertexList
- Ele é capaz de se desenhar também
Texture *
BoundingBox (3D apenas)*
Keyboard
Mouse (em 3D vai ser fodinha fazer, mas é necessário)
- Para funcionar em 3D precisa de uma bounding box além da VertexList
Luzes
Billboard

*Não tão básicos

*/

//glfwGetWindowSize( &width, &height );
//height = height > 0 ? height : 1;

//glViewport( 0, 0, 1024, 768 );

//glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
//glClear( GL_COLOR_BUFFER_BIT );

//glMatrixMode( GL_PROJECTION );
//glLoadIdentity();
//gluPerspective( 65.0f, (GLfloat)1024/(GLfloat)768, 1.0f, 100.0f );

// Draw some rotating garbage
//glMatrixMode( GL_MODELVIEW );
//glLoadIdentity();
//gluLookAt(0.0f, -10.0f, 0.0f,
//          0.0f, 0.0f, 0.f,
//          0.0f, 0.0f, 1.0f );

//glTranslatef( 1.0f, 1.0f, 0.0f );
