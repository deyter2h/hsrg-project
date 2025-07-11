#include <vector>
#include <chrono>

enum Signature {
	ONE_FOUR,
	ONE_THREE,
};

struct Beat {
	Signature signature;
    int timing_ms;
};

class Timeable {
public:
	Timeable();

	void tick() const;

	int getTimePassed() const;

	void resume();
	void pause();
	void restart();
	bool isPaused() const;
	
	void setBpm(unsigned int bpm);
	void setSignature(Signature sign);
	void incrementTime(int ms);

	unsigned int getBeatTime() const;
	std::vector<Beat> getCurrentBeats(int timing_ms, unsigned int distance_ms) const;

	virtual void onBeat() = 0;

private:
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point pauseTime;
	int time_added;
	bool _isPaused;

	unsigned int bpm;
	Signature signature;
};