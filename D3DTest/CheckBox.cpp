#include "CheckBox.h"

CheckBox::CheckBox(Dialog * pDialog)
{
	m_Type = DXUT_CONTROL_CHECKBOX;
	m_pDialog = pDialog;

	m_bChecked = false;
}

bool CheckBox::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
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
						SetCheckedInternal(!m_bChecked, true);
					}
					return true;
			}
		}
	}

	return false;
}

bool CheckBox::HandleMouse(UINT uMsg, POINT pt/*, WPARAM wParam, LPARAM lParam*/)
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
				// Когда нажали внутри элемента
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

				// Нажатие кнопки
				if (ContainsPoint(pt))
					SetCheckedInternal(!m_bChecked, true);

				return true;
			}
			break;
		}
	}

	return false;
}

BOOL CheckBox::ContainsPoint(POINT pt)
{
	return (PtInRect(&m_rcBoundingBox, pt) || PtInRect(&m_rcButton, pt));
}

void CheckBox::UpdateRects()
{
	Button::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.right = m_rcButton.left + RectHeight(m_rcButton);

	m_rcText = m_rcBoundingBox;
	m_rcText.left += (int)(1.25f * RectWidth(m_rcButton));
}

void CheckBox::Render(float fElapsedTime)
{
	if (m_bVisible == false)
		return;
	DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

	if (m_bVisible == false)
		iState = DXUT_STATE_HIDDEN;
	else if (m_bEnabled == false)
		iState = DXUT_STATE_DISABLED;
	else if (m_bPressed)
		iState = DXUT_STATE_PRESSED;
	else if (m_bMouseOver)
		iState = DXUT_STATE_MOUSEOVER;
	else if (m_bHasFocus)
		iState = DXUT_STATE_FOCUS;

	Element *pElement = m_Elements.GetAt(0);

	float fBlendRate = (iState == DXUT_STATE_PRESSED) ? 0.0f : 0.8f;

	pElement->TextureColor.Blend(iState, fElapsedTime, fBlendRate);
	pElement->FontColor.Blend(iState, fElapsedTime, fBlendRate);

	m_pDialog->DrawSprite(pElement, &m_rcButton, DXUT_NEAR_BUTTON_DEPTH);
	m_pDialog->DrawText(m_strText, pElement, &m_rcText, false, -1, false);

	if (!m_bChecked)
		iState = DXUT_STATE_HIDDEN;

	pElement = m_Elements.GetAt(1);

	pElement->TextureColor.Blend(iState, fElapsedTime, fBlendRate);
	m_pDialog->DrawSprite(pElement, &m_rcButton, DXUT_FAR_BUTTON_DEPTH);
}

void CheckBox::SetCheckedInternal(bool bChecked, bool bFromInput)
{
	m_bChecked = bChecked;

	m_pDialog->SendEvent(EVENT_CHECKBOX_CHANGED, bFromInput, this);
}
