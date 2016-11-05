#include "Element.h"

void BlendColor::Init(D3DCOLOR defaultColor, D3DCOLOR disabledColor, D3DCOLOR hiddenColor)
{
	for (int i = 0; i < MAX_CONTROL_STATES; i++)
	{
		States[i] = defaultColor;
	}

	States[DXUT_STATE_DISABLED] = disabledColor;
	States[DXUT_STATE_HIDDEN] = hiddenColor;
	Current = hiddenColor;
}

void BlendColor::Blend(UINT iState, float fElapsedTime, float fRate)
{
	D3DXCOLOR destColor = States[iState];
	D3DXColorLerp(&Current, &Current, &destColor, 1.0f - powf(fRate, 30 * fElapsedTime));
}

void Element::SetTexture(UINT iTexture, RECT *prcTexture, D3DCOLOR defaultTextureColor)
{
	this->iTexture = iTexture;

	if (prcTexture)
		rcTexture = *prcTexture;
	else
		SetRectEmpty(&rcTexture);

	TextureColor.Init(defaultTextureColor);
}

void Element::SetFont(UINT iFont, D3DCOLOR defaultFontColor, DWORD dwTextFormat)
{
	this->iFont = iFont;
	this->dwTextFormat = dwTextFormat;

	FontColor.Init(defaultFontColor);
}

void Element::Refresh()
{
	TextureColor.Current = TextureColor.States[DXUT_STATE_HIDDEN];
	FontColor.Current = FontColor.States[DXUT_STATE_HIDDEN];
}