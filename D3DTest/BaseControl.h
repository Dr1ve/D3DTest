#pragma once

#include "Misc.h"
//#include "Dialog.h"
#include "Element.h"

class Dialog;

using namespace D3D11Framework;

//-------------------------------------------------------------------
// Базовый класс для элементов управления
//-------------------------------------------------------------------
class Control
{
public:
	bool m_bVisible; // Флаг видимости элемента
	bool m_bMouseOver; // Флаг что курсор находится над элементом
	bool m_bHasFocus; // Флаг что элемент находится в фокусе ввода
	bool m_bIsDefault; // Дефолтный элемент управления

	// Переменные позиции длины и высоты элемента
	int m_x, m_y;
	int m_width, m_height;
	
	// Переменные контейнера
	Dialog *m_pDialog; // Ссылка на контейнер
	UINT m_Index; // Индекс в списке управления

	CGrowableArray<Element*> m_Elements; // Все элементы на экране

	Control(Dialog *pDialog = NULL);
	virtual ~Control();

	virtual HRESULT OnInit() { return S_OK; }
	virtual void Refresh();
	virtual void Render(float fElapsedTime) {}

	// Сообщения приложения
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
	int m_ID; // Ид номер
	DXUT_CONTROL_TYPE m_Type; // Тип элемента управления устанавливается один раз в конструкторе
	UINT m_nHotkey; // Виртуальный код кнопки для обработки этого элемента управления
	void *m_pUserData; // Данные о элементе установленные пользователем

	bool m_bEnabled; // Флаг активный/неактивный
	RECT m_rcBoundingBox; // Зона активации элемента управления

	virtual void UpdateRects();
};