#include "File.h"
#include <filesystem>
#include <algorithm>

static std::vector<std::string> split(std::string line) {
    std::vector<std::string> ret;
    auto p = strtok(line.data(), " ");
    while (p != NULL) {
        ret.push_back(p);
        p = strtok(NULL, " ");
    }
    return ret;
}

File::File(std::string path) {
    this->_file.open(path);
    this->path = path;
    if (!_file) {
        std::cerr << "Ошибка при открытии файла\n";
        exit(666);
    }
}

File::~File() {
    this->_file.close();
}

bool File::check() {
    static auto last_time = std::filesystem::last_write_time(path);
    auto current_time = std::filesystem::last_write_time(path);

    if (current_time != last_time) {
        std::cout << "File changed\n";
        //later so only valid
    }
    else {
        std::cout << "File not changed\n";
        return false;
    }

    this->_loadedInfo = {};

    _file.clear();
    _file.seekg(0, std::ios::beg);

    std::string line;
    size_t index = 0;

    int sectionId = -1;
    int beatRowId = 0;

    while (std::getline(_file, line)) {
        if (isLineSkippable(line)) continue;
        auto parsed = split(line);

        if (index == 0) {
            std::cout << "Header read: " << line << '\n';
            _loadedInfo.songOffsetMs = std::stof(parsed[0]);
            _loadedInfo.playbackSpeed = std::stof(parsed[1]);
            index++;
            continue;
        }

        if (parsed.size() == 2) {
            std::cout << "Section read: " << line << '\n';
            this->_loadedInfo.sections.push_back({ (int)std::stof(parsed[0]), (int)std::stof(parsed[1])});
            sectionId++;
            beatRowId = 0;
        }

        if (parsed.size() == 1 && (parsed[0][0] == '+' || parsed[0][0] == '-')) {
            std::cout << "Beatrow read: " << line << '\n';
            auto row = BeatRow(this->_loadedInfo.sections[sectionId].divisions);
            for (int i = 0; i < parsed[0].size(); i++) {
                row[i] = parsed[0][i] == '+';
            }

            this->_loadedInfo.sections[sectionId].beats.push_back(row);
            beatRowId++;
        }
        if (parsed.size() == 1 && parsed[0][0] == '<') {
            std::cout << "Start read: " << sectionId << " " << beatRowId << '\n';
            this->_loadedInfo.start = { sectionId, beatRowId };
        }
       
        index++;
    }
    last_time = current_time;
    return true;
}

bool File::isLineSkippable(const std::string& line) {
    if (std::all_of(line.begin(), line.end(), isspace)) return true;
    size_t firstChar = line.find_first_not_of(" \t");
    return (firstChar != std::string::npos && line[firstChar] == '#');
}
