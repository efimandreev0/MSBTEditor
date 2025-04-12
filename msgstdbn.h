#pragma once
#include <fstream>
#include <cstdint>
#include <string>
#include <iostream>
#include "utils.h"
#include "txt2.h"
using namespace std;
class msgstdbn
{
public:
	struct MsgStdBn;
	static void read(string file, bool ignoreTags, bool debug = false);
	static void write(string file, bool debug);
};

