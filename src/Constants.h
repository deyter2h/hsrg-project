#include <string>

const int DEFAULT_SECTION_LENGTH = 1000 * 3600;
const int DEFAULT_BPM = 60;
const int MAX_BPM = 99999;

const float PLAYBACK_MULTIPLIERS_RAW[7] = {0.25f, 0.5f, 0.75f, 1.0f, 1.5f, 1.75f, 2.0f};
const std::string PLAYBACK_MULTIPLIERS_STR = "0.25;0.5;0.75;1.0;1.5;1.75;2.0";

const std::string SIGNATURES_STR = "32/4;16/4;8/4;4/4;2/4;1/4;12/4;6/4;3/4";
const int SIGNATURES_RAW[9] = { 32, 16, 8, 4, 2, 1, 12, 6, 3 };

