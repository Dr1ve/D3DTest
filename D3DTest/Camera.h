#pragma once

#include "D3D11_Framework.h"

using namespace D3D11Framework;

class Camera
{
public:
	Camera();

	void Render(float time);
	
	void TurnLeft(bool keydown);
	void TurnRight(bool keydown);
	void TurnForward(bool keydown);
	void TurnBack(bool keydown);

	void Strafe(float d);
	void Walk(float d);
	void RotateY(float angle);
	void SetPitch(float angle);

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	CXMMATRIX GetViewMatrix();

private:
	XMMATRIX m_viewMatrix;
	XMFLOAT3 m_pos;
	XMFLOAT3 m_rot;

	float m_frameTime;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_forwardTurnSpeed, m_backTurnSpeed;

	XMFLOAT3 m_right;
	XMFLOAT3 m_up;
	XMFLOAT3 m_look;
};