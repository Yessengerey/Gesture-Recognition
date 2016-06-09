#pragma once

#include <Windows.h>
class GameTimer {
public:
	GameTimer(void);
	~GameTimer(void) {}

	//How many milliseconds have passed since the GameTimer was created
	float	GetMS() const;

	//How many milliseconds have passed since GetTimedMS was last called
	float	GetTimedMS();

protected:
	LARGE_INTEGER	start;			//Start of timer
	LARGE_INTEGER	frequency;		//Ticks Per Second

	float lastTime;					//Last time GetTimedMS was called
};

