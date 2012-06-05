#ifndef MODELCOMPONENT_H
#define MODELCOMPONENT_H
#include "IComponent.h"
#include "Mesh.h"
#include "Image.h"
namespace luck { namespace core
{
    Component(Model)
    {
        asset::Mesh* _model;
        asset::Image* _texture;
        void init()
        {
            //owner->addEventListener("Draw",eventCallback(this,&Model::handleDraw));
            owner->requires("Position");
            _model = nullptr;
            _texture = nullptr;
        }
        Model* model(asset::Mesh* model)
        {
            if(!model) return this;
            if(!model->hasVertexBuffer()) model->createVertexBuffer();
            _model = model;

            return this;
        }
        Model* texture(asset::Image* texture)
        {
            _texture = texture;
            return this;
        }
    };
}}
#endif
