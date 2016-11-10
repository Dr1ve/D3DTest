#pragma once

#include "D3D11_Framework.h"
#include "Fps.h"
#include "ModelList.h"
#include "Timer.h"
#include "Camera.h"
#include "Frustum.h"
#include "NetworkClass.h"
#include "DialogResourceManager.h"
#include "Dialog.h"


using namespace D3D11Framework;

class MyRender : public Render
{
public:
	MyRender();
	bool Init();
	bool Draw();
	void Close();

private:
	friend class MyInput;

	ModelList m_listBlackFishka;
	ModelList m_listWhiteFishka;
	Camera m_cam;
	Frustum m_frustum;
	Timer m_timer;

	BitmapFont *m_font;
	Text *text1;
	Text *text2;
	Text *text3;
	Text *lantency;

	Image *m_img;
	StaticMesh *m_pMesh;
	StaticMesh *m_CheesBoard;
	StaticMesh *m_WhiteFishka;
	StaticMesh *m_BlackFishka;

	std::wstring str;
	const int str_max_len = 30;

	bool m_leftcam;
	bool m_rightcam;
	bool m_forwardcam;
	bool m_backcam;

	float m_frametime;
	
	FpsClass fps;
	NetworkClass *m_Network;

	bool m_CloseApp;

	Dialog m_HUD;
	DialogResourceManager m_DialogResourceManager;
};