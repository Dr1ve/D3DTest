#include "DialogResourceManager.h"

CHAR g_strUIEffectFile[] = \
"Texture2D g_Texture;"\
""\
"SamplerState Sampler"\
"{"\
"    Filter = MIN_MAG_MIP_LINEAR;"\
"    AddressU = Wrap;"\
"    AddressV = Wrap;"\
"};"\
""\
"BlendState UIBlend"\
"{"\
"    AlphaToCoverageEnable = FALSE;"\
"    BlendEnable[0] = TRUE;"\
"    SrcBlend = SRC_ALPHA;"\
"    DestBlend = INV_SRC_ALPHA;"\
"    BlendOp = ADD;"\
"    SrcBlendAlpha = ONE;"\
"    DestBlendAlpha = ZERO;"\
"    BlendOpAlpha = ADD;"\
"    RenderTargetWriteMask[0] = 0x0F;"\
"};"\
""\
"BlendState NoBlending"\
"{"\
"    BlendEnable[0] = FALSE;"\
"    RenderTargetWriteMask[0] = 0x0F;"\
"};"\
""\
"DepthStencilState DisableDepth"\
"{"\
"    DepthEnable = false;"\
"};"\
"DepthStencilState EnableDepth"\
"{"\
"    DepthEnable = true;"\
"};"\
"struct VS_OUTPUT"\
"{"\
"    float4 Pos : POSITION;"\
"    float4 Dif : COLOR;"\
"    float2 Tex : TEXCOORD;"\
"};"\
""\
"VS_OUTPUT VS( float3 vPos : POSITION,"\
"              float4 Dif : COLOR,"\
"              float2 vTexCoord0 : TEXCOORD )"\
"{"\
"    VS_OUTPUT Output;"\
""\
"    Output.Pos = float4( vPos, 1.0f );"\
"    Output.Dif = Dif;"\
"    Output.Tex = vTexCoord0;"\
""\
"    return Output;"\
"}"\
""\
"float4 PS( VS_OUTPUT In ) : SV_Target"\
"{"\
"    return g_Texture.Sample( Sampler, In.Tex ) * In.Dif;"\
"}"\
""\
"float4 PSUntex( VS_OUTPUT In ) : SV_Target"\
"{"\
"    return In.Dif;"\
"}"\
""\
"technique10 RenderUI"\
"{"\
"    pass P0"\
"    {"\
"        SetVertexShader( CompileShader( vs_4_0, VS() ) );"\
"        SetGeometryShader( NULL );"\
"        SetPixelShader( CompileShader( ps_4_0, PS() ) );"\
"        SetDepthStencilState( DisableDepth, 0 );"\
"        SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );"\
"    }"\
"}"\
"technique10 RenderUIUntex"\
"{"\
"    pass P0"\
"    {"\
"        SetVertexShader( CompileShader( vs_4_0, VS() ) );"\
"        SetGeometryShader( NULL );"\
"        SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );"\
"        SetDepthStencilState( DisableDepth, 0 );"\
"        SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );"\
"    }"\
"}"\
"technique10 RestoreState"\
"{"\
"    pass P0"\
"    {"\
"        SetDepthStencilState( EnableDepth, 0 );"\
"        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );"\
"    }"\
"}";
const UINT g_uUIEffectFileSize = sizeof(g_strUIEffectFile);


DialogResourceManager::DialogResourceManager()
{
	m_pVSRenderUI11 = NULL;
	m_pPSRenderUI11 = NULL;
	m_pPSRenderUIUntex11 = NULL;

	m_pDepthStencilStateUI11 = NULL;
	m_pRasterizerStateUI11 = NULL;
	m_pBlendStateUI11 = NULL;
	m_pSamplerStateUI11 = NULL;
	m_pDepthStencilStateStored11 = NULL;
	m_pRasterizerStateStored11 = NULL;
	m_pBlendStateStored11 = NULL;
	m_pSamplerStateStored11 = NULL;

	m_pInputLayout11 = NULL;
	m_pVBScreenQuad11 = NULL;
	m_pSpriteBuffer11 = NULL;
}

