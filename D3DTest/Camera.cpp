#include "Camera.h"

Camera::Camera()
{
	m_frameTime = 0.0f;
	m_leftTurnSpeed = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_forwardTurnSpeed = 0.0f;
	m_backTurnSpeed = 0.0f;

	m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

void Camera::Render(float time)
{
	m_frameTime = time;
	/*
	float radians = m_rot.y * 0.0174532925f;
	float lAtx = sinf(radians) + m_pos.x;
	float lAty = m_pos.y;
	float lAtz = cosf(radians) + m_pos.z;

	XMVECTOR camPos = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.0f);
	//XMVECTOR camLookAt = XMVectorSet(lAtx, lAty, lAtz, 0.0f);
	XMVECTOR camLookAt = XMVectorSet(m_look.x, m_pos.y, m_look.z, 0.0f);
	//XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(m_up.x, m_up.y, m_up.z, 0.0f);

	m_viewMatrix = XMMatrixLookAtLH(camPos, camLookAt, camUp);*/

	XMVECTOR R = XMLoadFloat3(&m_right);
	XMVECTOR U = XMLoadFloat3(&m_up);
	XMVECTOR L = XMLoadFloat3(&m_look);
	XMVECTOR P = XMLoadFloat3(&m_pos);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
	XMStoreFloat3(&m_look, L);

	m_viewMatrix(0, 0) = m_right.x;
	m_viewMatrix(1, 0) = m_right.y;
	m_viewMatrix(2, 0) = m_right.z;
	m_viewMatrix(3, 0) = x;

	m_viewMatrix(0, 1) = m_up.x;
	m_viewMatrix(1, 1) = m_up.y;
	m_viewMatrix(2, 1) = m_up.z;
	m_viewMatrix(3, 1) = y;

	m_viewMatrix(0, 2) = m_look.x;
	m_viewMatrix(1, 2) = m_look.y;
	m_viewMatrix(2, 2) = m_look.z;
	m_viewMatrix(3, 2) = z;

	m_viewMatrix(0, 3) = 0.0f;
	m_viewMatrix(1, 3) = 0.0f;
	m_viewMatrix(2, 3) = 0.0f;
	m_viewMatrix(3, 3) = 1.0f;
}

void Camera::TurnLeft(bool keydown)
{
	if (keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if (m_leftTurnSpeed > (m_frameTime * 0.15f))
			m_leftTurnSpeed = m_frameTime * 0.15f;
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime* 0.05f;

		if (m_leftTurnSpeed < 0.0f)
			m_leftTurnSpeed = 0.0f;
	}

	RotateY(-m_leftTurnSpeed);
}

void Camera::TurnRight(bool keydown)
{
	if (keydown)
	{
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if (m_rightTurnSpeed > (m_frameTime * 0.15f))
			m_rightTurnSpeed = m_frameTime * 0.15f;
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime* 0.05f;

		if (m_rightTurnSpeed < 0.0f)
			m_rightTurnSpeed = 0.0f;
	}

	RotateY(m_rightTurnSpeed);
}

void Camera::TurnForward(bool keydown)
{
	if (keydown)
	{
		m_forwardTurnSpeed += m_frameTime * 0.0001f;

		if (m_forwardTurnSpeed > (m_frameTime * 0.015f))
			m_forwardTurnSpeed = m_frameTime * 0.015f;
	}
	else
	{
		m_forwardTurnSpeed -= m_frameTime * 0.005f;

		if (m_forwardTurnSpeed < 0.0f)
			m_forwardTurnSpeed = 0.0f;
	}
	
	Walk(m_forwardTurnSpeed);
}

void Camera::TurnBack(bool keydown)
{
	if (keydown)
	{
		m_backTurnSpeed += m_frameTime * 0.0001f;

		if (m_backTurnSpeed > (m_frameTime * 0.015f))
			m_backTurnSpeed = m_frameTime * 0.015f;
	}
	else
	{
		m_backTurnSpeed -= m_frameTime * 0.005f;

		if (m_backTurnSpeed < 0.0f)
			m_backTurnSpeed = 0.0f;
	}

	Walk(-m_backTurnSpeed);
}

void Camera::Strafe(float d)
{

}

void Camera::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_pos);
	XMStoreFloat3(&m_pos, XMVectorMultiplyAdd(s, l, p));
	float r = 25.0f;
	if (m_pos.x > r)
		m_pos.x = r;
	if (m_pos.x < -r)
		m_pos.x = -r;
	if (m_pos.z > r)
		m_pos.z = r;
	if (m_pos.z < -r)
		m_pos.z = -r;
}

void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle*0.01f);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
}

void Camera::SetPitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
}

void Camera::SetPosition(float x, float y, float z)
{
	m_pos = XMFLOAT3(x, y, z);
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rot = XMFLOAT3(x, y, z);
}

CXMMATRIX Camera::GetViewMatrix()
{
	return m_viewMatrix;
}