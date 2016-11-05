#pragma once

#include "D3D11_Framework.h"
#include <assert.h>

#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_SLIDER_VALUE_CHANGED_UP       0x0502

#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703
// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE 8

// Delay and repeat period when clicking on the scroll bar arrows
#define SCROLLBAR_ARROWCLICK_DELAY  0.33
#define SCROLLBAR_ARROWCLICK_REPEAT 0.05

#define DXUT_NEAR_BUTTON_DEPTH 0.6f
#define DXUT_FAR_BUTTON_DEPTH 0.8f

#define DXUT_MAX_GUI_SPRITES 500

enum DXUT_CONTROL_TYPE
{
	DXUT_CONTROL_BUTTON,
	DXUT_CONTROL_STATIC,
	DXUT_CONTROL_CHECKBOX,
	DXUT_CONTROL_RADIOBUTTON,
	DXUT_CONTROL_COMBOBOX,
	DXUT_CONTROL_SLIDER,
	DXUT_CONTROL_EDITBOX,
	DXUT_CONTROL_IMEEDITBOX,
	DXUT_CONTROL_LISTBOX,
	DXUT_CONTROL_SCROLLBAR,
};

struct DXUTTextureNode
{
	bool bFileSource; // Тру если загружается из файла, фолс если из ресурсов
	HMODULE hResourceModule;
	int nResourceID; // Ид ресурса. Если 0, то базовая строка ида используется и хранится в strFilename
	WCHAR strFilename[MAX_PATH];
	DWORD dwWidth;
	DWORD dwHeight;
	ID3D11Texture2D *pTexture11;
	ID3D11ShaderResourceView *pTexResView11;
};

//-------------------------------------------------------------------
// Расширяемый массив
//-------------------------------------------------------------------
template<typename TYPE> class CGrowableArray
{
protected:
	TYPE *m_pData; // Фактический массив с данными
	int m_nSize; // Количество элементов
	int m_nMaxSize; // Максимальное выделение

	HRESULT SetSizeInternal(int nNewMaxSize);

public:
	CGrowableArray() { m_pData = NULL; m_nSize = 0; m_nMaxSize = 0; }
	CGrowableArray(const CGrowableArray<TYPE> &a) { for (int i = 0; i < a.m_nSize; i++) Add(a.m_pData[i]); }
	~CGrowableArray() { RemoveAll(); }

	const TYPE& operator[](int nIndex) const { return GetAt(nIndex); }
	TYPE& operator[](int nIndex) { return GetAt(nIndex); }

	CGrowableArray& operator=(const CGrowableArray<TYPE> &a) { if (this == &a) return *this; RemoveAll(); for (int i = 0; i < a.m_nSize; i++) Add(a.m_pData[i]); return *this; }

	HRESULT SetSize(int nNewMaxSize);
	HRESULT Add(const TYPE &value);
	HRESULT Insert(int nIndex, const TYPE &value);
	HRESULT SetAt(int nIndex, const TYPE &value);
	TYPE &GetAt(int nIndex) const { assert(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }
	int GetSize() const { return m_nSize; }
	TYPE *GetData() { return m_pData; }
	bool Constains(const TYPE &value) { return (-1 != IndexOf(value)); }

	int IndexOf(const TYPE &value) { return (m_nSize > 0) ? IndexOf(value, 0, m_nSize) : -1; }
	int IndexOf(const TYPE &value, int iStart) { return IndexOf(value, iStart, m_nSize - iStart); }
	int IndexOf(const TYPE &value, int iStart, int nNumElement);

	int LastIndexOf(const TYPE &value) { return (m_nSize > 0) ? LastIndexOf(value, m_nSize - 1, m_nSize) : -1; }
	int LastIndexOf(const TYPE &value, int nIndex) { return LastIndexOf(value, nIndex, nIndex + 1); }
	int LastIndexOf(const TYPE &value, int nIndex, int nNumElements);

	HRESULT Remove(int nIndex);
	void RemoveAll() { SetSize(0); }
	void Reset() { m_nSize = 0; }
};

#if defined(PROFILE) || defined(DEBUG)
inline void DXUT_SetDebugName(IDirect3DResource9* pObj, const CHAR* pstrName)
{
	if (pObj)
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, pstrName, lstrlenA(pstrName), 0);
}
inline void DXUT_SetDebugName(IDXGIObject* pObj, const CHAR* pstrName)
{
	if (pObj)
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pstrName), pstrName);
}
inline void DXUT_SetDebugName(ID3D10Device* pObj, const CHAR* pstrName)
{
	if (pObj)
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pstrName), pstrName);
}
inline void DXUT_SetDebugName(ID3D10DeviceChild* pObj, const CHAR* pstrName)
{
	if (pObj)
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pstrName), pstrName);
}
inline void DXUT_SetDebugName(ID3D11Device* pObj, const CHAR* pstrName)
{
	if (pObj)
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pstrName), pstrName);
}
inline void DXUT_SetDebugName(ID3D11DeviceChild* pObj, const CHAR* pstrName)
{
	if (pObj)
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pstrName), pstrName);
}
#else
#define DXUT_SetDebugName( pObj, pstrName )
#endif

DXGI_FORMAT MAKE_SRGB(DXGI_FORMAT format);