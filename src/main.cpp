#include "File.h"
#include "Beat.h"

#include <raylib.h>

int main() {
	InitAudioDevice();

	auto music = LoadMusicStream("C:/music/music.mp3");
	PlayMusicStream(music);
	SetMusicVolume(music, 0.0f);

	File file("C:/Users/Ilya/Desktop/py/format.txt");
	while (true) {
		try {
			auto isNew = file.check();
			if (isNew) {
				auto t = get_beat_timings(file._loadedInfo);
			}
			
		}
		catch (...) {
			std::exception_ptr p = std::current_exception();
			try {
				if (p) std::rethrow_exception(p);
			}
			catch (const std::exception& e) {
				std::clog << "Exception: " << e.what() << std::endl;
			}
			catch (...) {
				std::clog << "Unknown exception occurred" << std::endl;
			}
		}
		UpdateMusicStream(music);
		_sleep(100);
	}
	
	return 0;
}