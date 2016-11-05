#include "BaseControl.h"

Control::Control(Dialog *pDialog)
{
	m_Type = DXUT_CONTROL_BUTTON;
	m_pDialog = pDialog;
	m_ID = 0;
	m_Index = 0;
	m_pUserData = NULL;

	m_bEnabled = true;
	m_bVisible = true;
	m_bMouseOver = false;
	m_bHasFocus = false;
	m_bIsDefault = false;

	m_x = m_y = 0;
	m_width = m_height = 0;

	ZeroMemory(&m_rcBoundingBox, sizeof(m_rcBoundingBox));
}

Control::~Control()
{
	for (int i = 0; i < m_Elements.GetSize(); ++i)
	{
		delete m_Elements[i];
	}
	m_Elements.RemoveAll();
}

void Control::Refresh()
{
	m_bMouseOver = false;
	m_bHasFocus = false;

	for (int i = 0; i < m_Elements.GetSize(); i++)
	{
		Element *pElement = m_Elements.GetAt(i);
		pElement->Refresh();
	}
}

void Control::SetTextColor(D3DCOLOR Color)
{
	Element *pElement = m_Elements.GetAt(0);
	
	if (pElement)
		pElement->FontColor.States[DXUT_STATE_NORMAL] = Color;
}

HRESULT Control::SetElement(UINT iElement, Element *pElement)
{
	HRESULT hr = S_OK;

	if (pElement == NULL)
		return E_INVALIDARG;

	// ”беждаюсь что в массиве есть место
	for (UINT i = m_Elements.GetSize(); i <= iElement; i++)
	{
		Element *pNewElement = new Element();
		if (pNewElement == NULL)
			return E_OUTOFMEMORY;

		hr = m_Elements.Add(pNewElement);
		if (FAILED(hr))
		{
			_DELETE(pNewElement);
			return hr;
		}
	}

	// ќбновл€ю данные
	Element *pCurElement = m_Elements.GetAt(iElement);
	*pCurElement = *pElement;

	return S_OK;
}

void Control::UpdateRects()
{
	SetRect(&m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height);
}