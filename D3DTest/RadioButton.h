#pragma once

#include "Misc.h"
#include "CheckBox.h"

//-------------------------------------------------------------------
// Ёлемент управлени€ CheckBox
//-------------------------------------------------------------------
class RadioButton : public CheckBox
{
public:
	RadioButton(Dialog *pDialog = NULL);

	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool HandleMouse(UINT uMsg, POINT pt/*, WPARAM wParam, LPARAM lParam*/);
	virtual void OnHotkey()
	{
		if (m_pDialog->IsKeyboardInputEnabled()) m_pDialog->RequestFocus(this);
		SetCheckedInternal(true, true, true);
	}

	void SetChecked(bool bChecked, bool bClearGroup = true) { SetCheckedInternal(bChecked, bClearGroup, false); }
	void SetButtonGroup(UINT nButtonGroup) { m_nButtonGroup = nButtonGroup; }
	UINT GetButtonGroup() { return m_nButtonGroup; }

protected:
	UINT m_nButtonGroup;
	virtual void SetCheckedInternal(bool bChecked, bool bClearGroup, bool bFromInput);
};