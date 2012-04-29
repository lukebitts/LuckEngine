#ifndef VERTEXBUFFERCOMPONENT_H
#define VERTEXBUFFERCOMPONENT_H
#include "luck.h"
#include "IComponent.h"
#include "Vertex.h"
namespace luck { namespace core
{
    Component(VertexBuffer)
    {
        void init()
        {
            owner->requires("Position");
        }
        struct BufferInfo
        {
            s32 type;
            Vertex* vertexList;
            s32* faceList;
            s32 faceAmm;
            BufferInfo(Vertex vertexList[], s32* faceList, s32 faceAmm, s32 type)
            {
                this->type = type;
                this->vertexList = vertexList;
                this->faceList = faceList;
                this->faceAmm = faceAmm;
            }
        };
        void setVertexBuffer(Vertex vertexList[], s32* faceList, s32 faceAmm, s32 type)
        {
            bufferList.push_back(BufferInfo(vertexList, faceList, faceAmm, type));
        }
        vector<BufferInfo> bufferList;
    };
}}
#endif
