#pragma once

#include "Misc.h"
//#include "Dialog.h"
#include "Element.h"

class Dialog;

using namespace D3D11Framework;

//-------------------------------------------------------------------
// ������� ����� ��� ��������� ����������
//-------------------------------------------------------------------
class Control
{
public:
	bool m_bVisible; // ���� ��������� ��������
	bool m_bMouseOver; // ���� ��� ������ ��������� ��� ���������
	bool m_bHasFocus; // ���� ��� ������� ��������� � ������ �����
	bool m_bIsDefault; // ��������� ������� ����������

	// ���������� ������� ����� � ������ ��������
	int m_x, m_y;
	int m_width, m_height;
	
	// ���������� ����������
	Dialog *m_pDialog; // ������ �� ���������
	UINT m_Index; // ������ � ������ ����������

	CGrowableArray<Element*> m_Elements; // ��� �������� �� ������

	Control(Dialog *pDialog = NULL);
	virtual ~Control();

	virtual HRESULT OnInit() { return S_OK; }
	virtual void Refresh();
	virtual void Render(float fElapsedTime) {}

	// ��������� ����������
	virtual bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	virtual bool HandleMouse(UINT uMsg, POINT pt/*, WPARAM wParam, LPARAM lParam*/) { return false; }
	virtual bool CanHaveFocus() { return false; }
	virtual void OnFocusIn() { m_bHasFocus = true; }
	virtual void OnFocusOut() { m_bHasFocus = false; }
	virtual void OnMouseEnter() { m_bMouseOver = true; }
	virtual void OnMouseLeave() { m_bMouseOver = false; }
	virtual void OnHotkey() {}

	virtual BOOL ContainsPoint(POINT pt) { return PtInRect(&m_rcBoundingBox, pt); }
	virtual void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }
	virtual bool GetEnabled() { return m_bEnabled; }
	virtual void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	virtual bool GetVisible() { return m_bVisible; }

	UINT GetType() const { return m_Type; }
	int GetID() const { return m_ID; }
	void SetID(int ID) { m_ID = ID; }
	void SetLocation(int x, int y) { m_x = x; m_y = y; UpdateRects(); }
	void SetSize(int width, int height) { m_width = width; m_height = height; UpdateRects(); }
	void SetHotkey(UINT nHotkey) { m_nHotkey = nHotkey; }
	UINT GetHotkey() { return m_nHotkey; }
	void SetUserData(void *pUserData) { m_pUserData = pUserData; }
	void *GetUserData() const { return m_pUserData; }

	virtual void SetTextColor(D3DCOLOR Color);
	Element *GetElement(UINT iElement) { return m_Elements.GetAt(iElement); }
	HRESULT SetElement(UINT iElement, Element *pElement);

protected:
	int m_ID; // �� �����
	DXUT_CONTROL_TYPE m_Type; // ��� �������� ���������� ��������������� ���� ��� � ������������
	UINT m_nHotkey; // ����������� ��� ������ ��� ��������� ����� �������� ����������
	void *m_pUserData; // ������ � �������� ������������� �������������

	bool m_bEnabled; // ���� ��������/����������
	RECT m_rcBoundingBox; // ���� ��������� �������� ����������

	virtual void UpdateRects();
};