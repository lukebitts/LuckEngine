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
            //std::cout<<"a";
        }
    }
    ~Test(){ }
};

Component(MoveCamera)
{
    void init()
    {
        owner->addEventListener("EnterFrame",this);
        owner->addEventListener("KeyDown",this);
        count = 0;
    }
    void handleEvent(string type, Event* e)
    {
        if(type == "EnterFrame")
            handleEnterFrame((EnterFrameEvent*)e);
        else if(type == "KeyDown")
            handleKeyDown((KeyEvent*)e);
    }
    void handleKeyDown(KeyEvent* e)
    {
        if(e->keyCode == 'Q')
        {
            Position* pos = owner->get<Position>("Position");
            pos->lookAt(Vector3<f32>());
        }
    }
    void handleEnterFrame(EnterFrameEvent* e)
    {
        Position* pos = owner->get<Position>("Position");
        Keyboard* k = owner->get<Keyboard>("Keyboard");
        ///@todo move the camera based on its rotation

        if(k->isDown('A'))
        {
            pos->_position.x -= 10.f * e->deltaTime;
        }
        if(k->isDown('D'))
        {
            pos->_position.x += 10.f * e->deltaTime;
        }
        if(k->isDown('W'))
        {
            pos->_position.z -= 10.f * e->deltaTime;
        }
        if(k->isDown('S'))
        {
            pos->_position.z += 10.f * e->deltaTime;
        }
        if(k->isDown('R'))
        {
            pos->_position.y += 10.f * e->deltaTime;
        }
        if(k->isDown('F'))
        {
            pos->_position.y -= 10.f * e->deltaTime;
        }
    }
    f32 count;
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

    Entity* camera = smgr->createEntity("Camera Keyboard MoveCamera")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,5.f,1.f))->lookAt(Vector3<f32>(0.f,0.f,0.f))
        ->owner
        ->get<Camera>("Camera")->fov(85.f)->near(1.f)->far(100.f)
        ->owner;

    smgr->addCamera("cam", camera);
    glEnable(GL_POLYGON_SMOOTH);
    int frame1 = 0;
    int frame2 = 0;
    while(lkw->isRunning())
    {
        smgr->updateScene();
        smgr->drawScene(Color4(100,101,140,255));

        glPushMatrix();
        glRotatef(frame1, 0.0f, 1.0f, 0.0f);
        glBegin( GL_QUADS );
          glColor3f(1.0f, 0.0f, 0.0f );
          glVertex3f(-2.f, -0.5f, 2.0f);
          glColor3f(0.0f, 1.0f, 0.0f );
          glVertex3f(-2.0f, -0.5f, -2.0f);
          glColor3f(0.0f, 0.0f, 1.0f );
          glVertex3f(2.0f, -0.5f, -2.0f);
          glColor3f(1.0f, 1.0f, 0.0f );
          glVertex3f(2.0f, -0.5f, 2.0f);
        glEnd();
        glPopMatrix();

        glPushMatrix();
        glRotatef(frame2--,0.f,1.f,0.f);
        glBegin( GL_QUADS );
          glColor3f(0.0f, 0.0f, 1.0f );
          glVertex3f(-1.f, 0.0f, 1.0f);
          glColor3f(1.0f, 0.0f, 0.0f );
          glVertex3f(-1.0f, 0.0f, -1.0f);
          glColor3f(0.0f, 1.0f, 0.0f );
          glVertex3f(1.0f, 0.0f, -1.0f);
          glColor3f(0.0f, 1.0f, 1.0f );
          glVertex3f(1.0f, 0.0f, 1.0f);
        glEnd();
        glPopMatrix();
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
BoundingBox (3D e 2D(será que vale a pena fazer 2D também, o foco da engine não é 2D [por enquanto anyway] ) )*
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
