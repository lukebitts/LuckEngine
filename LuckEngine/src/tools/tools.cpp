#include <tools/tools.hpp>

#include <vector>
#include <string>
#include <sstream>

namespace luck
{
	namespace tools
	{
		std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems)
		{
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
			return elems;
		}

		std::vector<std::string> split(const std::string& s, char delim)
		{
			std::vector<std::string> elems;
			split(s, delim, elems);
			return elems;
		}

		std::string get_file_path(std::string path)
		{
			auto words = split(path,'/'); ///@todo check if windows, mac or linux
			return reform(words.begin(),words.end()-1,'/');
		}

		std::string get_file_from_path(std::string path)
		{
			auto words = split(path,'/');
			return *(words.end()-1);
		}

		std::string wget_file_from_path(std::string path)
		{
			auto words = split(path, '\\');
			return *(words.end() - 1);
		}

		std::string get_file_name(std::string file)
		{
			auto words = split(get_file_from_path(file),'.');
			return reform(words.begin(),words.end()-1,'.');
		}
	}
}