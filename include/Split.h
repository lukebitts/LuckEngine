#ifndef SPLIT_H
#define SPLIT_H
#include "luck.h"
#include <sstream>
namespace luck { namespace core
{
    vector<string>& split(const string &s, char delim, vector<string> &elems);
    vector<string> split(const string &s, char delim);
}}
#endif
