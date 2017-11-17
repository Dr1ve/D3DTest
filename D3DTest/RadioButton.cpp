#include "RadioButton.h"

RadioButton::RadioButton(Dialog * pDialog)
{
	m_Type = DXUT_CONTROL_RADIOBUTTON;
	m_pDialog = pDialog;
}

bool RadioButton::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!m_bEnabled || !m_bVisible)
		return false;

	switch (uMsg)
	{
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_SPACE:
					m_bPressed = true;
					return true;
			}
		}
		case WM_KEYUP:
		{
			switch (wParam)
			{
				case VK_SPACE:
					if (m_bPressed == true)
					{
						m_bPressed = false;

						m_pDialog->ClearRadioButtonGroup(m_nButtonGroup);
						m_bChecked = !m_bChecked;

						m_pDialog->SendEvent(EVENT_RADIOBUTTON_CHANGED, true, this);
					}
					return true;
			}
		}
	}
	return false;
}

bool RadioButton::HandleMouse(UINT uMsg, POINT pt/*, WPARAM wParam, LPARAM lParam*/)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	switch (uMsg)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if (ContainsPoint(pt))
			{
				m_bPressed = true;
				// SetCapture(DXUTGetHWND());

				if (!m_bHasFocus)
					m_pDialog->RequestFocus(this);

				return true;
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			if (m_bPressed)
			{
				m_bPressed = false;
				// ReleaseCapture();

				// Нажатие кнопки
				if (ContainsPoint(pt))
				{
					m_pDialog->ClearRadioButtonGroup(m_nButtonGroup);
					m_bChecked = !m_bChecked;

					m_pDialog->SendEvent(EVENT_RADIOBUTTON_CHANGED, true, this);
				}
				return true;
			}
			break;
		}
	}
	return false;
}

void RadioButton::SetCheckedInternal(bool bChecked, bool bClearGroup, bool bFromInput)
{
	if (bChecked && bClearGroup)
		m_pDialog->ClearRadioButtonGroup(m_nButtonGroup);

	m_bChecked = bChecked;
	m_pDialog->SendEvent(EVENT_RADIOBUTTON_CHANGED, bFromInput, this);
}
