#pragma once

#include "D3D11_Framework.h"
#include "Misc.h"
#include "DXUTres.h"
#include "Dialog.h"

extern CHAR g_strUIEffectFile[];
extern const UINT g_uUIEffectFileSize;

//class Dialog;

struct DXUT_SCREEN_VERTEX_10
{
	float x, y, z;
	D3DCOLORVALUE color;
	float tu, tv;
};

struct DXUTSpriteVertex
{
	D3DXVECTOR3 vPos;
	D3DXCOLOR vColor;
	D3DXVECTOR2 vTex;
};

/*struct DXUTFontNode
{
	WCHAR strFace[MAX_PATH];
	LONG nHeight;
	LONG nWeight;
};*/

//-------------------------------------------------------------------
// Управляет общими ресурсами класса Dialog
//-------------------------------------------------------------------
class DialogResourceManager
{
public:
	// Шейдеры
	ID3D11VertexShader *m_pVSRenderUI11;
	ID3D11PixelShader *m_pPSRenderUI11;
	ID3D11PixelShader *m_pPSRenderUIUntex11;

	// Состояния
	ID3D11DepthStencilState *m_pDepthStencilStateUI11;
	ID3D11RasterizerState *m_pRasterizerStateUI11;
	ID3D11BlendState *m_pBlendStateUI11;
	ID3D11SamplerState *m_pSamplerStateUI11;

	// Сохраненные состояния
	ID3D11DepthStencilState *m_pDepthStencilStateStored11;
	UINT m_StencilRefStored11;
	ID3D11RasterizerState *m_pRasterizerStateStored11;
	ID3D11BlendState *m_pBlendStateStored11;
	float m_BlendFactorStored11[4];
	UINT m_SampleMaskStored11;
	ID3D11SamplerState *m_pSamplerStateStored11;

	ID3D11InputLayout *m_pInputLayout11;
	ID3D11Buffer *m_pVBScreenQuad11;


	ID3D11Buffer *m_pSpriteBuffer11;
	UINT m_SpriteBufferBytes11;
	CGrowableArray<DXUTSpriteVertex> m_SpriteVertices;

	UINT m_nBackBufferWidth;
	UINT m_nBackBufferHeight;

	CGrowableArray<Dialog*> m_Dialogs; // Зарегистрированные элементы

	DialogResourceManager();
	~DialogResourceManager();

	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT OnD3D11CreateDevice(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3d11DeviceContext);
	HRESULT OnD3D11ResizedSwapChain(ID3D11Device *pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
	void OnD3D11ReleasingSwapChain();
	void OnD3D11DestroyDevice();
	void StoreD3D11State(ID3D11DeviceContext *pd3dImmediateContext);
	void RestoreD3D11State(ID3D11DeviceContext *pd3dImmediateContext);
	void ApplyRenderUI11(ID3D11DeviceContext *pd3dImmediateContext);
	void ApplyRenderUIUntex11(ID3D11DeviceContext *pd3dImmediateContext);
	void BeginSprites11();
	void EndSprites11(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext);
	ID3D11Device *GetD3D11Device() { return m_pd3d11Device; }
	ID3D11DeviceContext *GetD3D11DeviceContext() { return m_pd3d11DeviceContext; }

	//DXUTFontNode *GetFontNode(int iIndex) { return m_FontCache.GetAt(iIndex); }
	DXUTTextureNode *GetTextureNode(int iIndex) { return m_TextureCache.GetAt(iIndex); }

	//int AddFont(LPCWSTR strFaceName, LONG height, LONG weight);
	int AddTexture(LPCWSTR strFilename);
	int AddTexture(LPCWSTR strResourceName, HMODULE hResourceModule);

	bool RegisterDialog(Dialog *pDialog);
	void UnregisterDialog(Dialog *pDialog);
	void EnableKeyboardInputForAllDialogs();
	
protected:
	ID3D11Device *m_pd3d11Device;
	ID3D11DeviceContext *m_pd3d11DeviceContext;

	HRESULT CreateFont11(UINT iFont);
	HRESULT CreateTexture11(UINT iTexture);

	CGrowableArray<DXUTTextureNode*> m_TextureCache; // Общие текстуры
	//CGrowableArray<DXUTFontNode*> m_FontCache; // Общие шрифты
};