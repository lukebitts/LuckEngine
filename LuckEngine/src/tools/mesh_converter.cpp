#include <tools/mesh_converter.hpp>
#include <core/common/debug.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <fstream>

namespace luck
{
	namespace tools
	{
		namespace mesh
		{
			std::string save(const file& f, std::string path)
			{
				std::string file_path = get_file_path(path);
				std::string file_name = get_file_name(path);
				std::ofstream save_file(file_path+"/"+file_name+".l3d", std::ios::out | std::ios::binary);
				
				if(!save_file.is_open())
				{
					LOG("Error writing file ",file_path,"/",file_name,".l3d");
					return "";
				}

				save_file.write(&(f.header.file_type[0]),3);
				save_file.write((char*)&f.header.version,sizeof(f.header.version));
				auto size = f.vertices.size();
				save_file.write((char*)&size,sizeof(f.vertices.size()));

				for(size_t i = 0; i < size; ++i)
				{
					save_file.write((char*)&f.vertices[i].x,sizeof(f.vertices[0].x));
					save_file.write((char*)&f.vertices[i].y,sizeof(f.vertices[0].y));
					save_file.write((char*)&f.vertices[i].z,sizeof(f.vertices[0].z));
					save_file.write((char*)&f.vertices[i].u,sizeof(f.vertices[0].u));
					save_file.write((char*)&f.vertices[i].v,sizeof(f.vertices[0].v));
					save_file.write((char*)&f.vertices[i].nx,sizeof(f.vertices[0].nx));
					save_file.write((char*)&f.vertices[i].ny,sizeof(f.vertices[0].ny));
					save_file.write((char*)&f.vertices[i].nz,sizeof(f.vertices[0].nz));
					save_file.write((char*)&f.vertices[i].tx,sizeof(f.vertices[0].tx));
					save_file.write((char*)&f.vertices[i].ty,sizeof(f.vertices[0].ty));
					save_file.write((char*)&f.vertices[i].tz,sizeof(f.vertices[0].tz));
					save_file.write((char*)&f.vertices[i].bx,sizeof(f.vertices[0].bx));
					save_file.write((char*)&f.vertices[i].by,sizeof(f.vertices[0].by));
					save_file.write((char*)&f.vertices[i].bz,sizeof(f.vertices[0].bz));

					//LOG(f.vertices[i].x," ",f.vertices[i].y," ",f.vertices[i].z);
				}
				
				//save_file.write((char*)&f.vertices[0],sizeof(f.vertices[0])*f.vertices.size());
				size = f.indices.size();
				save_file.write((char*)&size,sizeof(f.indices.size()));
				for(size_t i = 0; i < f.indices.size(); ++i)
				{
					size = f.indices[i].indices.size();
					save_file.write((char*)&size,sizeof(f.indices[i].indices.size()));
					for(size_t j = 0; j < size; ++j)
					{
						save_file.write((char*)&f.indices[i].indices[j],sizeof(f.indices[i].indices[j]));
					}
				}
				
				save_file.close();

				return std::string(file_path)+"/"+file_name+".l3d";
			}

