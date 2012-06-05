#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include "luck.h"
#include "Mesh.h"
#include "Image.h"
#include "LoadProgressEvent.h"
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
            map<string, Image*> _imageList;
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
                u32 totalToLoad = _loadQueue.size();
                u32 totalLoaded = 0;
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
                        if(_imageList[_loadQueue[0].path] == NULL)
                        {
                            _imageList[_loadQueue[0].path] = new Image(_loadQueue[0].path);
                        }
                    }
                    else if(_loadQueue[0].type == ASSET_SOUND)
                    {

                    }
                    func(event::LoadProgressEvent(_loadQueue[0].path,++totalLoaded,totalToLoad));
                    _loadQueue.erase(_loadQueue.begin());
                }
            }
            Mesh* getLoadedMesh(string name)
            {
                return _meshList[name];
            }
            Image* getLoadedImage(string name)
            {
                return _imageList[name];
            }
    };
}}
#endif
