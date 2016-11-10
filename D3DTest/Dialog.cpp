//#include "Dialog.h"
#include "Button.h"

void DrawText11DXUT(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3d11DeviceContext,
	LPCWSTR strText, RECT rcScreen, D3DXCOLOR vFontColor,
	float fBBWidth, float fBBHeight, bool bCenter);
void EndText11(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3d11DeviceContext);
void BeginText11();

double Dialog::s_fTimeRefresh = 0.0f;
Control *Dialog::s_pControlFocus = NULL; // Элемент в фокусе
Control *Dialog::s_pControlPressed = NULL; // Нажатый элемент

Dialog::Dialog()
{
	m_x = m_y = 0;
	m_width = m_height = 0;

	m_pManager = NULL;
	m_bVisible = true;
	m_bCaption = false;
	m_bMinimized = false;
	m_bDrag = false;
	m_wszCaption[0] = L'\0';
	m_nCaptionHeight = 18;

	m_colorTopLeft = 0;
	m_colorTopRight = 0;
	m_colorBottomLeft = 0;
	m_colorBottomRight = 0;

	m_pCallbackEvent = NULL;
	m_pCallbackEventUserContext = NULL;

	m_fTimeLastRefresh = 0;

	m_pControlMouseOver = NULL;

	m_pNextDialog = this;
	m_pPrevDialog = this;

	m_nDefaultControlID = 0xffff;
	m_bNonUserEvents = false;
	m_bKeyboardInput = false;
	m_bMouseInput = true;
}

Dialog::~Dialog()
{
	RemoveAllControls();

	m_Fonts.RemoveAll();
	m_Textures.RemoveAll();

	for (int i = 0; i < m_DefaultElements.GetSize(); i++)
	{
		ElementHolder *pElementHolder = m_DefaultElements.GetAt(i);
		_DELETE(pElementHolder);
	}

	m_DefaultElements.RemoveAll();
}

HRESULT Dialog::DrawSprite(Element * pElement, RECT * prcDest, float fDepth)
{
	return DrawSprite11(pElement, prcDest, fDepth);
}

HRESULT Dialog::DrawSprite11(Element * pElement, RECT * prcDest, float fDepth)
{
	// Выходим если полностью прозрачная текстура
	if (pElement->TextureColor.Current.a == 0)
		return S_OK;

	RECT rcTexture = pElement->rcTexture;

	RECT rcScreen = *prcDest;
	OffsetRect(&rcScreen, m_x, m_y);

	// Если есть заголовок смещаю на высоту заголовка
	if (m_bCaption)
		OffsetRect(&rcScreen, 0, m_nCaptionHeight);

	DXUTTextureNode* pTextureNode = GetTexture(pElement->iTexture);
	if (pTextureNode == NULL)
		return E_FAIL;

	float fBBWidth = (float)m_pManager->m_nBackBufferWidth;
	float fBBHeight = (float)m_pManager->m_nBackBufferHeight;
	float fTexWidth = (float)pTextureNode->dwWidth;
	float fTexHeight = (float)pTextureNode->dwHeight;

	float fRectLeft = rcScreen.left / fBBWidth;
	float fRectTop = 1.0f - rcScreen.top / fBBHeight;
	float fRectRight = rcScreen.right / fBBWidth;
	float fRectBottom = 1.0f - rcScreen.bottom / fBBHeight;

	fRectLeft = fRectLeft * 2.0f - 1.0f;
	fRectTop = fRectTop * 2.0f - 1.0f;
	fRectRight = fRectRight * 2.0f - 1.0f;
	fRectBottom = fRectBottom * 2.0f - 1.0f;

	float fTexLeft = rcTexture.left / fTexWidth;
	float fTexTop = rcTexture.top / fTexHeight;
	float fTexRight = rcTexture.right / fTexWidth;
	float fTexBottom = rcTexture.bottom / fTexHeight;

	// Добавляю 6 спрайт вершин
	DXUTSpriteVertex SpriteVertex;

	// Первая тройка
	SpriteVertex.vPos = D3DXVECTOR3(fRectLeft, fRectTop, fDepth);
	SpriteVertex.vTex = D3DXVECTOR2(fTexLeft, fTexTop);
	SpriteVertex.vColor = pElement->TextureColor.Current;
	m_pManager->m_SpriteVertices.Add(SpriteVertex);

	SpriteVertex.vPos = D3DXVECTOR3(fRectRight, fRectTop, fDepth);
	SpriteVertex.vTex = D3DXVECTOR2(fTexRight, fTexTop);
	SpriteVertex.vColor = pElement->TextureColor.Current;
	m_pManager->m_SpriteVertices.Add(SpriteVertex);

	SpriteVertex.vPos = D3DXVECTOR3(fRectLeft, fRectBottom, fDepth);
	SpriteVertex.vTex = D3DXVECTOR2(fTexLeft, fTexBottom);
	SpriteVertex.vColor = pElement->TextureColor.Current;
	m_pManager->m_SpriteVertices.Add(SpriteVertex);

	// Вторая тройка
	SpriteVertex.vPos = D3DXVECTOR3(fRectRight, fRectTop, fDepth);
	SpriteVertex.vTex = D3DXVECTOR2(fTexRight, fTexTop);
	SpriteVertex.vColor = pElement->TextureColor.Current;
	m_pManager->m_SpriteVertices.Add(SpriteVertex);

	SpriteVertex.vPos = D3DXVECTOR3(fRectRight, fRectBottom, fDepth);
	SpriteVertex.vTex = D3DXVECTOR2(fTexRight, fTexBottom);
	SpriteVertex.vColor = pElement->TextureColor.Current;
	m_pManager->m_SpriteVertices.Add(SpriteVertex);

	SpriteVertex.vPos = D3DXVECTOR3(fRectLeft, fRectBottom, fDepth);
	SpriteVertex.vTex = D3DXVECTOR2(fTexLeft, fTexBottom);
	SpriteVertex.vColor = pElement->TextureColor.Current;
	m_pManager->m_SpriteVertices.Add(SpriteVertex);

	m_pManager->EndSprites11(m_pManager->GetD3D11Device(),m_pManager->GetD3D11DeviceContext());

	return S_OK;
}

