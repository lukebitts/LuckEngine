#ifndef MESH_H
#define MESH_H
#include "luck.h"
#include "Vertex.h"
namespace luck { namespace asset
{
    struct Mesh
    {
        vector<core::Vertex> vertexList;
        vector<u32> faceList;
    };
}}
#endif

