#pragma once

#include "Misc.h"
#include "BaseControl.h"
#include "Element.h"
#include "DialogResourceManager.h"
#include "Static.h"
#include "Button.h"

//class Control;
class Static;
class Button;

typedef VOID(CALLBACK*PCALLBACKDXUTGUIEVENT)(UINT nEvent, int nControlID, Control* pControl, void* pUserContext);

HRESULT InitFont11(ID3D11Device *pd3d11Device, ID3D11InputLayout *pInputLayout);
void EndFont11();

struct ElementHolder
{
	UINT nControlType;
	UINT iElement;

	Element Element;
};

D3DCOLORVALUE D3DCOLOR_TO_D3DCOLORVALUE(D3DCOLOR c)
{
	D3DCOLORVALUE cv =
	{
		((c >> 16) & 0xFF) / 255.0f,
		((c >> 8) & 0xFF) / 255.0f,
		(c & 0xFF) / 255.0f,
		((c >> 24) & 0xFF) / 255.0f
	};
	return cv;
}

//-------------------------------------------------------------------
// Все элементы упавления должны быть связаны с этим классом
// Он обрабатывает ввод и отрисовку на экран
//-------------------------------------------------------------------
class Dialog
{
	friend class DialogResourceManager;

public:
	bool m_bNonUserEvents;
	bool m_bKeyboardInput;
	bool m_bMouseInput;

	Dialog();
	~Dialog();

	void Init(DialogResourceManager *pManager, bool bRegisterDialog = true);
	void Init(DialogResourceManager *pManager, bool bRegisterDialog, LPCWSTR pszControlTextureFilename);
	void Init(DialogResourceManager *pManager, bool bRegisterDialog, LPCWSTR szControlTextureResourceName, HMODULE hControlTextureResourceModule);

	// Сообщения окна
	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Создание элемента управления
	HRESULT AddStatic(int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault = false, Static **ppCreated = NULL);
	HRESULT AddButton(int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey = 0, bool bIsDefault = false, Button **ppCreated = NULL);
	// Еще элементы управления
	HRESULT AddControl(Control *pControl);
	HRESULT InitControl(Control *pControl);

	// Поиск элемента управления
	Static *GetStatic(int ID) { return (Static*)GetControl(ID, DXUT_CONTROL_STATIC); }
	Button *GetButton(int ID) { return (Button*)GetControl(ID, DXUT_CONTROL_BUTTON); }

	Control *GetControl(int ID);
	Control *GetControl(int ID, UINT nControlType);
	Control *GetControlAtPoint(POINT pt);

	bool GetControlEnabled(int ID);
	void SetControlEnabled(int ID, bool bEnabled);

	//void ClearRadioButtonGroup(UINT nGroup);
	//void ClearComboBox(int ID);

	// Доступ к стандартным элементам на дисплее для добавления новых элементов
	HRESULT SetDefaultElement(UINT nControlType, UINT iElement, Element *pElement);
	Element *GetDefaultElement(UINT nControlType, UINT iElement);

	// Методы вызова элементов
	void SendEvent(UINT nEvent, bool bTriggeredByUser, Control *pControl);
	void RequestFocus(Control *pControl);

	// Помошник отрисовки
	HRESULT DrawSprite(Element *pElement, RECT *prcDest, float fDepth);
	HRESULT DrawSprite11(Element *pElement, RECT *prcDest, float fDepth);
	HRESULT DrawText(LPCWSTR strText, Element *pElement, RECT *prcDest, bool bShadow = false, int nCount = -1, bool bCenter = false);
	HRESULT DrawText11(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3d11DeviceContext,
		LPCWSTR strText, Element *pElement, RECT *prcDest, bool bShadow = false,
		int nCount = -1, bool bCenter = false);

