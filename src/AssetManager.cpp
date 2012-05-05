#include "AssetManager.h"
#include "Split.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace luck;
using namespace core;
using namespace asset;

AssetManager* AssetManager::_instance = nullptr;

Mesh* AssetManager::_loadObj(string path)
{
    /// @todo create a static OBJLoader class or something (OBJLoader::load(string) -> Mesh*)
    Mesh* ret = new Mesh();
    std::ifstream objFile;
    objFile.open(path);

    string line;
    if (!objFile.is_open()) return nullptr;
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
                ret->vertexList.push_back(Vertex(Vector3<f32>(v[0],v[1],v[2]),Color4(rand()%256,rand()%256,rand()%256,255)));
                break;
            }
            case 'f':
            {
                vector<string> face = split(line,' ');
                ret->faceList.push_back(atoi(face[1].c_str())-1);
                ret->faceList.push_back(atoi(face[2].c_str())-1);
                ret->faceList.push_back(atoi(face[3].c_str())-1);
                if(face.size() == 5)
                {
                    ret->faceList.push_back(atoi(face[1].c_str())-1);
                    ret->faceList.push_back(atoi(face[3].c_str())-1);
                    ret->faceList.push_back(atoi(face[4].c_str())-1);
                }
            }
        }
    }
    objFile.close();
    return ret;
}