HRESULT Dialog::DrawText(LPCWSTR strText, Element * pElement, RECT * prcDest, bool bShadow, int nCount, bool bCenter)
{
	return DrawText11(m_pManager->GetD3D11Device(), m_pManager->GetD3D11DeviceContext(), strText, pElement, prcDest, bShadow, nCount, bCenter);
}

HRESULT Dialog::DrawText11(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pd3d11DeviceContext, LPCWSTR strText, Element * pElement, RECT * prcDest, bool bShadow, int nCount, bool bCenter)
{
	// Нет необходимости делать полностью прозрачные слои
	if (pElement->FontColor.Current.a == 0)
		return S_OK;

	RECT rcScreen = *prcDest;
	OffsetRect(&rcScreen, m_x, m_y);

	// Если есть заголовок то смещаю позицию Y на высоту этого заголовка
	if (m_bCaption)
		OffsetRect(&rcScreen, 0, m_nCaptionHeight);

	float fBBWidth = (float)m_pManager->m_nBackBufferWidth;
	float fBBHeight = (float)m_pManager->m_nBackBufferHeight;

	if (bShadow)
	{
		RECT rcShadow = rcScreen;
		OffsetRect(&rcShadow, 1, 1);

		D3DXCOLOR vShadowColor(0, 0, 0, 1.0f);
		DrawText11DXUT(pd3dDevice, pd3d11DeviceContext, strText, rcShadow, vShadowColor, fBBWidth, fBBHeight, bCenter);
	}

	D3DXCOLOR vFontColor(pElement->FontColor.Current.r, pElement->FontColor.Current.g, pElement->FontColor.Current.b, pElement->FontColor.Current.a);
	DrawText11DXUT(pd3dDevice, pd3d11DeviceContext, strText, rcScreen, vFontColor, fBBWidth, fBBHeight, bCenter);

	return S_OK;
}

void Dialog::SetBackgroudColors(D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight)
{
	m_colorTopLeft = colorTopLeft;
	m_colorTopRight = colorTopRight;
	m_colorBottomLeft = colorBottomLeft;
	m_colorBottomRight = colorBottomRight;
}

Control * Dialog::GetNextControl(Control * pControl)
{
	int index = pControl->m_Index + 1;
	Dialog *pDialog = pControl->m_pDialog;

	while (index >= (int)pDialog->m_Controls.GetSize())
	{
		pDialog = pDialog->m_pNextDialog;
		index = 0;
	}

	return pDialog->m_Controls.GetAt(index);
}

Control * Dialog::GetPrevControl(Control * pControl)
{
	int index = pControl->m_Index - 1;

	Dialog *pDialog = pControl->m_pDialog;

	while (index < 0)
	{
		pDialog = pDialog->m_pPrevDialog;
		if (pDialog == NULL)
			pDialog = pControl->m_pDialog;

		index = pDialog->m_Controls.GetSize() - 1;
	}

	return pDialog->m_Controls.GetAt(index);
}

void Dialog::RemoveControl(int ID)
{
	for (int i = 0; i < m_Controls.GetSize(); i++)
	{
		Control *pControl = m_Controls.GetAt(i);
		if (pControl->GetID() == ID)
		{
			// Очищаю первый фокус
			ClearFocus();

			if (s_pControlFocus == pControl)
				s_pControlFocus = NULL;
			if (s_pControlPressed == pControl)
				s_pControlPressed = NULL;
			if (m_pControlMouseOver == pControl)
				m_pControlMouseOver = NULL;

			_DELETE(pControl);
			m_Controls.Remove(i);
			return;
		}
	}
}

void Dialog::RemoveAllControls()
{
	if (s_pControlFocus && s_pControlFocus->m_pDialog == this)
		s_pControlFocus = NULL;
	if (s_pControlPressed && s_pControlPressed->m_pDialog == this)
		s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;
	
	for (int i = 0; i < m_Controls.GetSize(); i++)
	{
		Control *pControl = m_Controls.GetAt(i);
		_DELETE(pControl);
	}

	m_Controls.RemoveAll();
}

void Dialog::SetCallback(PCALLBACKDXUTGUIEVENT pCallback, void * pUserContext)
{
	assert(m_pManager != NULL && L"To fix call Dialog::Init() first. See comments for details.");

	m_pCallbackEvent = pCallback;
	m_pCallbackEventUserContext = pUserContext;
}

void Dialog::Refresh()
{
	if (s_pControlFocus)
		s_pControlFocus->OnFocusOut();

	if (m_pControlMouseOver)
		m_pControlMouseOver->OnMouseLeave();

	s_pControlFocus = NULL;
	s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;

	for (int i = 0; i < m_Controls.GetSize(); i++)
	{
		Control *pControl = m_Controls.GetAt(i);
		pControl->Refresh();
	}

	if (m_bKeyboardInput)
		FocusDefaultControl();
}

HRESULT Dialog::OnRender(float fElapsedTime)
{
	return OnRender11(fElapsedTime);
}

HRESULT Dialog::SetTexture(UINT index, LPCWSTR strFilename)
{
	assert(m_pManager != NULL && L"To fix this, call Dialog::Init() first.  See comments for details.");

	for (UINT i = m_Textures.GetSize(); i <= index; i++)
	{
		m_Textures.Add(-1);
	}

	int iTexture = m_pManager->AddTexture(strFilename);

	m_Textures.SetAt(index, iTexture);

	return S_OK;
}

HRESULT Dialog::SetTexture(UINT index, LPCWSTR strResoucreName, HMODULE hResourceModule)
{
	assert(m_pManager != NULL && L"To fix this, call Dialog::Init() first.  See comments for details.");

	for (UINT i = m_Textures.GetSize(); i <= index; i++)
	{
		m_Textures.Add(-1);
	}

	int iTexture = m_pManager->AddTexture(strResoucreName, hResourceModule);

	m_Textures.SetAt(index, iTexture);

	return S_OK;
}

DXUTTextureNode * Dialog::GetTexture(UINT index)
{
	if (NULL == m_pManager)
		return NULL;
	return m_pManager->GetTextureNode(m_Textures.GetAt(index));
}

void Dialog::ClearFocus()
{
	if (s_pControlFocus)
	{
		s_pControlFocus->OnFocusOut();
		s_pControlFocus = NULL;
	}

	ReleaseCapture();
}