	// Атрибуты
	bool GetVisible() { return m_bVisible; }
	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	bool GetMinimized() { return m_bMinimized; }
	void SetMinimized(bool bMinimized) { m_bMinimized = bMinimized; }
	void SetBackgroudColors(D3DCOLOR colorAllCorners) { SetBackgroudColors(colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners); }
	void SetBackgroudColors(D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight);
	void EnableCaption(bool bEnable) { m_bCaption = bEnable; }
	int GetCaptionHeight() const { return m_nCaptionHeight; }
	void SetCaptionHeight(int nHeight) { m_nCaptionHeight = nHeight; }
	void SetCaptionText(const WCHAR *pwszText) { wcscpy_s(m_wszCaption, sizeof(m_wszCaption) / sizeof(m_wszCaption[0]), pwszText); }
	void GetLocation(POINT &pt) const { pt.x = m_x; pt.y = m_y; }
	void SetLocation(int x, int y) { m_x = x; m_y = y; }
	void SetSize(int width, int height) { m_width = width; m_height = height; }
	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

	static void WINAPI SetRefreshTime(float fTime) { s_fTimeRefresh = fTime; }

	static Control* WINAPI GetNextControl(Control *pControl);
	static Control* WINAPI GetPrevControl(Control *pControl);

	void RemoveControl(int ID);
	void RemoveAllControls();

	void SetCallback(PCALLBACKDXUTGUIEVENT pCallback, void *pUserContext = NULL);
	void EnableNonUserEvents(bool bEnable) { m_bNonUserEvents = bEnable; }
	void EnableKeyboardInput(bool bEnable) { m_bKeyboardInput = bEnable; }
	void EnableMouseInput(bool bEnable) { m_bMouseInput = bEnable; }
	bool IsKeyboardInputEnabled() const { return m_bKeyboardInput; }

	void Refresh();
	HRESULT OnRender(float fElapsedTime);

	// Общий доступ к ресурсам. Индексирование шрифтов и текстур для всех элементов
	//HRESULT SetFont(UINT index, LPCWSTR strFaceName, LONG height, LONG weight);
	//DXUTFontNode *GetFont(UINT index);

	HRESULT SetTexture(UINT index, LPCWSTR strFilename);
	HRESULT SetTexture(UINT index, LPCWSTR strResoucreName, HMODULE hResourceModule);
	DXUTTextureNode *GetTexture(UINT index);

	DialogResourceManager *GetManager() { return m_pManager; }

	static void WINAPI ClearFocus();
	void FocusDefaultControl();

private:
	int m_nDefaultControlID;

	static double s_fTimeRefresh;
	double m_fTimeLastRefresh;

	static Control *s_pControlFocus; // Элемент в фокусе
	static Control *s_pControlPressed; // Нажатый элемент

	Control *m_pControlMouseOver; // Элемент под курсором

	bool m_bVisible;
	bool m_bCaption;
	bool m_bMinimized;
	bool m_bDrag;
	WCHAR m_wszCaption[256];

	int m_x, m_y;
	int m_width, m_height;
	int m_nCaptionHeight;

	D3DCOLOR m_colorTopLeft;
	D3DCOLOR m_colorTopRight;
	D3DCOLOR m_colorBottomLeft;
	D3DCOLOR m_colorBottomRight;

	DialogResourceManager *m_pManager;
	PCALLBACKDXUTGUIEVENT m_pCallbackEvent;
	void *m_pCallbackEventUserContext;

	CGrowableArray<int> m_Textures; // Индексы буфера текстур
	CGrowableArray<int> m_Fonts; // Индексы буфера шрифтов

	CGrowableArray<Control*> m_Controls;
	CGrowableArray<ElementHolder*> m_DefaultElements;

	Element m_CapElement;

	Dialog *m_pNextDialog;
	Dialog *m_pPrevDialog;

	HRESULT OnRender11(float fElapsedTime);

	// Инициализация стандартных элементов
	void InitDefaultElements();

	// Сообщения окна
	void OnMouseMove(POINT pt);
	void OnMouseUp(POINT pt);

	void SetNextDialog(Dialog *pNextDialog);

	// События элементов
	bool OnCycleFocus(bool bForward);
};