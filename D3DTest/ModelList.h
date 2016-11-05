#pragma once

#include "D3D11_Framework.h"

using namespace D3D11Framework;

class ModelList
{
public:
	ModelList();

	bool Init(int numModels, bool bWhile);
	void Close();

	int GetModelCount();
	void GetData(int index, float &posX, float &posY, float &posZ);

private:
	int m_modelCount;
	XMFLOAT3 *m_ModelInfoList;
};