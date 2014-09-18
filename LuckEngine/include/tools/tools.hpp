#ifndef _TOOLS_HPP_
#define _TOOLS_HPP_

#include <stdint.h>
#include <vector>
#include <array>
#include <string>

namespace luck
{
	namespace tools
	{
		struct file_header
		{
			char file_type[3];// {'X', 'X', 'X'};
			//std::array<char, 3> file_type({ 'X', 'X', 'X' });
			uint32_t version = 0;
			//uint32_t name_size = 0;
			//char* name = nullptr;
			inline file_header() { file_type[0] = 'X'; file_type[1] = 'X'; file_type[2] = 'X'; }
			inline file_header(char t1, char t2, char t3, uint32_t version) : version(version) { file_type[0] = t1; file_type[1] = t2; file_type[2] = t3; }
			//inline ~file_header() { delete[] name; }
		};
		
		std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems);
		std::vector<std::string> split(const std::string& s, char delim);
		std::string get_file_path(std::string path);
		std::string get_file_from_path(std::string path);
		std::string wget_file_from_path(std::string path); ///@todo: use boost filesystem here? or find a good alternative
		std::string get_file_name(std::string file);

		template <class Begin, class End>
		inline std::string reform(Begin b, End e, char delimiter)
		{
			std::string ret;
			while(b != e)
			{
				ret += *b;
				if(b+1!=e)
					ret += delimiter;
				++b;
			}
			return ret;
		}
	}
}

#endif //_TOOLS_HPP_