#pragma once
#include <vector>
#include <chrono>
#include "Structs.h"

//const std::vector<SignatureInfo> common_signatures ={
//	{ 1, 1, "1/1" },
//	{ 1, 32, "1/32" },
//	{ 1, 16, "1/16" },
//	{ 1, 8, "1/8" },
//	{ 1, 4, "1/4" },
//	{ 1, 2, "1/2" },
//	{ 1, 6, "1/6" },
//	{ 1, 3, "1/3" },
//};

class Timeable {
public:
	Timeable();

	void tick();

	int getTimePassed() const;

	void resume();
	void pause();
	void restart();
	bool isPaused() const;
	
	//void setBpm(unsigned int bpm);
	//void setSignature(SignatureInfo sign);
	void incrementTime(int ms);

	//double getBeatTime() const;

	//int getSignatureDivision() const;
	//std::vector<Beat> calculateBeats(unsigned int segments) const;

protected:
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point pauseTime;
	int time_added;
	bool _isPaused;

	//int bpm = 60;
	//int offset = 0;
	//SignatureInfo signature;

	int generateTimePassed();
	int timePassedMs = 0;
};