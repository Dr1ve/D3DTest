#pragma once

#include "D3D11_Framework.h"
#include "MyRender.h"

using namespace D3D11Framework;

class MyInput : public InputListener
{
public:
	MyInput(MyRender *render);
	bool KeyPressed(const KeyEvent &arg);
	bool KeyReleased(const KeyEvent &arg);
	bool MouseMove(const MouseEvent &arg);
	bool MousePressed(const MouseEventClick &arg);
	bool MouseReleased(const MouseEventClick &arg);

private:
	MyRender *m_render;
};