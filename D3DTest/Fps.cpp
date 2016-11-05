#include "FPS.h"
#include "D3D11_Framework.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

void FpsClass::Init()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
}

void FpsClass::Frame()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}

int FpsClass::GetFps()
{
	return m_fps;
}