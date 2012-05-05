#ifndef MODELCOMPONENT_H
#define MODELCOMPONENT_H
#include "IComponent.h"
#include "Mesh.h"
namespace luck { namespace core
{
    Component(Model)
    {
        asset::Mesh* _model;
        void init()
        {
            //owner->addEventListener("Draw",eventCallback(this,&Model::handleDraw));
            owner->requires("Position");
            _model = nullptr;
        }
        Model* model(asset::Mesh* model)
        {
            if(!model->hasVertexBuffer()) model->createVertexBuffer();
            _model = model;
            //VertexBuffer* vb = owner->get<VertexBuffer>("VertexBuffer");
            //vb->setVertexBuffer(&_model->vertexList[0],&_model->faceList[0],_model->faceList.size()/3,GL_TRIANGLES);
            //std::cout<<"On Model, the faceList.size()/3 is "<<_model->faceList.size()/3<<"\n";
        }
    };
}}
#endif