DialogResourceManager::~DialogResourceManager()
{
	/*for (int i = 0; i < m_FontCache.GetSize(); i++);
	{
		DXUTFontNode *pFontNode = m_FontCache.GetAt(i);
		_DELETE(pFontNode);
	}
	m_FontCache.RemoveAll();*/

	for (int i = 0; i < m_TextureCache.GetSize(); i++)
	{
		DXUTTextureNode *pTextureNode = m_TextureCache.GetAt(i);
		_DELETE(pTextureNode);
	}
	m_TextureCache.RemoveAll();

	//CUniBuffer::Uninitialize();
}

bool DialogResourceManager::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

HRESULT DialogResourceManager::OnD3D11CreateDevice(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3d11DeviceContext)
{
	m_pd3d11Device = pd3dDevice;
	m_pd3d11DeviceContext = pd3d11DeviceContext;

	HRESULT hr = S_OK;

	ID3DBlob *pVSBlob = NULL;
	ID3DBlob *pPSBlob = NULL;
	ID3DBlob *pPSUntexBlob = NULL;
	hr = D3DCompile(g_strUIEffectFile, g_uUIEffectFileSize, "none", NULL, NULL, "VS", "vs_4_0_level_9_1",
		D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &pVSBlob, NULL);
	if (FAILED(hr))
		return hr;
	hr = D3DCompile(g_strUIEffectFile, g_uUIEffectFileSize, "none", NULL, NULL, "PS", "ps_4_0_level_9_1",
		D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &pPSBlob, NULL);
	if (FAILED(hr))
		return hr;
	hr = D3DCompile(g_strUIEffectFile, g_uUIEffectFileSize, "none", NULL, NULL, "PSUntex", "ps_4_0_level_9_1",
		D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &pPSUntexBlob, NULL);
	if (FAILED(hr))
		return hr;

	hr = pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVSRenderUI11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pVSRenderUI11, "DialogResourceManager");

	hr = pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPSRenderUI11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pPSRenderUI11, "DialogResourceManager");

	hr = pd3dDevice->CreatePixelShader(pPSUntexBlob->GetBufferPointer(), pPSUntexBlob->GetBufferSize(), NULL, &m_pPSRenderUIUntex11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pPSRenderUIUntex11, "DialogResourceManager");

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	hr = pd3dDevice->CreateDepthStencilState(&dsDesc, &m_pDepthStencilStateUI11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pDepthStencilStateUI11, "CDXUTDialogResourceManager");

	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.AntialiasedLineEnable = FALSE;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.MultisampleEnable = TRUE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	hr = pd3dDevice->CreateRasterizerState(&rsDesc, &m_pRasterizerStateUI11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pRasterizerStateUI11, "CDXUTDialogResourceManager");

	D3D11_BLEND_DESC bsDesc;
	ZeroMemory(&bsDesc, sizeof(D3D11_BLEND_DESC));
	bsDesc.RenderTarget[0].BlendEnable = TRUE;
	bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bsDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	hr = pd3dDevice->CreateBlendState(&bsDesc, &m_pBlendStateUI11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pBlendStateUI11, "CDXUTDialogResourceManager");

	D3D11_SAMPLER_DESC ssDesc;
	ZeroMemory(&ssDesc, sizeof(D3D11_SAMPLER_DESC));
	ssDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	ssDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	ssDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	ssDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ssDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ssDesc.MaxAnisotropy = 16;
	ssDesc.MinLOD = 0;
	ssDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (pd3dDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_9_3)
	{
		ssDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		ssDesc.MaxAnisotropy = 0;
	}
	hr = pd3dDevice->CreateSamplerState(&ssDesc, &m_pSamplerStateUI11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pSamplerStateUI11, "CDXUTDialogResourceManager");

	// Создание шрифта и текстур объектов
	/*for (int i = 0; i < m_FontCache.GetSize(); i++)
	{
		hr = CreateFont11(i);
		if (FAILED(hr))
			return hr;
	}*/

	for (int i = 0; i < m_TextureCache.GetSize(); i++)
	{
		hr = CreateTexture11(i);
		if (FAILED(hr))
			return hr;
	}

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pInputLayout11, "CDXUTDialogResourceManager");

	_RELEASE(pVSBlob);
	_RELEASE(pPSBlob);
	_RELEASE(pPSUntexBlob);

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = sizeof(DXUT_SCREEN_VERTEX_10) * 4;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bufDesc, NULL, &m_pVBScreenQuad11);
	if (FAILED(hr))
		return hr;
	DXUT_SetDebugName(m_pVBScreenQuad11, "CDXUTDialogResourceManager");

	// Инициализирую d3d11 шрифт
	InitFont11(pd3dDevice, m_pInputLayout11);

	return S_OK;
}

