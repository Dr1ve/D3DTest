#include "MyRender.h"

std::wstring IntToStr(int i)
{
	wchar_t str[255];
	swprintf(str, 255, L"%d", i);
	return str;
}

MyRender::MyRender()
{
	m_pMesh = nullptr;
	m_img = nullptr;
	m_font = nullptr;
	text1 = nullptr;
	text2 = nullptr;
	text3 = nullptr;
	lantency = nullptr;
	m_leftcam = m_rightcam = m_forwardcam = m_backcam = false;
	str = L"����� � ����������";
	m_CloseApp = true;
	m_Network = nullptr;
}

bool MyRender::Init()
{
	m_font = new BitmapFont(this);
	if (!m_font->Init("font.fnt"))
	{
		Log::Get()->Err("�� ���� ���������������� �����, myrender.cpp");
		return false;
	}	

	text1 = new Text(this, m_font);
	text1->Init(L"FPS",false,10);
	
	text2 = new Text(this, m_font);
	text2->Init(L"", false, str_max_len);

	if (NETWORK_ENABLED)
	{
		lantency = new Text(this, m_font);
		lantency->Init(L"", false, 10);
	}
	
	//m_input->Init(this, text2);

	text3 = new Text(this, m_font);
	text3->Init(L"������ ���",false,30);

	m_cam.SetPosition(0.0f, 5.0f, 0.0f);
	
	m_WhiteFishka = new StaticMesh(this);
	if (!m_WhiteFishka->Init(L"Objects/BlackFishka.obj", L"Objects/WhiteFishka.bmp"))
	{
		Log::Get()->Err("�� ���� ���������������� ������ � ���������, myrender.cpp");
		return false;
	}

	m_BlackFishka = new StaticMesh(this);
	if (!m_BlackFishka->Init(L"Objects/BlackFishka.obj", L"Objects/BlackFishka.bmp"))
	{
		Log::Get()->Err("�� ���� ���������������� ������ � ���������, myrender.cpp");
		return false;
	}

	m_CheesBoard = new StaticMesh(this);
	if (!m_CheesBoard->Init(L"Objects/CheesBoard.obj", L"Objects/CheesBoard.bmp"))
	{
		Log::Get()->Err("�� ���� ���������������� ������ � ���������, myrender.cpp");
		return false;
	}

	/*m_img = new Image(this);
	if (!m_img->Init(L"Texture.png", 256, 256))
	{
		Log::Get()->Err("�� ���� ���������������� ��������, myrender.cpp");
		return false;
	}*/
		
	if (!m_listWhiteFishka.Init(12, true))
	{
		Log::Get()->Err("�� ���� ������� ������ � ������������ ��������, myrender.cpp");
		return false;
	}

	if (!m_listBlackFishka.Init(12, false))
	{
		Log::Get()->Err("�� ���� ������� ������ � ������������ ��������, myrender.cpp");
		return false;
	}
		
	if (!m_timer.Init())
	{
		Log::Get()->Err("�� ���� ���������������� ������, myrender.cpp");
		return false;
	}
		
	fps.Init();

	// ���� network ������� �� ������ � ������������� ������ network
	if (NETWORK_ENABLED)
	{
		m_Network = new NetworkClass;
		if (!m_Network)
		{
			Log::Get()->Err("�� ���� ������� ������ Network, myrender.cpp");
			return false;
		}

		if (!m_Network->Initialize())
		{
			MessageBox(m_hwnd, L"�� ���� ���������������� network ������.", L"������", MB_OK);
			Log::Get()->Err("�� ���� ���������������� network ������, myrender.cpp");
			return false;
		}

		if (!m_Network->ConnectToServer(SERVER_ADDRESS, SERVER_PORT))
		{
			MessageBox(m_hwnd, L"�� ���� ����������� � ��������.", L"������", MB_OK);
			Log::Get()->Err("�� ���� ����������� � ��������, myrender.cpp");
			return false;
		}
	}

	return true;
}

