#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include "luck.h"
#include "Mesh.h"
#include "event.h"
namespace luck { namespace asset {
    enum assetType
    {
        ASSET_MESH,
        ASSET_IMAGE,
        ASSET_SOUND
    };
    class AssetManager
    {
        private:
            static AssetManager* _instance;
            AssetManager()
            {
                //alala
            }
            struct _toLoadInfo
            {
                string path;
                assetType type;
                _toLoadInfo(string path, assetType type) : path(path), type(type){}
            };
            vector<_toLoadInfo> _loadQueue;
            map<string, Mesh*> _meshList;
            Mesh* _loadObj(string path);
        public:
            static AssetManager* getInstance()
            {
                if(!_instance) _instance = new AssetManager();
                return _instance;
            }
            void addToLoadQueue(string path, assetType type)
            {
                _loadQueue.push_back(_toLoadInfo(path,type));
            }
            template <typename Func>
            void load(Func func)
            {
                while(_loadQueue.size() > 0)
                {
                    if(_loadQueue[0].type == ASSET_MESH)
                    {
                        /// @todo check file type
                        if(_meshList[_loadQueue[0].path] == NULL)
                        {
                            _meshList[_loadQueue[0].path] = _loadObj(_loadQueue[0].path);
                        }
                    }
                    else if(_loadQueue[0].type == ASSET_IMAGE)
                    {

                    }
                    else if(_loadQueue[0].type == ASSET_SOUND)
                    {

                    }
                    /// @todo create a LoadProgressEvent struct to pass
                    func(event::Event(_loadQueue[0].path));
                    _loadQueue.erase(_loadQueue.begin());
                }
            }
            Mesh* getLoadedMesh(string name)
            {
                return _meshList[name];
            }
    };
}}
#endif