HRESULT DialogResourceManager::OnD3D11ResizedSwapChain(ID3D11Device * pd3dDevice, const DXGI_SURFACE_DESC * pBackBufferSurfaceDesc)
{
	HRESULT hr = S_OK;

	m_nBackBufferWidth = pBackBufferSurfaceDesc->Width;
	m_nBackBufferHeight = pBackBufferSurfaceDesc->Height;

	return hr;
}

void DialogResourceManager::OnD3D11ReleasingSwapChain()
{
}

void DialogResourceManager::OnD3D11DestroyDevice()
{
	for (int i = 0; i < m_TextureCache.GetSize(); i++)
	{
		DXUTTextureNode *pTextureNode = m_TextureCache.GetAt(i);
		_RELEASE(pTextureNode->pTexResView11);
		_RELEASE(pTextureNode->pTexture11);
	}

	_RELEASE(m_pVBScreenQuad11);
	_RELEASE(m_pSpriteBuffer11);
	m_SpriteBufferBytes11 = 0;
	_RELEASE(m_pInputLayout11);

	_RELEASE(m_pVSRenderUI11);
	_RELEASE(m_pPSRenderUI11);
	_RELEASE(m_pPSRenderUIUntex11);

	_RELEASE(m_pDepthStencilStateUI11);
	_RELEASE(m_pRasterizerStateUI11);
	_RELEASE(m_pBlendStateUI11);
	_RELEASE(m_pSamplerStateUI11);

	_RELEASE(m_pDepthStencilStateStored11);
	_RELEASE(m_pRasterizerStateStored11);
	_RELEASE(m_pBlendStateStored11);
	_RELEASE(m_pSamplerStateStored11);

	EndFont11();
}

void DialogResourceManager::StoreD3D11State(ID3D11DeviceContext * pd3dImmediateContext)
{
	pd3dImmediateContext->OMGetDepthStencilState(&m_pDepthStencilStateStored11, &m_StencilRefStored11);
	pd3dImmediateContext->RSGetState(&m_pRasterizerStateStored11);
	pd3dImmediateContext->OMGetBlendState(&m_pBlendStateStored11, m_BlendFactorStored11, &m_SampleMaskStored11);
	pd3dImmediateContext->PSGetSamplers(0, 1, &m_pSamplerStateStored11);
}

void DialogResourceManager::RestoreD3D11State(ID3D11DeviceContext * pd3dImmediateContext)
{
	pd3dImmediateContext->OMSetDepthStencilState(m_pDepthStencilStateStored11, m_StencilRefStored11);
	pd3dImmediateContext->RSSetState(m_pRasterizerStateStored11);
	pd3dImmediateContext->OMSetBlendState(m_pBlendStateStored11, m_BlendFactorStored11, m_SampleMaskStored11);
	pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerStateStored11);

	_RELEASE(m_pDepthStencilStateStored11);
	_RELEASE(m_pRasterizerStateStored11);
	_RELEASE(m_pBlendStateStored11);
	_RELEASE(m_pSamplerStateStored11);
}

void DialogResourceManager::ApplyRenderUI11(ID3D11DeviceContext * pd3dImmediateContext)
{
	pd3dImmediateContext->VSSetShader(m_pVSRenderUI11, NULL, 0);
	pd3dImmediateContext->HSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->DSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(m_pPSRenderUI11, NULL, 0);

	pd3dImmediateContext->OMSetDepthStencilState(m_pDepthStencilStateUI11, 0);
	pd3dImmediateContext->RSSetState(m_pRasterizerStateUI11);
	float BlendFactor[4] = { 0,0,0,0 };
	pd3dImmediateContext->OMSetBlendState(m_pBlendStateUI11, BlendFactor, 0xFFFFFFFF);
	pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerStateUI11);
}

