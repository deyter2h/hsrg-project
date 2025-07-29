#pragma once

#include <string>
#include "Structs.h"
#include <fstream>
#include <filesystem>
#include "tinyfiledialog.h"

struct MapContainerData {
	Music music;
	MapData data;
};

class MapContainer {
public:
	MapContainer();

	void modifyMap(std::string audioPath, MapData mapData) const; // Saves

	MapContainerData loadExisting(std::string folder);

	std::string getWorkingPath();
	std::string getAudioPath();

private:
	std::string generateName();
	std::string workingDir;

	Music loadedMusic;
};