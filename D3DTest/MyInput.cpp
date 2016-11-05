#include "MyInput.h"

using namespace D3D11Framework;

MyInput::MyInput(MyRender *render)
{
	m_render = render;
}

bool MyInput::KeyPressed(const KeyEvent &arg)
{
	switch (arg.code)
	{
	case KEY_BACK:
		if (m_render->str.length() > 0)
		{
			wchar_t *temp = new wchar_t[m_render->str.length()];
			m_render->str._Copy_s(temp, m_render->str.length(), m_render->str.length() - 1, 0);
			temp[m_render->str.length() - 1] = '\0';
			m_render->str.clear();
			m_render->str.append(temp);
			delete temp;
		}		
		break;
	case KEY_SHIFT:
		break;
	case KEY_LEFT:
		m_render->m_leftcam = true;
		break;
	case KEY_RIGHT:
		m_render->m_rightcam = true;
		break;
	case KEY_UP:
		m_render->m_forwardcam = true;
		break;
	case KEY_DOWN:
		m_render->m_backcam = true;
		break;
	case KEY_ESCAPE:
		m_render->m_CloseApp = false;
		break;
	default:
		if (m_render->str.length()<m_render->str_max_len)
			m_render->str.append(&arg.wc, 1);
		break;
	}

	return false;
}

bool MyInput::KeyReleased(const KeyEvent &arg)
{
	switch (arg.code)
	{
	case KEY_LEFT:
		m_render->m_leftcam = false;
		break;
	case KEY_RIGHT:
		m_render->m_rightcam = false;
		break;
	case KEY_UP:
		m_render->m_forwardcam = false;
		break;
	case KEY_DOWN:
		m_render->m_backcam = false;
		break;
	default:
		break;
	}

	return false;
}

bool MyInput::MouseMove(const MouseEvent &arg)
{
	//printf("%d - %d\n", arg.x, arg.y);
	return false;
}