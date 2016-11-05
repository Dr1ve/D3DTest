#pragma once

#include "D3D11_Framework.h"
#include "Misc.h"

#ifndef DXUT_RES_H
#define DXUT_RES_H
HRESULT WINAPI DXUTCreateGUITextureFromInternalArray11(ID3D11Device* pd3dDevice, ID3D11Texture2D** ppTexture, D3DX11_IMAGE_INFO* pInfo);
#endif