void Dialog::FocusDefaultControl()
{
	for (int i = 0; i < m_Controls.GetSize(); i++)
	{
		Control *pControl = m_Controls.GetAt(i);
		if (pControl->m_bIsDefault)
		{
			ClearFocus();

			s_pControlFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return;
		}
	}
}

HRESULT Dialog::OnRender11(float fElapsedTime)
{
	assert(m_pManager->GetD3D11Device() && L"To fix hook up DialogResourceManager to device callbacks.  See comments for details");

	// Непонятно что тут с обновлением пока что закоменчу
	// Смотрим надо ли обновлить диалог
	/*if (m_fTimeLastRefresh < s_fTimeRefresh)
	{
		m_fTimeLastRefresh = DXUTGetTime();
		Refresh();
	}*/

	// Выходим если dialog невидимый либо нет подписи и свернут
	if (!m_bVisible || (m_bMinimized && !m_bCaption))
		return S_OK;

	ID3D11Device *pd3dDevice = m_pManager->GetD3D11Device();
	ID3D11DeviceContext *pd3dDeviceContext = m_pManager->GetD3D11DeviceContext();

	// Устанавливаю блокировку всем элементам, и восстанавливаю после завершения рисования
	m_pManager->StoreD3D11State(pd3dDeviceContext);

	BOOL bBackgroundIsVisible = (m_colorTopLeft | m_colorTopRight | m_colorBottomRight | m_colorBottomLeft) & 0xff000000;
	if (!m_bMinimized && bBackgroundIsVisible)
	{
		float Left, Right, Top, Bottom;
		Left = m_x * 2.0f / m_pManager->m_nBackBufferWidth - 1.0f;
		Right = (m_x + m_width) * 2.0f / m_pManager->m_nBackBufferWidth - 1.0f;
		Top = 1.0f - m_y * 2.0f / m_pManager->m_nBackBufferHeight;
		Bottom = 1.0f - (m_y + m_height) * 2.0f / m_pManager->m_nBackBufferHeight;

		DXUT_SCREEN_VERTEX_10 vertices[4] =
		{
			Left,  Top,    0.5f, D3DCOLOR_TO_D3DCOLORVALUE(m_colorTopLeft), 0.0f, 0.0f,
			Right, Top,    0.5f, D3DCOLOR_TO_D3DCOLORVALUE(m_colorTopRight), 1.0f, 0.0f,
			Left,  Bottom, 0.5f, D3DCOLOR_TO_D3DCOLORVALUE(m_colorBottomLeft), 0.0f, 1.0f,
			Right, Bottom, 0.5f, D3DCOLOR_TO_D3DCOLORVALUE(m_colorBottomRight), 1.0f, 1.0f,
		};

		D3D11_MAPPED_SUBRESOURCE MappedData;
		if (SUCCEEDED(pd3dDeviceContext->Map(m_pManager->m_pVBScreenQuad11, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedData)))
		{
			CopyMemory(MappedData.pData, vertices, sizeof(vertices));
			pd3dDeviceContext->Unmap(m_pManager->m_pVBScreenQuad11, 0);
		}

		UINT stride = sizeof(DXUT_SCREEN_VERTEX_10);
		UINT offset = 0;
		pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pManager->m_pVBScreenQuad11, &stride, &offset);
		pd3dDeviceContext->IASetInputLayout(m_pManager->m_pInputLayout11);
		pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		m_pManager->ApplyRenderUIUntex11(pd3dDeviceContext);
		pd3dDeviceContext->Draw(4, 0);
	}

	DXUTTextureNode *pTextureNode = GetTexture(0);
	pd3dDeviceContext->PSSetShaderResources(0, 1, &pTextureNode->pTexResView11);

	m_pManager->BeginSprites11();
	BeginText11();

	m_pManager->ApplyRenderUI11(pd3dDeviceContext);

	if (m_bCaption)
	{
		RECT rc = { 0,-m_nCaptionHeight,m_width,0 };
		DrawSprite11(&m_CapElement, &rc, 0.99f);
		rc.left += 5;
		WCHAR wszOutput[256];
		wcscpy_s(wszOutput, 256, m_wszCaption);
		if (m_bMinimized)
			wcscat_s(wszOutput, 256, L" (Minimized)");
		DrawText11(pd3dDevice, pd3dDeviceContext, wszOutput, &m_CapElement, &rc, true);
	}

	if (!m_bMinimized)
	{
		for (int i = 0; i < m_Controls.GetSize(); i++)
		{
			Control *pControl = m_Controls.GetAt(i);

			if (pControl == s_pControlFocus)
				continue;

			pControl->Render(fElapsedTime);
		}

		if (s_pControlFocus != NULL && s_pControlFocus->m_pDialog == this)
			s_pControlFocus->Render(fElapsedTime);
	}

	if (m_bCaption)
	{
		m_pManager->EndSprites11(pd3dDevice, pd3dDeviceContext);
		EndText11(pd3dDevice, pd3dDeviceContext);
	}
	m_pManager->RestoreD3D11State(pd3dDeviceContext);

	return S_OK;
}

