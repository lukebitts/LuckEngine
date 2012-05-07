#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include "luck.h"
#include "Vertex.h"
namespace luck { namespace asset
{
    struct Mesh
    {
        vector<core::Vertex> vertexList;
        vector<u32> faceList;
        u32 vboID;
        u32 cboID;
        u32 iboID;
        Mesh() : _hasVertexBuffer(false){}
        void createVertexBuffer()
        {
            if(_hasVertexBuffer) return;

            glGenBuffers(1, &vboID);
            glBindBuffer(GL_ARRAY_BUFFER, vboID);
            glBufferData(GL_ARRAY_BUFFER, vertexList.size()*sizeof(core::Vertex), &vertexList[0], GL_STATIC_DRAW);

            glGenBuffers(1, &cboID);
            glBindBuffer(GL_ARRAY_BUFFER, cboID);
            glBufferData(GL_ARRAY_BUFFER, vertexList.size()*sizeof(core::Vertex), &vertexList[0], GL_STATIC_DRAW);

            glGenBuffers(1, &iboID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceList.size()*sizeof(u32), &faceList[0], GL_STATIC_DRAW);

            _hasVertexBuffer = true;
        }
        bool hasVertexBuffer(){ return _hasVertexBuffer; }
        private:
            bool _hasVertexBuffer;
    };
}}
#endif

