#pragma once

#include "Dialog.h"
#include "BaseControl.h"

//-------------------------------------------------------------------
// Элемент управления Static
//-------------------------------------------------------------------
class Static : public Control
{
public:
	Static(Dialog *pDialog = NULL);

	virtual void Render(float fElapsedTime);
	virtual BOOL ContainsPoint(POINT pt) { return false; }

	HRESULT GetTextCopy(__out_ecount(bufferCount) LPWSTR strDest, UINT bufferCount);
	LPCWSTR GetText() { return m_strText; }
	HRESULT SetText(LPCWSTR strText);

protected:
	WCHAR m_strText[MAX_PATH]; // Текст в окне
};