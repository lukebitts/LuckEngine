#include <tools/image_converter.hpp>
#include <core/common/debug.hpp>
#include <core/common/lodepng.h>

#include <fstream>

namespace luck
{
	namespace tools
	{
		namespace image
		{
			std::string save(const file& f, std::string path)
			{
				std::string file_path = get_file_path(path);
				std::string file_name = get_file_name(path);
				std::ofstream save_file(file_path+"/"+file_name+".lif", std::ios::out | std::ios::binary);
				
				if(!save_file.is_open())
				{
					LOG("Error writing file ",file_path,"/",file_name,".lif");
					return "";
				}
				
				save_file.write(&(f.header.file_type[0]),3);
				save_file.write((char*)&f.header.version,sizeof(f.header.version));
				save_file.write((char*)&f.width,sizeof(f.width));
				save_file.write((char*)&f.height,sizeof(f.height));
				save_file.write((char*)f.data,f.width*f.height*4*sizeof(f.data[0]));
				
				save_file.close();
				
				return std::string(file_path)+"/"+file_name+".lif";
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
				
				if(std::memcmp(&(f.header.file_type[0]), "LIF", 3) != 0 || load_file.bad() || load_file.fail() || load_file.eof())
				{
					LOG("Error reading file ",path);
					return file{};
				}
				
				load_file.read((char*)&f.header.version,sizeof(f.header.version));
				
				switch(f.header.version)
				{
					case 1:
						load_file.read((char*)&f.width,sizeof(f.width));
						load_file.read((char*)&f.height,sizeof(f.height));
						f.data = new uint8_t[f.height * f.width * 4];
						for(decltype(f.height) i = 0; i < f.width * f.height * 4; ++i)
						{
							load_file.read((char*)&f.data[i],sizeof(f.data[0]));
							if(load_file.bad() || load_file.fail() || load_file.eof())
							{
								LOG("Error reading file ",path);
								return file{};
							}
						}
						break;
					default:
						LOG("Wrong file version");
						return file{};
						break;
				};
				
				return f;
			}

			std::string convert(std::string path, std::string output_path, bool force)
			{
				if (!force)
				{
					std::vector<std::string> file_info = tools::split(path,'.');
					std::string new_file = tools::reform(file_info.begin(), file_info.end() - 1,'.') + ".lif";

					if (tools::file_exists(new_file))
					{
						LOG(new_file, " was already converted, skipping.");
						return new_file;
					}
				}

				file f;

				std::vector<uint8_t> data;
				unsigned error = lodepng::decode(data, f.width, f.height, path);
				if(error != 0)
				{
					WARN_IF(true, lodepng_error_text(error), "(", path, ")");
					return "";
				}
				
				f.data = new uint8_t[f.width*f.height*4];
				std::memcpy(f.data,&data[0],data.size());
				return save(f, output_path == "" ? path : output_path + '/' + get_file_from_path(path));
			}
		}
	}
}