void DialogResourceManager::ApplyRenderUIUntex11(ID3D11DeviceContext * pd3dImmediateContext)
{
	pd3dImmediateContext->VSSetShader(m_pVSRenderUI11, NULL, 0);
	pd3dImmediateContext->HSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->DSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	pd3dImmediateContext->PSSetShader(m_pPSRenderUIUntex11, NULL, 0);

	pd3dImmediateContext->OMSetDepthStencilState(m_pDepthStencilStateUI11, 0);
	pd3dImmediateContext->RSSetState(m_pRasterizerStateUI11);
	float BlendFactor[4] = { 0, 0, 0, 0 };
	pd3dImmediateContext->OMSetBlendState(m_pBlendStateUI11, BlendFactor, 0xFFFFFFFF);
	pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerStateUI11);
}

void DialogResourceManager::BeginSprites11()
{
	m_SpriteVertices.Reset();
}

void DialogResourceManager::EndSprites11(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pd3dImmediateContext)
{
	UINT SpriteDataBytes = m_SpriteVertices.GetSize() * sizeof(DXUTSpriteVertex);
	if (m_SpriteBufferBytes11 < SpriteDataBytes)
	{
		_RELEASE(m_pSpriteBuffer11);
		m_SpriteBufferBytes11 = SpriteDataBytes;

		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.ByteWidth = m_SpriteBufferBytes11;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;

		pd3dDevice->CreateBuffer(&BufferDesc, NULL, &m_pSpriteBuffer11);
		DXUT_SetDebugName(m_pSpriteBuffer11, "DialogResourceManager");
	}

	D3D11_BOX destRegion;
	destRegion.left = 0;
	destRegion.right = SpriteDataBytes;
	destRegion.top = 0;
	destRegion.bottom = 1;
	destRegion.front = 0;
	destRegion.back = 1;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (S_OK == pd3dImmediateContext->Map(m_pSpriteBuffer11, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource))
	{
		CopyMemory(MappedResource.pData, (void*)m_SpriteVertices.GetData(), SpriteDataBytes);
		pd3dImmediateContext->Unmap(m_pSpriteBuffer11, 0);
	}

	UINT Stride = sizeof(DXUTSpriteVertex);
	UINT Offset = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_pSpriteBuffer11, &Stride, &Offset);
	pd3dImmediateContext->IASetInputLayout(m_pInputLayout11);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dImmediateContext->Draw(m_SpriteVertices.GetSize(), 0);

	m_SpriteVertices.Reset();
}

int DialogResourceManager::AddTexture(LPCWSTR strFilename)
{
	// Смотрю существует ли уже такая текстура
	for (int i = 0; i < m_TextureCache.GetSize(); i++)
	{
		DXUTTextureNode *pTextureNode = m_TextureCache.GetAt(i);
		size_t nLen = 0;
		nLen = wcsnlen(strFilename, MAX_PATH);
		if (pTextureNode->bFileSource && 0 == _wcsnicmp(pTextureNode->strFilename, strFilename, nLen))
		{
			return i;
		}
	}

	// Добавляю новую текстуру и пытаюсь создать ее
	DXUTTextureNode *pNewTextureNode = new DXUTTextureNode;
	if (pNewTextureNode == NULL)
		return -1;

	ZeroMemory(pNewTextureNode, sizeof(DXUTTextureNode));
	pNewTextureNode->bFileSource = true;
	wcscpy_s(pNewTextureNode->strFilename, MAX_PATH, strFilename);

	m_TextureCache.Add(pNewTextureNode);
	
	int iTexture = m_TextureCache.GetSize() - 1;

	return iTexture;
}

