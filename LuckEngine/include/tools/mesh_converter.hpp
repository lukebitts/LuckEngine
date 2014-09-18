#ifndef _MESH_CONVERTER_HPP
#define _MESH_CONVERTER_HPP

#include <vector>
#include "tools.hpp"
#include "../core/resources/resource.hpp"

namespace luck
{
	namespace tools
	{
		namespace mesh
		{
			typedef mesh_data_resource::vertex vertex;
			
			struct index_data
			{
				std::vector<uint32_t> indices;
			};
			
			struct file
			{
				file_header header{'L','3','D',1};
				std::vector<vertex> vertices;
				std::vector<index_data> indices;
			};
			
			std::string save(const file& f, std::string path);
			file load(std::string path);
			std::vector<std::string> convert(std::string path, std::string output_path = "");
		}
	}
}

#endif //_MESH_CONVERTER_HPP