void Dialog::InitDefaultElements()
{
	//SetFont(0, L"Arial", 14, FW_NORMAL);

	Element Element;
	RECT rcTexture;

	//-------------------------------------
	// Element for the caption
	//-------------------------------------
	m_CapElement.SetFont(0);
	SetRect(&rcTexture, 17, 269, 241, 287);
	m_CapElement.SetTexture(0, &rcTexture);
	m_CapElement.TextureColor.States[DXUT_STATE_NORMAL] = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_CapElement.FontColor.States[DXUT_STATE_NORMAL] = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_CapElement.SetFont(0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER);
	// Pre-blend as we don't need to transition the state
	m_CapElement.TextureColor.Blend(DXUT_STATE_NORMAL, 10.0f);
	m_CapElement.FontColor.Blend(DXUT_STATE_NORMAL, 10.0f);

	//-------------------------------------
	// CDXUTStatic
	//-------------------------------------
	Element.SetFont(0);
	Element.FontColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(200, 200, 200, 200);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_STATIC, 0, &Element);


	//-------------------------------------
	// CDXUTButton - Button
	//-------------------------------------
	SetRect(&rcTexture, 0, 0, 136, 54);
	Element.SetTexture(0, &rcTexture);
	Element.SetFont(0);
	Element.TextureColor.States[DXUT_STATE_NORMAL] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_PRESSED] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.FontColor.States[DXUT_STATE_MOUSEOVER] = D3DCOLOR_ARGB(255, 0, 0, 0);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_BUTTON, 0, &Element);


	//-------------------------------------
	// CDXUTButton - Fill layer
	//-------------------------------------
	SetRect(&rcTexture, 136, 0, 252, 54);
	Element.SetTexture(0, &rcTexture, D3DCOLOR_ARGB(0, 255, 255, 255));
	Element.TextureColor.States[DXUT_STATE_MOUSEOVER] = D3DCOLOR_ARGB(160, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_PRESSED] = D3DCOLOR_ARGB(60, 0, 0, 0);
	Element.TextureColor.States[DXUT_STATE_FOCUS] = D3DCOLOR_ARGB(30, 255, 255, 255);


	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_BUTTON, 1, &Element);


	//-------------------------------------
	// CDXUTCheckBox - Box
	//-------------------------------------
	SetRect(&rcTexture, 0, 54, 27, 81);
	Element.SetTexture(0, &rcTexture);
	Element.SetFont(0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER);
	Element.FontColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(200, 200, 200, 200);
	Element.TextureColor.States[DXUT_STATE_NORMAL] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_FOCUS] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_PRESSED] = D3DCOLOR_ARGB(255, 255, 255, 255);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_CHECKBOX, 0, &Element);


	//-------------------------------------
	// CDXUTCheckBox - Check
	//-------------------------------------
	SetRect(&rcTexture, 27, 54, 54, 81);
	Element.SetTexture(0, &rcTexture);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_CHECKBOX, 1, &Element);


	//-------------------------------------
	// CDXUTRadioButton - Box
	//-------------------------------------
	SetRect(&rcTexture, 54, 54, 81, 81);
	Element.SetTexture(0, &rcTexture);
	Element.SetFont(0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER);
	Element.FontColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(200, 200, 200, 200);
	Element.TextureColor.States[DXUT_STATE_NORMAL] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_FOCUS] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_PRESSED] = D3DCOLOR_ARGB(255, 255, 255, 255);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_RADIOBUTTON, 0, &Element);


	//-------------------------------------
	// CDXUTRadioButton - Check
	//-------------------------------------
	SetRect(&rcTexture, 81, 54, 108, 81);
	Element.SetTexture(0, &rcTexture);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_RADIOBUTTON, 1, &Element);


	//-------------------------------------
	// CDXUTComboBox - Main
	//-------------------------------------
	SetRect(&rcTexture, 7, 81, 247, 123);
	Element.SetTexture(0, &rcTexture);
	Element.SetFont(0);
	Element.TextureColor.States[DXUT_STATE_NORMAL] = D3DCOLOR_ARGB(150, 200, 200, 200);
	Element.TextureColor.States[DXUT_STATE_FOCUS] = D3DCOLOR_ARGB(170, 230, 230, 230);
	Element.TextureColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(70, 200, 200, 200);
	Element.FontColor.States[DXUT_STATE_MOUSEOVER] = D3DCOLOR_ARGB(255, 0, 0, 0);
	Element.FontColor.States[DXUT_STATE_PRESSED] = D3DCOLOR_ARGB(255, 0, 0, 0);
	Element.FontColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(200, 200, 200, 200);


	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_COMBOBOX, 0, &Element);


	//-------------------------------------
	// CDXUTComboBox - Button
	//-------------------------------------
	SetRect(&rcTexture, 98, 189, 151, 238);
	Element.SetTexture(0, &rcTexture);
	Element.TextureColor.States[DXUT_STATE_NORMAL] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_PRESSED] = D3DCOLOR_ARGB(255, 150, 150, 150);
	Element.TextureColor.States[DXUT_STATE_FOCUS] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(70, 255, 255, 255);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_COMBOBOX, 1, &Element);


	//-------------------------------------
	// CDXUTComboBox - Dropdown
	//-------------------------------------
	SetRect(&rcTexture, 13, 123, 241, 160);
	Element.SetTexture(0, &rcTexture);
	Element.SetFont(0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_COMBOBOX, 2, &Element);


	//-------------------------------------
	// CDXUTComboBox - Selection
	//-------------------------------------
	SetRect(&rcTexture, 12, 163, 239, 183);
	Element.SetTexture(0, &rcTexture);
	Element.SetFont(0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_COMBOBOX, 3, &Element);


	//-------------------------------------
	// CDXUTSlider - Track
	//-------------------------------------
	SetRect(&rcTexture, 1, 187, 93, 228);
	Element.SetTexture(0, &rcTexture);
	Element.TextureColor.States[DXUT_STATE_NORMAL] = D3DCOLOR_ARGB(150, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_FOCUS] = D3DCOLOR_ARGB(200, 255, 255, 255);
	Element.TextureColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(70, 255, 255, 255);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_SLIDER, 0, &Element);

	//-------------------------------------
	// CDXUTSlider - Button
	//-------------------------------------
	SetRect(&rcTexture, 151, 193, 192, 234);
	Element.SetTexture(0, &rcTexture);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_SLIDER, 1, &Element);

	//-------------------------------------
	// CDXUTScrollBar - Track
	//-------------------------------------
	int nScrollBarStartX = 196;
	int nScrollBarStartY = 191;
	SetRect(&rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 21, nScrollBarStartX + 22, nScrollBarStartY + 32);
	Element.SetTexture(0, &rcTexture);
	Element.TextureColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(255, 200, 200, 200);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_SCROLLBAR, 0, &Element);

	//-------------------------------------
	// CDXUTScrollBar - Up Arrow
	//-------------------------------------
	SetRect(&rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 1, nScrollBarStartX + 22, nScrollBarStartY + 21);
	Element.SetTexture(0, &rcTexture);
	Element.TextureColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(255, 200, 200, 200);


	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_SCROLLBAR, 1, &Element);

	//-------------------------------------
	// CDXUTScrollBar - Down Arrow
	//-------------------------------------
	SetRect(&rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 32, nScrollBarStartX + 22, nScrollBarStartY + 53);
	Element.SetTexture(0, &rcTexture);
	Element.TextureColor.States[DXUT_STATE_DISABLED] = D3DCOLOR_ARGB(255, 200, 200, 200);


	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_SCROLLBAR, 2, &Element);

	//-------------------------------------
	// CDXUTScrollBar - Button
	//-------------------------------------
	SetRect(&rcTexture, 220, 192, 238, 234);
	Element.SetTexture(0, &rcTexture);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_SCROLLBAR, 3, &Element);


	//-------------------------------------
	// CDXUTEditBox
	//-------------------------------------
	// Element assignment:
	//   0 - text area
	//   1 - top left border
	//   2 - top border
	//   3 - top right border
	//   4 - left border
	//   5 - right border
	//   6 - lower left border
	//   7 - lower border
	//   8 - lower right border

	Element.SetFont(0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP);

	// Assign the style
	SetRect(&rcTexture, 14, 90, 241, 113);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 0, &Element);
	SetRect(&rcTexture, 8, 82, 14, 90);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 1, &Element);
	SetRect(&rcTexture, 14, 82, 241, 90);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 2, &Element);
	SetRect(&rcTexture, 241, 82, 246, 90);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 3, &Element);
	SetRect(&rcTexture, 8, 90, 14, 113);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 4, &Element);
	SetRect(&rcTexture, 241, 90, 246, 113);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 5, &Element);
	SetRect(&rcTexture, 8, 113, 14, 121);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 6, &Element);
	SetRect(&rcTexture, 14, 113, 241, 121);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 7, &Element);
	SetRect(&rcTexture, 241, 113, 246, 121);
	Element.SetTexture(0, &rcTexture);
	SetDefaultElement(DXUT_CONTROL_EDITBOX, 8, &Element);

	//-------------------------------------
	// CDXUTListBox - Main
	//-------------------------------------
	SetRect(&rcTexture, 13, 123, 241, 160);
	Element.SetTexture(0, &rcTexture);
	Element.SetFont(0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_LISTBOX, 0, &Element);

	//-------------------------------------
	// CDXUTListBox - Selection
	//-------------------------------------

	SetRect(&rcTexture, 16, 166, 240, 183);
	Element.SetTexture(0, &rcTexture);
	Element.SetFont(0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP);

	// Assign the Element
	SetDefaultElement(DXUT_CONTROL_LISTBOX, 1, &Element);
}

