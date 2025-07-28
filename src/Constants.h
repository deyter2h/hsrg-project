#pragma once

#include <string>
#include <numeric>
#include <iterator>
#include "Structs.h"

const int DEFAULT_SECTION_LENGTH = 1000 * 3600;
const int DEFAULT_BPM = 60;
const int MAX_BPM = 99999;

const float PLAYBACK_MULTIPLIERS_RAW[7] = {0.25f, 0.5f, 0.75f, 1.0f, 1.5f, 1.75f, 2.0f};
const std::string PLAYBACK_MULTIPLIERS_STR = "0.25;0.5;0.75;1.0;1.5;1.75;2.0";

const std::vector<SignatureInfo> ALLOWED_SIGNATURES = {
	{ 1, 4, "1/4" },
	{ 4, 4, "4/4" },
	{ 3, 4, "3/4 "}
};

const std::string ALLOWED_SIGNATURES_STR = std::accumulate(
    std::next(ALLOWED_SIGNATURES.begin()), ALLOWED_SIGNATURES.end(),
    ALLOWED_SIGNATURES.front().str,
    [](const std::string& acc, const SignatureInfo& sig) {
        return acc + ";" + sig.str;
    }
);

inline int findSignatureIndex(int numerator, int denominator) {
    auto it = std::find_if(
        ALLOWED_SIGNATURES.begin(),
        ALLOWED_SIGNATURES.end(),
        [&](const SignatureInfo& sig) {
            return sig.numerator == numerator
                && sig.denominator == denominator;
        }
    );
    if (it == ALLOWED_SIGNATURES.end())
        return -1;
    return static_cast<int>(std::distance(ALLOWED_SIGNATURES.begin(), it));
}

