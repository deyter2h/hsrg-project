#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Structs.h"


class File {
public:
	File(std::string path);
	~File();

	bool check();
	bool isLineSkippable(const std::string& line);

	MapInfo _loadedInfo;
private:
	
	std::ifstream _file;
	std::string path;
};