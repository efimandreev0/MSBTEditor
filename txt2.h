#pragma once
#include <fstream>
#include <cstdint>
#include <string>
#include <iostream>
#include "utils.h"
#pragma once
using namespace std;
class txt2
{
public:
	struct TXT2;
	static vector<wstring> read(ifstream& f, bool ignoreTags, bool debug = false);
	static void write(fstream& f, vector<string> strings, bool debug = false);
};

