#include "LuckEngine.h"

#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

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
    void handleEnterFrame(Event const& e)
    {
        EnterFrameEvent const& ef = (EnterFrameEvent const&)e;
        Keyboard* k = owner->get<Keyboard>("Keyboard");
        Position* p = owner->get<Position>("Position");
        if(k->isDown(GLFW_KEY_DOWN))
        {
            p->_rotation.y += 200.f * ef.deltaTime;
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
    vector<Vertex> vertexList;
    vector<s32> faceList;
    void init()
    {
        owner->addEventListener("Draw",eventCallback(this,&Model::handleDraw));
        owner->requires("VertexBuffer");
    }
    Model* loadModel(string path)
    {
        std::ifstream objFile;
        objFile.open(path);

        string line;
        if (!objFile.is_open()) return this;
        while (!objFile.eof())
        {
            std::getline(objFile,line);
            switch(line[0])
            {
                case 'v':
                {
                    vector<string> vertex = split(line,' ');
                    f64 v[3] = {
                        atof(vertex[1].c_str()),
                        atof(vertex[2].c_str()),
                        atof(vertex[3].c_str())
                    };
                    vertexList.push_back(Vertex(Vector3<f32>(v[0],v[1],v[2]), Color4(rand()%256,rand()%256,rand()%256,255)  ));
                    break;
                }
                case 'f':
                {
                    vector<string> face = split(line,' ');
                    faceList.push_back(atoi(face[1].c_str())-1);
                    faceList.push_back(atoi(face[2].c_str())-1);
                    faceList.push_back(atoi(face[3].c_str())-1);
                    if(face.size() == 5)
                        faceList.push_back(atoi(face[4].c_str())-1);
                    else
                        faceList.push_back(atoi(face[3].c_str())-1);
                    break;
                }
            }
        }

        objFile.close();
        return this;
    }
    void handleDraw(Event const& e)
    {
        VertexBuffer* vb = owner->get<VertexBuffer>("VertexBuffer");
        vb->setVertexBuffer(&vertexList[0],&faceList[0],faceList.size()/4,GL_QUADS);
    }
};

int main(int argc, char* argv[])
{
    LuckWindow* lkw = createLuckWindow(1024,768);
    if(!lkw) return -1;
    lkw->setWindowCaption("LuckEngine");

    SceneManager* smgr = SceneManager::getInstance();

    smgr->createEntity("PLAYER")
        ->add("Test Model")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-5.f,0.f))->owner
        ->get<Model>("Model")->loadModel("assets/monkey.obj");

    smgr->createEntity("Model Test")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,-7.f,0.f))->owner
        ->get<Model>("Model")->loadModel("assets/cube.obj");

    smgr->createEntity("Camera FPSControl")
        ->get<Position>("Position")->position(Vector3<f32>(0.f,0.f,0.f))->lookAt(Vector3<f32>(0.f,0.f,-1.f))
        ->owner
        ->get<Camera>("Camera")->fov(85.f)->near(0.1f)->far(500.f)
        ->owner;

    smgr->addCamera("cam", smgr->find("Camera")[0]);

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
