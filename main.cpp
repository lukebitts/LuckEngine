#include <iostream>
#include <GL/glfw.h>
#include "LuckEngine.h"

using namespace luck;
using namespace core;
using namespace event;
using namespace scene;

Component(Test)
{
    void init()
    {
        owner->addEventListener("EnterFrame",this);
    }
    void handleEvent(string type, Event* e = nullptr)
    {
        if(type == "EnterFrame")
            this->handleEnterFrame((EnterFrameEvent*)e);
    }
    f32 var(){ return _var; }
    Entity* var(f32 var)
    {
        _var = var;
        return owner;
    }
    void handleEnterFrame(EnterFrameEvent* e)
    {
        /*_var += 10.f * e->deltaTime; //this means that _var will increase by 10 each second, but _var
                                     // has to be a float for it to work, you will have to cast it to
                                     // int later if you don't want a float value.
        //std::cout<<(u32)_var<<"\n";*/

        Keyboard* k = owner->get<Keyboard>("Keyboard");
        if(k->isDown(GLFW_KEY_DOWN))
            std::cout<<"a";

    }
    ~Test(){ }
    private:
        f32 _var;
};

int main(int argc, char* argv[])
{

    LuckWindow* lkw = createLuckWindow(1024,768);
    if(!lkw) return -1;
    lkw->setWindowCaption("LuckEngine");

    SceneManager* smgr = SceneManager::getInstance();

    Entity* e = smgr->createEntity("PLAYER")
        ->add("Test Keyboard")
        ->get<Test>("Test")
        ->var(0);

    int     width, height;
    int     frame = 0;

    while(lkw->isRunning())
    {
        smgr->updateScene();

        /*frame++;

        glfwGetWindowSize( &width, &height );
        height = height > 0 ? height : 1;

        glViewport( 0, 0, width, height );

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f );

        // Draw some rotating garbage
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        gluLookAt(0.0f, -10.0f, 0.0f,
                  0.0f, 0.0f, 0.0f,
                  0.0f, 0.0f, 1.0f );

        //glTranslatef( 1.0f, 1.0f, 0.0f );
        glRotatef(frame, 0.25f, 1.0f, 0.75f);
        glBegin( GL_TRIANGLES );
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
        glEnd();
        glfwSwapBuffers();*/
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

*Não tão básicos

*/
