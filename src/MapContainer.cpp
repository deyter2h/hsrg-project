#include "MapContainer.h"

std::string MapContainer::generateName() {
	std::string out;
	for (int i = 0; i < 10; i++) {
		out += std::to_string(GetRandomValue(0, 9));
	}
	return out;
}

MapContainer::MapContainer() {
	//creates new folder by default
	this->workingDir = std::string(GetApplicationDirectory()) + generateName();
	std::filesystem::create_directories(workingDir);
	std::ofstream file(workingDir + "/map.txt");
	file.close();
}

void MapContainer::modifyMap(std::string audioPath, MapData mapData) const {
	if (!audioPath.empty()) {
		//Sub-sing the mp3 file
		std::ifstream  src(audioPath, std::ios::binary);
		std::ofstream  dst(workingDir + "/audio.mp3", std::ios::binary);
		dst << src.rdbuf();
	}

	//Writing to file
	std::ofstream out(workingDir + "/map.txt");
	out << mapData.offset << "\n";
	out << "\n";
	// Write sections
	out << mapData.sections.size() << "\n";
	for (auto& s : mapData.sections) {
		out
			<< s.signature.numerator << " "
			<< s.signature.denominator << " "
			<< s.bpm << " "
			<< s.start_ms << " "
			<< s.end_ms << "\n";
	}
	out << "\n";
	// Write notes
	out << mapData.notes.size() << "\n";
	for (auto& n : mapData.notes) {
		out
			<< n.timing_ms << " "
			<< n.length_ms << "\n";
	}
	
}

MapContainerData MapContainer::loadExisting(std::string folder) {
	this->workingDir = folder;

	std::ifstream in(workingDir + "/map.txt");
	if (!in) return {};

	MapData result;

	// 1) Read the offset
	if (!(in >> result.offset)) return {};
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // consume the rest of that line

	// 2) skip any blank line
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// 3) Read sections
	size_t count;
	if (!(in >> count)) return {};
	for (size_t i = 0; i < count; i++) {
		Section s;
		if (!(in
			>> s.signature.numerator
			>> s.signature.denominator
			>> s.bpm
			>> s.start_ms
			>> s.end_ms))
			return {};
		s.signature.str =
			std::to_string(s.signature.numerator)
			+ "/" + std::to_string(s.signature.denominator);
		result.sections.push_back(s);
	}

	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// 4) Read notes count
	if (!(in >> count)) return {};
	for (size_t i = 0; i < count; i++) {
		Note n;
		if (!(in >> n.timing_ms >> n.length_ms))
			return {};
		result.notes.push_back(n);
	}

	this->loadedMusic = LoadMusicStream(std::string(workingDir + "/audio.mp3").c_str());

	return { loadedMusic, result };
}

std::string MapContainer::getWorkingPath() {
	return this->workingDir;
}

std::string MapContainer::getAudioPath() {
	return this->workingDir + "/audio.mp3";
}