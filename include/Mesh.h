#ifndef MESH_H
#define MESH_H
#include "luck.h"
#include "Vertex.h"
namespace luck { namespace asset
{
    struct Mesh
    {
        vector<core::Vertex> vertexList;
        vector<s32> faceList;
    };
}}
#endif

