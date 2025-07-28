#pragma once

#include "Structs.h"
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

struct TextMapStore {
    explicit TextMapStore(std::string path)
        : _path(std::move(path))
    {}

    bool save(const Map& m) {
        std::ofstream out(_path);
        if (!out) return false;

        out << m.offset << "\n";

        out << "\n";

        // Write sections
        out << m.sections.size() << "\n";
        for (auto& s : m.sections) {
            out
                << s.signature.numerator << " "
                << s.signature.denominator << " "
                << s.bpm << " "
                << s.start_ms << " "
                << s.end_ms << "\n";
        }
        out << "\n";

        // Write notes
        out << m.notes.size() << "\n";
        for (auto& n : m.notes) {
            out
                << n.timing_ms << " "
                << n.length_ms << "\n";
        }
        return out.good();
    }

    std::optional<Map> load() {
        std::ifstream in(_path);
        if (!in) return std::nullopt;

        Map result;

        // 1) Read the offset
        if (!(in >> result.offset)) return std::nullopt;
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // consume the rest of that line

        // 2) skip any blank line
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // 3) Read sections
        size_t count;
        if (!(in >> count)) return std::nullopt;
        for (size_t i = 0; i < count; i++) {
            Section s;
            if (!(in
                >> s.signature.numerator
                >> s.signature.denominator
                >> s.bpm
                >> s.start_ms
                >> s.end_ms))
                return std::nullopt;
            s.signature.str =
                std::to_string(s.signature.numerator)
                + "/" + std::to_string(s.signature.denominator);
            result.sections.push_back(s);
        }

        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // 4) Read notes count
        if (!(in >> count)) return std::nullopt;
        for (size_t i = 0; i < count; i++) {
            Note n;
            if (!(in >> n.timing_ms >> n.length_ms))
                return std::nullopt;
            result.notes.push_back(n);
        }

        return result;
    }

private:
    std::string _path;
};
