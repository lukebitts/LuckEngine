#include "Split.h"

using namespace luck;
using namespace core;

vector<string>& luck::core::split(const string &s, char delim, vector<string> &elems)
{
    std::stringstream ss(s);
    string item;
    while(std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

vector<string> luck::core::split(const string &s, char delim)
{
    vector<string> elems;
    return split(s, delim, elems);
}