bool MyRender::Draw()
{
	// ���� ��� ������� network �� �������� frame.
	if (NETWORK_ENABLED)
	{
		m_Network->Frame();
	}

	TurnZBufferOn();
	TurnOffAlphaBlending();
	m_timer.Frame();
	m_cam.TurnLeft(m_leftcam);
	m_cam.TurnRight(m_rightcam);
	m_cam.TurnForward(m_forwardcam);
	m_cam.TurnBack(m_backcam);
	m_cam.Render(m_timer.GetTime());

	XMMATRIX viewMatrix = m_cam.GetViewMatrix();

	m_frustum.ConstructFrustum(1000.0f, m_Projection, viewMatrix);

	int modelCountW = m_listWhiteFishka.GetModelCount();
	int modelCountB = m_listBlackFishka.GetModelCount();
	int renderCount = 0;

	m_frametime = m_timer.GetTime();
	static float rot = 0.0f;
	rot += m_frametime * 0.005f;
	
	if (rot > 6.26f)
		rot = 0.0f;

	/*static float rot = 0.0f;
	rot += 0.005f;
	if (rot > 6.26f) rot = 0.0f;*/

	for (int i = 0; i < modelCountW; i++)
	{
		float x, y, z;
		m_listWhiteFishka.GetData(i, x, y, z);

		//bool renderModel = m_frustum.CheckSphere(x, y, z, 10.0f);
		bool renderModel = m_frustum.CheckPoint(x, y, z);
		//bool renderModel = m_frustum.CheckCube(x, y, z, 1.0f);
		//bool renderModel = m_frustum.CheckRectangle(x, y, z, 1.0f, 1.0f, 1.0f);
		if (renderModel)
		{
			m_WhiteFishka->Identity();
			//m_pMesh->Rotate(-rot, 0.0f, 1.0f, 0.0f);
			m_WhiteFishka->Scale(10.0f, 10.0f, 10.0f);
			m_WhiteFishka->Translate(x, y, z);
			m_WhiteFishka->Draw(viewMatrix);

			renderCount++;
		}
	}

	for (int i = 0; i < modelCountB; i++)
	{
		float x, y, z;
		m_listBlackFishka.GetData(i, x, y, z);

		//bool renderModel = m_frustum.CheckSphere(x, y, z, 10.0f);
		bool renderModel = m_frustum.CheckPoint(x, y, z);
		//bool renderModel = m_frustum.CheckCube(x, y, z, 1.0f);
		//bool renderModel = m_frustum.CheckRectangle(x, y, z, 1.0f, 1.0f, 1.0f);
		if (renderModel)
		{
			m_BlackFishka->Identity();
			//m_pMesh->Rotate(-rot, 0.0f, 1.0f, 0.0f);
			m_BlackFishka->Scale(10.0f, 10.0f, 10.0f);
			m_BlackFishka->Translate(x, y, z);
			m_BlackFishka->Draw(viewMatrix);

			renderCount++;
		}
	}

	/*����� ��������� �����*/
	m_CheesBoard->Identity();
	m_CheesBoard->Scale(10.0f, 10.0f, 10.0f);
	m_CheesBoard->Translate(0.0f, 0.0f, 0.0f);
	m_CheesBoard->Draw(viewMatrix);

	/*m_pMesh->Identity();
	m_pMesh->Rotate(-rot, 0.0f, 1.0f, 0.0f);
	m_pMesh->Translate(10.0f, 0.0f, 0.0f);
	m_pMesh->Draw(m_View);*/

	/*m_pMesh->Identity();
	m_pMesh->Rotate(-rot, 0.0f, 1.0f, 0.0f);
	m_pMesh->Translate(1.5f, 0.0f, 0.0f);
	m_pMesh->Draw(m_View);*/

	TurnZBufferOff();
	//m_img->Draw(100, 100);

	TurnOnAlphaBlending();
	fps.Frame();
	std::wstring textfps = L"FPS: " + IntToStr(fps.GetFps());
	std::wstring text = L"����� �� ������: " + IntToStr(renderCount);

	text1->SetText(textfps);
	text1->Draw(1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	text2->SetText(str);
	text2->Draw(1.0f, 0.0f, 1.0f, 0.0f, 30.0f);
	text3->SetText(text);
	text3->Draw(0.0f, 1.0f, 1.0f, 0.0f, 60.0f);
	if (NETWORK_ENABLED)
	{
		std::wstring t = L"Ping: " + IntToStr(m_Network->GetLantency());
		lantency->SetText(t);
		lantency->Draw(0.0f, 1.0f, 0.0f, 200.0f, 0.0f);
	}

	TurnOffAlphaBlending();
	TurnZBufferOn();

	return m_CloseApp;
}

void MyRender::Close()
{
	// ���� network ��� ������� �� ������ ������ network.
	if (NETWORK_ENABLED)
	{
		if (m_Network)
		{
			m_Network->Shutdown();
			delete m_Network;
			m_Network = nullptr;
		}
	}

	_CLOSE(text1);
	_CLOSE(text2);
	_CLOSE(text3);
	if (NETWORK_ENABLED)
	{
		_CLOSE(lantency);
	}
	_CLOSE(m_font);
	_CLOSE(m_pMesh);
	_CLOSE(m_img);
	m_listWhiteFishka.Close();
	m_listBlackFishka.Close();
}