			file load(std::string path)
			{
				std::ifstream load_file(path, std::ios::in | std::ios::binary);
				if(!load_file.is_open())
				{
					LOG("Error loading file (",path,")"); ///@todo specify error
					return file{};
				}

				file f;
				load_file.read(&(f.header.file_type[0]), 3);
				
				if(std::memcmp(&(f.header.file_type[0]), "L3D", 3) != 0 || load_file.bad() || load_file.fail() || load_file.eof())
				{
					LOG("Error reading file ",path);
					return file{};
				}

				load_file.read((char*)&f.header.version,sizeof(f.header.version));

				switch(f.header.version)
				{
					case 1:
						decltype(f.vertices.size()) vertex_number;
						load_file.read((char*)&vertex_number,sizeof(vertex_number));
						f.vertices.reserve(vertex_number);
						for(unsigned i = 0; i < vertex_number; ++i)
						{
							f.vertices.emplace_back();
							load_file.read((char*)&f.vertices[i].x,sizeof(f.vertices[0].x));
							load_file.read((char*)&f.vertices[i].y,sizeof(f.vertices[0].y));
							load_file.read((char*)&f.vertices[i].z,sizeof(f.vertices[0].z));
							load_file.read((char*)&f.vertices[i].u,sizeof(f.vertices[0].u));
							load_file.read((char*)&f.vertices[i].v,sizeof(f.vertices[0].v));
							load_file.read((char*)&f.vertices[i].nx,sizeof(f.vertices[0].nx));
							load_file.read((char*)&f.vertices[i].ny,sizeof(f.vertices[0].ny));
							load_file.read((char*)&f.vertices[i].nz,sizeof(f.vertices[0].nz));
							load_file.read((char*)&f.vertices[i].tx,sizeof(f.vertices[0].tx));
							load_file.read((char*)&f.vertices[i].ty,sizeof(f.vertices[0].ty));
							load_file.read((char*)&f.vertices[i].tz,sizeof(f.vertices[0].tz));
							load_file.read((char*)&f.vertices[i].bx,sizeof(f.vertices[0].bx));
							load_file.read((char*)&f.vertices[i].by,sizeof(f.vertices[0].by));
							load_file.read((char*)&f.vertices[i].bz,sizeof(f.vertices[0].bz));

							//LOG(f.vertices[i].x," ",f.vertices[i].y," ",f.vertices[i].z);
						}
						decltype(f.indices.size()) index_size;
						load_file.read((char*)&index_size,sizeof(index_size));
						f.indices.reserve(index_size);
						for(size_t i = 0; i < index_size; ++i)
						{
							decltype(f.indices.size()) index_size2;
							load_file.read((char*)&index_size2,sizeof(index_size2));
							f.indices.emplace_back();
							f.indices[i].indices.reserve(index_size2);
							for(size_t j = 0; j < index_size2; ++j)
							{
								uint32_t index;
								load_file.read((char*)&index,sizeof(index));
								f.indices[i].indices.push_back(index);
							}
						}
						
						if(load_file.bad() || load_file.fail() || load_file.eof())
						{
							LOG("Error reading file ",path);
							return file{};
						}
						break;
					default:
						LOG("Wrong file version");
						return file{};
						break;
				};
				
				return f;
			}

			std::vector<std::string> convert(std::string path, std::string output_path, bool force)
			{
				if (!force)
				{
					std::vector<std::string> file_info = tools::split(path, '.');
					std::string new_file = tools::reform(file_info.begin(), file_info.end() - 1, '.') + ".l3d";

					file_info.clear();
					file_info.push_back(new_file);

					if (tools::file_exists(new_file))
					{
						LOG(new_file, " was already converted, skipping.");
						return file_info;
					}
				}

				file f;

				const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
				if(!scene)
				{
					WARN_IF(true, aiGetErrorString(), "(", path, ")");
					return {};
				}

				f.indices.reserve(scene->mNumMeshes);

				for(unsigned i = 0; i < scene->mNumMeshes; ++i)
				{
					aiMesh* mesh = scene->mMeshes[i];
					
					if(mesh->HasPositions())
					{
						f.vertices.reserve(f.vertices.size() + mesh->mNumVertices);
						uint32_t offset = (uint32_t)f.vertices.size();

						for(unsigned j = 0; j < mesh->mNumVertices; ++j)
						{
							vertex v{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
							v.x = mesh->mVertices[j].x;
							v.y = mesh->mVertices[j].y;
							v.z = mesh->mVertices[j].z;
							
							if(mesh->HasTextureCoords(0))
							{
								v.u = mesh->mTextureCoords[0][j].x;
								v.v = mesh->mTextureCoords[0][j].y;
							}
							
							if(mesh->HasNormals() && mesh->HasTangentsAndBitangents())
							{
								v.nx = mesh->mNormals[j].x;
								v.ny = mesh->mNormals[j].y;
								v.nz = mesh->mNormals[j].z;
								
								v.tx = mesh->mTangents[j].x;
								v.ty = mesh->mTangents[j].y;
								v.tz = mesh->mTangents[j].z;
								
								v.bx = mesh->mBitangents[j].x;
								v.by = mesh->mBitangents[j].y;
								v.bz = mesh->mBitangents[j].z;
							}
							f.vertices.push_back(v);
						}

						f.indices.emplace_back();
						
						for(unsigned j = 0; j < mesh->mNumFaces; ++j)
						{
							f.indices[i].indices.push_back(offset + mesh->mFaces[j].mIndices[0]);
							f.indices[i].indices.push_back(offset + mesh->mFaces[j].mIndices[1]);
							f.indices[i].indices.push_back(offset + mesh->mFaces[j].mIndices[2]);
						}
					}
				}
				
				aiReleaseImport(scene);

				std::string mesh_path = save(f, output_path == "" ? path : output_path + '/' + get_file_from_path(path));
				std::vector<std::string> ret;
				ret.push_back(mesh_path);
				return ret;
			}
		}
	}
}