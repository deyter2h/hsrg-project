#pragma once
#include <vector>
#include <chrono>
#include "Structs.h"

class Timeable {
public:
	Timeable();

	void tick();

	int getTimePassed() const;

	void setMul(float m);
	void resume();
	void pause();
	void restart();
	bool isPaused() const;

	void incrementTime(int ms);
	void setTime(int ms);

protected:
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point pauseTime;
	int time_added;
	bool _isPaused;

	float _speedMul = 1.0f;

	int generateTimePassed();
	int timePassedMs = 0;
};