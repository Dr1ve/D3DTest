#pragma once

#include "D3D11_Framework.h"
#include "d3d9types.h"
#include "d3dx9math.h"

enum DXUT_CONTROL_STATE
{
	DXUT_STATE_NORMAL = 0,
	DXUT_STATE_DISABLED,
	DXUT_STATE_HIDDEN,
	DXUT_STATE_FOCUS,
	DXUT_STATE_MOUSEOVER,
	DXUT_STATE_PRESSED,
};

#define MAX_CONTROL_STATES 6

struct BlendColor
{
	void Init(D3DCOLOR defaultColor, D3DCOLOR disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), D3DCOLOR hiddenColor = 0);
	void Blend(UINT iState, float fElapsedTime, float fRate = 0.7f);

	D3DCOLOR States[MAX_CONTROL_STATES];
	D3DXCOLOR Current;
};

//-------------------------------------------------------------------
// Содержит все элементы для субконтроля
//-------------------------------------------------------------------
class Element
{
public:
	UINT iTexture; // Индекс текстуры для этого элемента
	UINT iFont; // Индекс шрифта для этого элемента
	DWORD dwTextFormat; // Формат текста в DrawText

	RECT rcTexture; // Прямоугольник для текстуры элемента

	BlendColor TextureColor;
	BlendColor FontColor;

	void SetTexture(UINT iTexture, RECT *prcTexture, D3DCOLOR defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255));
	void SetFont(UINT iFont, D3DCOLOR defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER);
	void Refresh();
};