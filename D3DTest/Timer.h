#pragma once

#include "D3D11_Framework.h"

using namespace D3D11Framework;

class Timer
{
public:
	bool Init();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	INT64 m_startTime;
	float m_ticksPerMs;
	float m_frameTime;
};