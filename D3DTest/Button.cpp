#include "Button.h"

Button::Button(Dialog * pDialog)
{
	m_Type = DXUT_CONTROL_BUTTON;
	m_pDialog = pDialog;

	m_bPressed = false;
	m_nHotkey = 0;
}

bool Button::HandleMouse(UINT uMsg, POINT pt/*, WPARAM wParam, LPARAM lParam*/)
{
	if (!m_bEnabled || !m_bVisible)
		return false;

	switch (uMsg)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if (ContainsPoint(pt))
			{
				// Pressed while inside the control
				m_bPressed = true;
				//SetCapture(DXUTGetHWND());

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
				//ReleaseCapture();

				if (!m_pDialog->m_bKeyboardInput)
					m_pDialog->ClearFocus();

				// Button click
				if (ContainsPoint(pt))
					m_pDialog->SendEvent(EVENT_BUTTON_CLICKED, true, this);

				return true;
			}

			break;
		}
	}

	return false;
}

void Button::Render(float fElapsedTime)
{
	if (m_bVisible == false)
		return;

	int nOffsetX = 0;
	int nOffsetY = 0;

	DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

	if (m_bVisible == false)
	{
		iState = DXUT_STATE_HIDDEN;
	}
	else if (m_bEnabled == false)
	{
		iState = DXUT_STATE_DISABLED;
	}
	else if (m_bPressed)
	{
		iState = DXUT_STATE_PRESSED;

		nOffsetX = 1;
		nOffsetY = 2;
	}
	else if (m_bMouseOver)
	{
		iState = DXUT_STATE_MOUSEOVER;

		nOffsetX = -1;
		nOffsetY = -2;
	}
	else if (m_bHasFocus)
	{
		iState = DXUT_STATE_FOCUS;
	}

	// Background fill layer
	Element* pElement = m_Elements.GetAt(0);

	float fBlendRate = (iState == DXUT_STATE_PRESSED) ? 0.0f : 0.8f;

	RECT rcWindow = m_rcBoundingBox;
	OffsetRect(&rcWindow, nOffsetX, nOffsetY);


	// Blend current color
	pElement->TextureColor.Blend(iState, fElapsedTime, fBlendRate);
	pElement->FontColor.Blend(iState, fElapsedTime, fBlendRate);

	m_pDialog->DrawSprite(pElement, &rcWindow, DXUT_FAR_BUTTON_DEPTH);
	m_pDialog->DrawText(m_strText, pElement, &rcWindow, false, -1, true);

	// Main button
	pElement = m_Elements.GetAt(1);

	// Blend current color
	pElement->TextureColor.Blend(iState, fElapsedTime, fBlendRate);
	pElement->FontColor.Blend(iState, fElapsedTime, fBlendRate);

	m_pDialog->DrawSprite(pElement, &rcWindow, DXUT_NEAR_BUTTON_DEPTH);
	m_pDialog->DrawText(m_strText, pElement, &rcWindow, false, -1, true);
}

bool Button::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
					m_pDialog->SendEvent(EVENT_BUTTON_CLICKED, true, this);
				}
				return true;
			}
		}
	}

	return false;
}
