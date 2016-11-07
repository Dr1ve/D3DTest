#include "Static.h"
#include "Dialog.h"

Static::Static(Dialog *pDialog)
{
	m_Type = DXUT_CONTROL_STATIC;
	m_pDialog = pDialog;

	ZeroMemory(&m_strText, sizeof(m_strText));
	
	for (int i = 0; i < m_Elements.GetSize(); i++)
	{
		Element *pElement = m_Elements.GetAt(i);
		_DELETE(pElement);
	}

	m_Elements.RemoveAll();
}

void Static::Render(float fElapsedTime)
{
	if (m_bVisible == false)
		return;

	DXUT_CONTROL_STATE iState = DXUT_STATE_NORMAL;

	if (m_bEnabled == false)
		iState = DXUT_STATE_DISABLED;

	Element *pElement = m_Elements.GetAt(0);

	pElement->FontColor.Blend(iState, fElapsedTime);

	m_pDialog->DrawText(m_strText, pElement, &m_rcBoundingBox, false, -1, false);
}

HRESULT Static::GetTextCopy(LPWSTR strDest, UINT bufferCount)
{
	if (strDest == NULL || bufferCount == 0)
	{
		return E_INVALIDARG;
	}

	wcscpy_s(strDest, bufferCount, m_strText);

	return S_OK;
}

HRESULT Static::SetText(LPCWSTR strText)
{
	if (strText == NULL)
	{
		m_strText[0] = 0;
		return S_OK;
	}

	wcscpy_s(m_strText, MAX_PATH, strText);
	return S_OK;
}
