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
        owner->addEventListener("EnterFrame",eventCallback(this,&Test::handleEnterFrame));
    }
    void handleEnterFrame(Event* e)
    {
        EnterFrameEvent* ef = (EnterFrameEvent*)e;
        Keyboard* k = owner->get<Keyboard>("Keyboard");
        if(k->isDown('A'))
        {
            std::cout<<"a";
        }
    }
    ~Test(){ }
};

Component(FPSControl)
{
    void init()
    {
        owner->requires("Keyboard");
        owner->addEventListener("EnterFrame",eventCallback(this,&FPSControl::handleEnterFrame));
        count = 0;
    }
    void handleEnterFrame(Event* e)
    {
        //Position* pos = owner->get<Position>("Position");
        //Keyboard* k = owner->get<Keyboard>("Keyboard");
    }
    f32 count;
};

int main(int argc, char* argv[])
{
    LuckWindow* lkw = createLuckWindow(1024,768);
    if(!lkw) return -1;
    lkw->setWindowCaption("LuckEngine");

    SceneManager* smgr = SceneManager::getInstance();

    Entity* player = smgr->createEntity("PLAYER")
        ->add("Test Position")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,1.f,0.f))->owner;

    Entity* camera = smgr->createEntity("Camera FPSControl")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,0.f,0.f))->lookAt(Vector3<f32>(0.f,0.f,-1.f))
        ->owner
        ->get<Camera>("Camera")->fov(85.f)->near(1.f)->far(100.f)
        ->owner;

    smgr->addCamera("cam", camera);

    while(lkw->isRunning())
    {
        smgr->updateScene();
        smgr->drawScene(Color4(100,101,140,255));

        for(u16 i = 0;i < 20;i++){
        glPushMatrix();
        glRotatef(0, 0.0f, 1.0f, 0.0f);
        glBegin( GL_QUADS );
          glColor3f(1.0f, 0.0f, 0.0f );
          glVertex3f(-2.f, -0.5f*i, 2.0f*i);
          glColor3f(0.0f, 1.0f, 0.0f );
          glVertex3f(-2.0f, -0.5f*i, -2.0f*i);
          glColor3f(0.0f, 0.0f, 1.0f );
          glVertex3f(2.0f, -0.5f*i, -2.0f*i);
          glColor3f(1.0f, 1.0f, 0.0f );
          glVertex3f(2.0f, -0.5f*i, 2.0f*i);
        glEnd();
        glPopMatrix();

        glPushMatrix();
        glRotatef(0,0.f,1.f,0.f);
        glBegin( GL_QUADS );
          glColor3f(0.0f, 0.0f, 1.0f );
          glVertex3f(-1.f, 1.0f*i, 1.0f*i);
          glColor3f(1.0f, 0.0f, 0.0f );
          glVertex3f(-1.0f, 1.0f*i, -1.0f*i);
          glColor3f(0.0f, 1.0f, 0.0f );
          glVertex3f(1.0f, 1.0f*i, -1.0f*i);
          glColor3f(0.0f, 1.0f, 1.0f );
          glVertex3f(1.0f, 1.0f*i, 1.0f*i);
        glEnd();
        glPopMatrix();
        }
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