void Dialog::OnMouseMove(POINT pt)
{
	Control *pControl = GetControlAtPoint(pt);

	if (pControl == m_pControlMouseOver)
		return;

	if (m_pControlMouseOver)
		m_pControlMouseOver->OnMouseLeave();

	m_pControlMouseOver = pControl;
	if (pControl != NULL)
		m_pControlMouseOver->OnMouseEnter();
}

void Dialog::OnMouseUp(POINT pt)
{
	s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;
}

void Dialog::SetNextDialog(Dialog * pNextDialog)
{
	if (pNextDialog == NULL)
		pNextDialog = this;

	m_pNextDialog = pNextDialog;
	if (pNextDialog)
		m_pNextDialog->m_pPrevDialog = this;
}

bool Dialog::OnCycleFocus(bool bForward)
{
	Control* pControl = NULL;
	Dialog* pDialog = NULL; // pDialog and pLastDialog are used to track wrapping of
	Dialog* pLastDialog;    // focus from first control to last or vice versa.

	if (s_pControlFocus == NULL)
	{
		// If s_pControlFocus is NULL, we focus the first control of first dialog in
		// the case that bForward is true, and focus the last control of last dialog when
		// bForward is false.
		//
		if (bForward)
		{
			// Search for the first control from the start of the dialog
			// array.
			for (int d = 0; d < m_pManager->m_Dialogs.GetSize(); ++d)
			{
				pDialog = pLastDialog = m_pManager->m_Dialogs.GetAt(d);
				if (pDialog && pDialog->m_Controls.GetSize() > 0)
				{
					pControl = pDialog->m_Controls.GetAt(0);
					break;
				}
			}

			if (!pDialog || !pControl)
			{
				// No dialog has been registered yet or no controls have been
				// added to the dialogs. Cannot proceed.
				return true;
			}
		}
		else
		{
			// Search for the first control from the end of the dialog
			// array.
			for (int d = m_pManager->m_Dialogs.GetSize() - 1; d >= 0; --d)
			{
				pDialog = pLastDialog = m_pManager->m_Dialogs.GetAt(d);
				if (pDialog && pDialog->m_Controls.GetSize() > 0)
				{
					pControl = pDialog->m_Controls.GetAt(pDialog->m_Controls.GetSize() - 1);
					break;
				}
			}

			if (!pDialog || !pControl)
			{
				// No dialog has been registered yet or no controls have been
				// added to the dialogs. Cannot proceed.
				return true;
			}
		}
	}
	else if (s_pControlFocus->m_pDialog != this)
	{
		// If a control belonging to another dialog has focus, let that other
		// dialog handle this event by returning false.
		//
		return false;
	}
	else
	{
		// Focused control belongs to this dialog. Cycle to the
		// next/previous control.
		pLastDialog = s_pControlFocus->m_pDialog;
		pControl = (bForward) ? GetNextControl(s_pControlFocus) : GetPrevControl(s_pControlFocus);
		pDialog = pControl->m_pDialog;
	}

	for (int i = 0; i < 0xffff; i++)
	{
		// If we just wrapped from last control to first or vice versa,
		// set the focused control to NULL. This state, where no control
		// has focus, allows the camera to work.
		int nLastDialogIndex = m_pManager->m_Dialogs.IndexOf(pLastDialog);
		int nDialogIndex = m_pManager->m_Dialogs.IndexOf(pDialog);
		if ((!bForward && nLastDialogIndex < nDialogIndex) ||
			(bForward && nDialogIndex < nLastDialogIndex))
		{
			if (s_pControlFocus)
				s_pControlFocus->OnFocusOut();
			s_pControlFocus = NULL;
			return true;
		}

		// If we've gone in a full circle then focus doesn't change
		if (pControl == s_pControlFocus)
			return true;

		// If the dialog accepts keybord input and the control can have focus then
		// move focus
		if (pControl->m_pDialog->m_bKeyboardInput && pControl->CanHaveFocus())
		{
			if (s_pControlFocus)
				s_pControlFocus->OnFocusOut();
			s_pControlFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return true;
		}

		pLastDialog = pDialog;
		pControl = (bForward) ? GetNextControl(pControl) : GetPrevControl(pControl);
		pDialog = pControl->m_pDialog;
	}

	return false;
}