int DialogResourceManager::AddTexture(LPCWSTR strResourceName, HMODULE hResourceModule)
{
	// Смотрим существует ли такая текстура
	for (int i = 0; i < m_TextureCache.GetSize(); i++)
	{
		DXUTTextureNode *pTextureNode = m_TextureCache.GetAt(i);
		if (!pTextureNode->bFileSource && pTextureNode->hResourceModule == hResourceModule)
		{
			if (IS_INTRESOURCE(strResourceName))
			{
				if ((INT_PTR)strResourceName == pTextureNode->nResourceID)
					return i;
			}
			else
			{
				size_t nLen = 0;
				nLen = wcsnlen(strResourceName, MAX_PATH);
				if (0 == _wcsnicmp(pTextureNode->strFilename, strResourceName, nLen))
					return i;
			}
		}
	}

	// Добавляю новую текстуру и пытаюсь создать ее
	DXUTTextureNode *pNewTextureNode = new DXUTTextureNode;
	if (pNewTextureNode == NULL)
		return -1;

	ZeroMemory(pNewTextureNode, sizeof(DXUTTextureNode));
	pNewTextureNode->hResourceModule = hResourceModule;
	if (IS_INTRESOURCE(strResourceName))
	{
		pNewTextureNode->nResourceID = (int)(size_t)strResourceName;
	}
	else
	{
		pNewTextureNode->nResourceID = 0;
		wcscpy_s(pNewTextureNode->strFilename, MAX_PATH, strResourceName);
	}

	m_TextureCache.Add(pNewTextureNode);

	int iTexture = m_TextureCache.GetSize() - 1;

	return iTexture;
}

bool DialogResourceManager::RegisterDialog(Dialog * pDialog)
{
	// Проверяю зарегистрирован ли уже такой dialog
	for (int i = 0; i < m_Dialogs.GetSize(); ++i)
		if (m_Dialogs.GetAt(i) == pDialog)
			return true;

	// Добавляю новую запись с диалогом
	if (FAILED(m_Dialogs.Add(pDialog)))
		return false;

	// Записываю следующий и предыдущий элемент
	if (m_Dialogs.GetSize() > 1)
		m_Dialogs[m_Dialogs.GetSize() - 2]->SetNextDialog(pDialog);
	m_Dialogs[m_Dialogs.GetSize() - 1]->SetNextDialog(m_Dialogs[0]);

	return false;
}

void DialogResourceManager::UnregisterDialog(Dialog * pDialog)
{
	// Поиск элемента диалог 
	for (int i = 0; i < m_Dialogs.GetSize(); ++i)
	{
		if (m_Dialogs.GetAt(i) == pDialog)
		{
			m_Dialogs.Remove(i);
			if (m_Dialogs.GetSize() > 0)
			{
				int l, r;

				if (0 == i)
					l = m_Dialogs.GetSize() - 1;
				else
					l = i - 1;

				if (m_Dialogs.GetSize() == i)
					r = 0;
				else
					r = i;

				m_Dialogs[l]->SetNextDialog(m_Dialogs[r]);
			}
			return;
		}
	}
}

void DialogResourceManager::EnableKeyboardInputForAllDialogs()
{
	// Включаю обработку клавиатуры для всех зарегистрированных элементов
	for (int i = 0; i < m_Dialogs.GetSize(); ++i)
		m_Dialogs[i]->EnableKeyboardInput(true);
}

HRESULT DialogResourceManager::CreateFont11(UINT iFont)
{
	return S_OK;
}

HRESULT DialogResourceManager::CreateTexture11(UINT iTexture)
{
	HRESULT hr = S_OK;

	DXUTTextureNode *pTextureNode = m_TextureCache.GetAt(iTexture);

	if (!pTextureNode->bFileSource)
	{
		if (pTextureNode->nResourceID == 0xFFFF && pTextureNode->hResourceModule == (HMODULE)0xFFFF)
		{
			hr = DXUTCreateGUITextureFromInternalArray11(m_pd3d11Device, &pTextureNode->pTexture11, NULL);
			if (FAILED(hr))
				return hr;
			DXUT_SetDebugName(pTextureNode->pTexture11, "DXUT GUI Texture");
		}
	}
	
	D3D11_TEXTURE2D_DESC desc;
	pTextureNode->pTexture11->GetDesc(&desc);
	pTextureNode->dwWidth = desc.Width;
	pTextureNode->dwHeight = desc.Height;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = MAKE_SRGB(desc.Format);
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = m_pd3d11Device->CreateShaderResourceView(pTextureNode->pTexture11, &srvDesc, &pTextureNode->pTexResView11);
	DXUT_SetDebugName(pTextureNode->pTexResView11, "DXUT GUI Texture");

	return hr;
}