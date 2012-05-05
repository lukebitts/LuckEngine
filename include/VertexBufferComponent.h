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
            u32* faceList;
            u32 faceAmm;
            u32 vboID;
            u32 iboID;
            BufferInfo(Vertex vertexList[], u32* faceList, u32 faceAmm, s32 type, u32 vboID, u32 iboID)
            {
                this->type = type;
                this->vertexList = vertexList;
                this->faceList = faceList;
                this->faceAmm = faceAmm;
                this->vboID = vboID;
                this->iboID = iboID;
            }
        };
        void setVertexBuffer(Vertex vertexList[], u32* faceList, u32 faceAmm, s32 type)
        {
            /// @todo meshs should have each a vertex buffer (So if we have multiple copies, it's still the same on the video board)
            u32 vertexBufferID;
            u32 indexBufferID;

            glGenBuffers(1, &vertexBufferID);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
            glBufferData(GL_ARRAY_BUFFER, faceAmm*3*sizeof(f32), &vertexList[0], GL_STATIC_DRAW);

            glGenBuffers(1, &indexBufferID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceAmm*3*sizeof(u32), &faceList[0], GL_STATIC_DRAW);

            //glEnableVertexAttribArray(owner->id);

            bufferList.push_back(BufferInfo(vertexList, faceList, faceAmm, type, vertexBufferID, indexBufferID));
        }
        vector<BufferInfo> bufferList;
    };
}}
#endif
