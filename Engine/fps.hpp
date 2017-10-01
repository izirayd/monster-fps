#pragma once

#include "base.hpp"

#pragma comment(lib, "winmm.lib")

class CFps
{
public:
	float FPS;
	float framesPerSecond = 0.0f;
	float lastTime = 0.0f;
	float currentTime;


	void Run()
	{ 
		currentTime = timeGetTime() * 0.001f;
		++framesPerSecond;		
		if (currentTime - lastTime > 1.0f)
		{
			lastTime = currentTime;
			FPS = framesPerSecond;
			framesPerSecond = 0;
		}
	}
};
