#pragma once
#pragma warning(disable:4996)
#include <fstream>
#include <cstdint>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include "utils.h"
using namespace std;
class umsbt
{
public:
	static void read(string file);
	static void write(string dir);
};