void Dialog::Init(DialogResourceManager *pManager, bool bRegisterDialog)
{
	m_pManager = pManager;

	if (bRegisterDialog)
		pManager->RegisterDialog(this);

	SetTexture(0, MAKEINTRESOURCE(0xFFFF), (HMODULE)0xFFFF);
	InitDefaultElements();
}

void Dialog::Init(DialogResourceManager *pManager, bool bRegisterDialog, LPCWSTR pszControlTextureFilename)
{
	m_pManager = pManager;

	if (bRegisterDialog)
		pManager->RegisterDialog(this);

	SetTexture(0, pszControlTextureFilename);
	InitDefaultElements();
}

void Dialog::Init(DialogResourceManager *pManager, bool bRegisterDialog, LPCWSTR szControlTextureResourceName, HMODULE hControlTextureResourceModule)
{
	m_pManager = pManager;

	if (bRegisterDialog)
		pManager->RegisterDialog(this);

	SetTexture(0, szControlTextureResourceName, hControlTextureResourceModule);
	InitDefaultElements();
}

bool Dialog::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bHandled = false;

	// Для невидимых элементов не обрабатываем сообщение
	if (!m_bVisible)
		return false;

	// Пока что не понятно что делает эта часть закомпнчу.
	// If automation command-line switch is on, enable this dialog's keyboard input
	// upon any key press or mouse click.
	/*if (DXUTGetAutomation() &&
		(WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg || WM_KEYDOWN == uMsg))
	{
		m_pManager->EnableKeyboardInputForAllDialogs();
	}*/

	// Перетаскивание элементов пока что не буду делать
	// Если есть заголовок, проверяю что нажали на заголовок
	/*if (m_bCaption)
	{
		if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK)
		{
			POINT mousePoint =
			{
				short(LOWORD(lParam)), short(HIWORD(lParam))
			};

			if (mousePoint.x >= m_x && mousePoint.x < m_x + m_width &&
				mousePoint.y >= m_y && mousePoint.y < m_y + m_nCaptionHeight)
			{
				m_bDrag = true;
				SetCapture(DXUTGetHWND());
				return true;
			}
		}
		else if (uMsg == WM_LBUTTONUP && m_bDrag)
		{
			POINT mousePoint =
			{
				short(LOWORD(lParam)), short(HIWORD(lParam))
			};

			if (mousePoint.x >= m_x && mousePoint.x < m_x + m_width &&
				mousePoint.y >= m_y && mousePoint.y < m_y + m_nCaptionHeight)
			{
				ReleaseCapture();
				m_bDrag = false;
				m_bMinimized = !m_bMinimized;
				return true;
			}
		}
	}*/

	// Если элемент свернут не обрабатываем сообщение
	if (m_bMinimized)
		return false;

	// Если элемент управления выделен, и он пренадлежит этому диалогу, и он включен
	// то передаю сообщение на обработку выделенному элементу управления
	if (s_pControlFocus && s_pControlFocus->m_pDialog == this && s_pControlFocus->GetEnabled())
	{
		// Если сообщение обработал элемент управления то выходим из общего обработчика
		if (s_pControlFocus->MsgProc(uMsg, wParam, lParam))
			return true;
	}

	switch (uMsg)
	{
	case WM_SIZE:
	case WM_MOVE:
	{
		POINT pt = { -1,-1 };
		OnMouseMove(pt);
		break;
	}
	default:
		break;
	}
	return bHandled;
}

HRESULT Dialog::AddStatic(int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault, Static ** ppCreated)
{
	HRESULT hr = S_OK;

	Static *pStatic = new Static(this);

	if (ppCreated != NULL)
		return E_OUTOFMEMORY;

	hr = AddControl(pStatic);
	if (FAILED(hr))
		return hr;

	pStatic->SetID(ID);
	pStatic->SetText(strText);
	pStatic->SetLocation(x, y);
	pStatic->SetSize(width, height);
	pStatic->m_bIsDefault = bIsDefault;

	return S_OK;
}

HRESULT Dialog::AddButton(int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault, Button ** ppCreated)
{
	HRESULT hr = S_OK;

	Button *pButton = new Button(this);

	if (ppCreated != NULL)
		*ppCreated = pButton;

	if (pButton == NULL)
		return E_OUTOFMEMORY;

	hr = AddControl(pButton);
	if (FAILED(hr))
		return hr;

	pButton->SetID(ID);
	pButton->SetText(strText);
	pButton->SetLocation(x, y);
	pButton->SetSize(width, height);
	pButton->SetHotkey(nHotkey);
	pButton->m_bIsDefault = bIsDefault;

	return S_OK;
}

