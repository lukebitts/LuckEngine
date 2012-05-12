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
            if(!model) return this;
            if(!model->hasVertexBuffer()) model->createVertexBuffer();
            _model = model;

            return this;
        }
    };
}}
#endif
