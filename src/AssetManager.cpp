#include "AssetManager.h"
#include "Split.h"
#include "Vector2.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace luck;
using namespace core;
using namespace asset;

AssetManager* AssetManager::_instance = nullptr;

Color4 randomColor()
{
    return Color4(/*(f32)rand()/RAND_MAX,(f32)rand()/RAND_MAX,(f32)rand()/RAND_MAX,1.f*/);
}

bool floatCompare(f32 a, f32 b)
{
    if(fabs(a-b) < 0.00001)
        return true;
    return false;
}

Mesh* AssetManager::_loadObj(string path)
{
    /// @todo create a static OBJLoader class or something (OBJLoader::load(string) -> Mesh*)
    /// @todo use this http://en.wikipedia.org/wiki/Wavefront_.obj_file
    Mesh* ret = new Mesh();
    std::ifstream objFile;
    objFile.open(path);

    vector<u32> tempVertexList;
    vector<core::Vector2<f32>> tempUvList;

    string line;
    if (!objFile.is_open()) return nullptr;
    while (!objFile.eof())
    {
        std::getline(objFile,line);
        switch(line[0])
        {
            case 'v':
            {
                if(line[1] == ' ')
                {
                    vector<string> vertex = split(line,' ');
                    f64 v[3] = {
                        atof(vertex[1].c_str()),
                        atof(vertex[2].c_str()),
                        atof(vertex[3].c_str())
                    };
                    ret->vertexList.push_back(Vertex(Vector3<f32>(v[0],v[1],v[2]), randomColor(), Vector2<f32>(0,0) ));
                }
                if(line[1] == 't')
                {
                    vector<string> vertexUV = split(line,' ');
                    tempUvList.push_back(Vector2<f32>(
                        atof(vertexUV[1].c_str()),
                        atof(vertexUV[2].c_str())
                    ));
                }
                break;
            }
            case 'f':
            {
                s32 vList[4] = {-1};
                vector<string> face = split(line,' ');
                for(u32 i = 1; i < face.size(); i++)
                {
                    vector<string> vInfo = split(face[i],'/');
                    u32 v = atoi(vInfo[0].c_str())-1;

                    if(vInfo.size() >= 2)
                    {
                        u32 uv = atoi(vInfo[1].c_str())-1;
                        bool found = false;
                        for(u32 j = 0; j < tempVertexList.size(); j++)
                        {
                            if(tempVertexList[j] == v)
                            {
                                found = true;
                                break;
                            }
                        }
                        if(found)
                        {
                            Vertex fV = ret->vertexList[v];
                            if(!floatCompare(fV.u,tempUvList[uv][0]) || !floatCompare(fV.v,tempUvList[uv][1]))
                            {
                                v = ret->vertexList.size();
                                ret->vertexList.push_back(Vertex(Vector3<f32>(fV.x,fV.y,fV.z), randomColor(), Vector2<f32>(tempUvList[uv][0],tempUvList[uv][1]) ));
                            }
                        }
                        else
                        {
                            ret->vertexList[v].u = tempUvList[uv][0];
                            ret->vertexList[v].v = tempUvList[uv][1];
                            tempVertexList.push_back(v);
                        }
                    }
                    vList[i-1] = v;
                }
                ret->faceList.push_back(vList[0]);
                ret->faceList.push_back(vList[1]);
                ret->faceList.push_back(vList[2]);
                if(vList[3] != -1 && vList[3] != 0) ///@todo find out where this zero is coming from
                {
                    ret->faceList.push_back(vList[0]);
                    ret->faceList.push_back(vList[2]);
                    ret->faceList.push_back(vList[3]);
                }
                break;
            }
        }
    }
    objFile.close();
    ret->createVertexBuffer();
    return ret;
}