HRESULT Dialog::AddControl(Control * pControl)
{
	HRESULT hr = S_OK;

	hr = InitControl(pControl);
	if (FAILED(hr))
		return hr;

	// Добавляем элемент в список
	hr = m_Controls.Add(pControl);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Dialog::InitControl(Control * pControl)
{
	HRESULT hr;

	if (pControl == NULL)
		return E_INVALIDARG;

	pControl->m_Index = m_Controls.GetSize();

	for (int i = 0; i < m_DefaultElements.GetSize(); i++)
	{
		ElementHolder *pElementHolder = m_DefaultElements.GetAt(i);
		if (pElementHolder->nControlType == pControl->GetType())
			pControl->SetElement(pElementHolder->iElement, &pElementHolder->Element);
	}

	hr = pControl->OnInit();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

Control *Dialog::GetControl(int ID)
{
	// Пробуем найти элемент с данным идом
	for (int i = 0; i < m_Controls.GetSize(); i++)
	{
		Control *pControl = m_Controls.GetAt(i);

		if (pControl->GetID() == ID)
		{
			return pControl;
		}
	}

	// Не нашли
	return NULL;
}

Control *Dialog::GetControl(int ID, UINT nControlType)
{
	// Пробую найти элемент с данным идом
	for (int i = 0; i < m_Controls.GetSize(); i++)
	{
		Control *pControl = m_Controls.GetAt(i);

		if (pControl->GetID() == ID && pControl->GetType() == nControlType)
		{
			return pControl;
		}
	}

	// Не нашли
	return NULL;
}

Control *Dialog::GetControlAtPoint(POINT pt)
{
	// Поиск всех дочерних элементов от первого на котором стоит мышка
	for (int i = 0; i < m_Controls.GetSize(); i++)
	{
		Control *pControl = m_Controls.GetAt(i);

		if (pControl == NULL)
		{
			continue;
		}

		// Возвращаем текущий элемент если она включена и видна. 
		if (pControl->ContainsPoint(pt) && pControl->GetEnabled() && pControl->GetVisible())
		{
			return pControl;
		}
	}

	return NULL;
}

bool Dialog::GetControlEnabled(int ID)
{
	Control *pControl = GetControl(ID);
	if (pControl == NULL)
		return false;

	return pControl->GetEnabled();
}

void Dialog::SetControlEnabled(int ID, bool bEnabled)
{
	Control *pControl = GetControl(ID);
	if (pControl == NULL)
		return;

	pControl->SetEnabled(bEnabled);
}

HRESULT Dialog::SetDefaultElement(UINT nControlType, UINT iElement, Element *pElement)
{
	// Если данный элемент есть в списке то просто его обновляю
	for (int i = 0; i < m_DefaultElements.GetSize(); i++)
	{
		ElementHolder *pElementHolder = m_DefaultElements.GetAt(i);

		if (pElementHolder->nControlType == nControlType && pElementHolder->iElement == iElement)
		{
			pElementHolder->Element = *pElement;
			return S_OK;
		}
	}

	// Если не нашли в списке, то добавляю новый
	ElementHolder *pNewHolder;
	pNewHolder = new ElementHolder;
	if (pNewHolder == NULL)
		return E_OUTOFMEMORY;

	pNewHolder->nControlType = nControlType;
	pNewHolder->iElement = iElement;
	pNewHolder->Element = *pElement;

	HRESULT hr = m_DefaultElements.Add(pNewHolder);
	if (FAILED(hr))
	{
		delete pNewHolder;
	}

	return hr;
}

Element *Dialog::GetDefaultElement(UINT nControlType, UINT iElement)
{
	for (int i = 0; i < m_DefaultElements.GetSize(); i++)
	{
		ElementHolder *pElementHolder = m_DefaultElements.GetAt(i);
		if (pElementHolder->nControlType == nControlType && pElementHolder->iElement == iElement)
		{
			return &pElementHolder->Element;
		}
	}

	return NULL;
}

void Dialog::SendEvent(UINT nEvent, bool bTriggeredByUser, Control * pControl)
{
	// Если нету зарегистрированного обратного вызова, то не отправляем событие
	if (m_pCallbackEvent == NULL)
		return;

	// Если событие не программное и у элемента не была включена обработка программных событий то выходим
	if (!bTriggeredByUser && !m_bNonUserEvents)
		return;

	m_pCallbackEvent(nEvent, pControl->GetID(), pControl, m_pCallbackEventUserContext);
}

void Dialog::RequestFocus(Control * pControl)
{
	if (s_pControlFocus == pControl)
		return;

	if (!pControl->CanHaveFocus())
		return;

	if (s_pControlFocus)
		s_pControlFocus->OnFocusOut();

	pControl->OnFocusIn();
	s_pControlFocus = pControl;
}

ID3D11Buffer *g_pFontBuffer11 = NULL;
UINT g_FontBufferBytes11 = 0;
CGrowableArray<DXUTSpriteVertex> g_FontVertices;
ID3D11ShaderResourceView *g_pFont11 = NULL;
ID3D11InputLayout *g_pInputLayout11 = NULL;
void DrawText11DXUT(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3d11DeviceContext,
	LPCWSTR strText, RECT rcScreen, D3DXCOLOR vFontColor,
	float fBBWidth, float fBBHeight, bool bCenter)
{
	float fCharTextSizeX = 0.010526315f;
	float fGlyphSizeX = 15.0f / fBBWidth;
	float fGlyphSizeY = 42.0f / fBBHeight;

	float fRectLeft = rcScreen.left / fBBWidth;
	float fRectTop = 1.0f - rcScreen.top / fBBHeight;

	fRectLeft = fRectLeft * 2.0f - 1.0f;
	fRectTop = fRectTop * 2.0f - 1.0f;

	int NumChars = (int)wcslen(strText);
	if (bCenter)
	{
		float fRectRight = rcScreen.right / fBBWidth;
		fRectRight = fRectRight * 2.0f - 1.0f;
		float fRectBottom = 1.0f - rcScreen.bottom / fBBHeight;
		fRectBottom = fRectBottom * 2.0f - 1.0f;
		float fcenterx = ((fRectRight - fRectLeft) - (float)NumChars*fGlyphSizeX)*0.5f;
		float fcentery = ((fRectTop - fRectBottom) - (float)1 * fGlyphSizeY)*0.5f;
		fRectLeft += fcenterx;
		fRectTop -= fcentery;
	}
	float fOriginalLeft = fRectLeft;
	float fTexTop = 0.0f;
	float fTexBottom = 1.0f;

	float fDepth = 0.5f;
	for (int i = 0; i < NumChars; i++)
	{
		if (strText[i] == '\n')
		{
			fRectLeft = fOriginalLeft;
			fRectTop -= fGlyphSizeY;

			continue;
		}
		else if (strText[i] < 32 || strText[i] > 126)
		{
			continue;
		}

		// добавляю 6 спрайт вершин
		DXUTSpriteVertex SpriteVertex;
		float fRectRight = fRectLeft + fGlyphSizeX;
		float fRectBottom = fRectTop - fGlyphSizeY;
		float fTexLeft = (strText[i] - 32)*fCharTextSizeX;
		float fTexRight = fTexLeft + fCharTextSizeX;

		// Первая тройка
		SpriteVertex.vPos = D3DXVECTOR3(fRectLeft, fRectTop, fDepth);
		SpriteVertex.vTex = D3DXVECTOR2(fTexLeft, fTexTop);
		SpriteVertex.vColor = vFontColor;
		g_FontVertices.Add(SpriteVertex);

		SpriteVertex.vPos = D3DXVECTOR3(fRectRight, fRectTop, fDepth);
		SpriteVertex.vTex = D3DXVECTOR2(fTexRight, fTexTop);
		SpriteVertex.vColor = vFontColor;
		g_FontVertices.Add(SpriteVertex);

		SpriteVertex.vPos = D3DXVECTOR3(fRectLeft, fRectBottom, fDepth);
		SpriteVertex.vTex = D3DXVECTOR2(fTexLeft, fTexBottom);
		SpriteVertex.vColor = vFontColor;
		g_FontVertices.Add(SpriteVertex);

		// Вторая тройка
		SpriteVertex.vPos = D3DXVECTOR3(fRectRight, fRectTop, fDepth);
		SpriteVertex.vTex = D3DXVECTOR2(fTexRight, fTexTop);
		SpriteVertex.vColor = vFontColor;
		g_FontVertices.Add(SpriteVertex);

		SpriteVertex.vPos = D3DXVECTOR3(fRectRight, fRectBottom, fDepth);
		SpriteVertex.vTex = D3DXVECTOR2(fTexRight, fTexBottom);
		SpriteVertex.vColor = vFontColor;
		g_FontVertices.Add(SpriteVertex);

		SpriteVertex.vPos = D3DXVECTOR3(fRectLeft, fRectBottom, fDepth);
		SpriteVertex.vTex = D3DXVECTOR2(fTexLeft, fTexBottom);
		SpriteVertex.vColor = vFontColor;
		g_FontVertices.Add(SpriteVertex);

		fRectLeft += fGlyphSizeX;
	}

	// Мы должны закончить текст после каждой строки, чтобы сохранить порядок рендерига между спрайтами и шрифтами
	EndText11(pd3dDevice, pd3d11DeviceContext);
}

void EndText11(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3d11DeviceContext)
{
	UINT FontDataButes = g_FontVertices.GetSize() * sizeof(DXUTSpriteVertex);
	if (g_FontBufferBytes11 < FontDataButes)
	{
		_RELEASE(g_pFontBuffer11);
		g_FontBufferBytes11 = FontDataButes;

		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.ByteWidth = g_FontBufferBytes11;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;

		pd3dDevice->CreateBuffer(&BufferDesc, NULL, &g_pFontBuffer11);
		DXUT_SetDebugName(g_pFontBuffer11, "DXUT Text11");
	}

	D3D11_BOX destRegion;
	destRegion.left = 0;
	destRegion.right = FontDataButes;
	destRegion.top = 0;
	destRegion.bottom = 1;
	destRegion.front = 0;
	destRegion.back = 1;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (S_OK == pd3d11DeviceContext->Map(g_pFontBuffer11, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource))
	{
		CopyMemory(MappedResource.pData, (void*)g_FontVertices.GetData(), FontDataButes);
		pd3d11DeviceContext->Unmap(g_pFontBuffer11, 0);
	}

	ID3D11ShaderResourceView *pOldTexture = NULL;
	pd3d11DeviceContext->PSGetShaderResources(0, 1, &pOldTexture);
	pd3d11DeviceContext->PSSetShaderResources(0, 1, &g_pFont11);

	// Рисуем
	UINT Stride = sizeof(DXUTSpriteVertex);
	UINT Offset = 0;
	pd3d11DeviceContext->IASetVertexBuffers(0, 1, &g_pFontBuffer11, &Stride, &Offset);
	pd3d11DeviceContext->IASetInputLayout(g_pInputLayout11);
	pd3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3d11DeviceContext->Draw(g_FontVertices.GetSize(), 0);

	pd3d11DeviceContext->PSSetShaderResources(0, 1, &pOldTexture);
	_RELEASE(pOldTexture);

	g_FontVertices.Reset();
}

HRESULT InitFont11(ID3D11Device *pd3d11Device, ID3D11InputLayout *pInputLayout)
{
	HRESULT hr = S_OK;
	WCHAR str[MAX_PATH];
	// Поиск существования файла по заданному пути, но я пока что не буду искать
	//V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"UI\\Font.dds"));
	wcscpy_s(str, MAX_PATH, L"Font.dds");

	if (pd3d11Device->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
	{
		D3DX11_IMAGE_INFO dii;
		D3DX11GetImageInfoFromFile(str, NULL, &dii, NULL);

		D3DX11_IMAGE_LOAD_INFO dili;
		dili.BindFlags = D3DX11_DEFAULT;
		dili.CpuAccessFlags = D3DX11_DEFAULT;
		dili.Depth = D3DX11_DEFAULT;
		dili.Filter = D3DX11_DEFAULT;
		dili.FirstMipLevel = 0;
		dili.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dili.Height = D3DX11_DEFAULT;
		dili.MipFilter = D3DX11_DEFAULT;
		dili.MipLevels = 1;
		dili.MiscFlags = D3DX11_DEFAULT;
		dili.pSrcInfo = &dii;
		dili.Usage = D3D11_USAGE_DEFAULT;
		dili.Width = D3DX11_DEFAULT;

		hr = D3DX11CreateShaderResourceViewFromFile(pd3d11Device, str, &dili, NULL, &g_pFont11, &hr);
	}
	else
	{
		hr = D3DX11CreateShaderResourceViewFromFile(pd3d11Device, str, NULL, NULL, &g_pFont11, NULL);
		if (FAILED(hr))
			return hr;
	}

#if defined (PROFILE) || defined(DEBUG)
	if (g_pFont11)
	{
		ID3D11Resource *pRes = NULL;
		g_pFont11->GetResource(&pRes);
		DXUT_SetDebugName(pRes, "DXUT Text11");
		_RELEASE(pRes);
	}
	DXUT_SetDebugName(g_pFont11, "DXUT Text11");
#endif

	g_pInputLayout11 = pInputLayout;
	return hr;
}

void EndFont11()
{
	_RELEASE(g_pFontBuffer11);
	g_FontBufferBytes11 = 0;
	_RELEASE(g_pFont11);
}

void BeginText11()
{
	g_FontVertices.Reset();
}