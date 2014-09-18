#include <core/resources/resource.hpp>
#include <fstream>
#include <core/common/debug.hpp>

namespace luck
{
	bool text_resource::load(text_resource* r, std::string path)
	{
		std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
		if(!ifs)
		{
			WARN_IF(true, "There was an error opening the text file (", path, ")", strerror(errno));
			return false;
		}
		std::fstream::pos_type fileSize = ifs.tellg();
		ifs.seekg(0, std::ios::beg);
		std::vector<char> bytes((unsigned)fileSize);
		ifs.read(&bytes[0], fileSize);

		///@todo check for other errors after reading

		r->text = std::move(std::string(&bytes[0], (unsigned)fileSize));
		return true;
	}
}
