#pragma once

#include "Misc.h"
#include "Button.h"
//#include "Dialog.h"

//-------------------------------------------------------------------
// Ёлемент управлени€ CheckBox
//-------------------------------------------------------------------
class CheckBox : public Button
{
public:
	CheckBox(Dialog *pDialog = NULL);

	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool HandleMouse(UINT uMsg, POINT pt/*, WPARAM wParam, LPARAM lParam*/);
	virtual void OnHotkey()
	{
		if (m_pDialog->IsKeyboardInputEnabled()) m_pDialog->RequestFocus(this);
		SetCheckedInternal(!m_bChecked, true);
	}

	virtual BOOL ContainsPoint(POINT pt);
	virtual void UpdateRects();

	virtual void Render(float fElapsedTime);

	bool GetChecked() { return m_bChecked; }
	void SetChecked(bool bChecked) { SetCheckedInternal(bChecked, false); }

protected:
	bool m_bChecked;
	RECT m_rcButton;
	RECT m_rcText;

	virtual void SetCheckedInternal(bool bChecked, bool bFromInput);
};