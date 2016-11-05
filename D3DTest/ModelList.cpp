#include "ModelList.h"

ModelList::ModelList()
{
	m_ModelInfoList = nullptr;
}

bool ModelList::Init(int numModels, bool bWhile)
{
	m_modelCount = numModels;

	m_ModelInfoList = new XMFLOAT3[m_modelCount];
	if (!m_ModelInfoList)
		return false;

	/*srand((unsigned int)time(NULL));

	for (int i = 0; i<m_modelCount; i++)
	{
		m_ModelInfoList[i].x = (((float)rand() - (float)rand()) / RAND_MAX) * 20.0f;
		m_ModelInfoList[i].y = 2.5f;//(((float)rand() - (float)rand()) / RAND_MAX) * 20.0f;
		m_ModelInfoList[i].z = ((((float)rand() - (float)rand()) / RAND_MAX) * 20.0f) + 5.0f;
	}*/
	float r = 2.8f;
	if (!bWhile)
	{
		m_ModelInfoList[0].x = -r*7.0f;
		m_ModelInfoList[0].y = 2.5f;
		m_ModelInfoList[0].z = r*7.0f;

		m_ModelInfoList[1].x = -r*3.0f;
		m_ModelInfoList[1].y = 2.5f;
		m_ModelInfoList[1].z = r*7.0f;

		m_ModelInfoList[2].x = r*1.0f;
		m_ModelInfoList[2].y = 2.5f;
		m_ModelInfoList[2].z = r*7.0f;

		m_ModelInfoList[3].x = r*5.0f;
		m_ModelInfoList[3].y = 2.5f;
		m_ModelInfoList[3].z = r*7.0f;

		m_ModelInfoList[4].x = -r*5.0f;
		m_ModelInfoList[4].y = 2.5f;
		m_ModelInfoList[4].z = r*5.0f;

		m_ModelInfoList[5].x = -r*1.0f;
		m_ModelInfoList[5].y = 2.5f;
		m_ModelInfoList[5].z = r*5.0f;

		m_ModelInfoList[6].x = r*3.0f;
		m_ModelInfoList[6].y = 2.5f;
		m_ModelInfoList[6].z = r*5.0f;

		m_ModelInfoList[7].x = r*7.0f;
		m_ModelInfoList[7].y = 2.5f;
		m_ModelInfoList[7].z = r*5.0f;

		m_ModelInfoList[8].x = -r*7.0f;
		m_ModelInfoList[8].y = 2.5f;
		m_ModelInfoList[8].z = r*3.0f;

		m_ModelInfoList[9].x = -r*3.0f;
		m_ModelInfoList[9].y = 2.5f;
		m_ModelInfoList[9].z = r*3.0f;

		m_ModelInfoList[10].x = r*1.0f;
		m_ModelInfoList[10].y = 2.5f;
		m_ModelInfoList[10].z = r*3.0f;

		m_ModelInfoList[11].x = r*5.0f;
		m_ModelInfoList[11].y = 2.5f;
		m_ModelInfoList[11].z = r*3.0f;
	}
	else
	{
		m_ModelInfoList[0].x = r*7.0f;
		m_ModelInfoList[0].y = 2.5f;
		m_ModelInfoList[0].z = -r*7.0f;

		m_ModelInfoList[1].x = r*3.0f;
		m_ModelInfoList[1].y = 2.5f;
		m_ModelInfoList[1].z = -r*7.0f;

		m_ModelInfoList[2].x = -r*1.0f;
		m_ModelInfoList[2].y = 2.5f;
		m_ModelInfoList[2].z = -r*7.0f;

		m_ModelInfoList[3].x = -r*5.0f;
		m_ModelInfoList[3].y = 2.5f;
		m_ModelInfoList[3].z = -r*7.0f;

		m_ModelInfoList[4].x = r*5.0f;
		m_ModelInfoList[4].y = 2.5f;
		m_ModelInfoList[4].z = -r*5.0f;

		m_ModelInfoList[5].x = r*1.0f;
		m_ModelInfoList[5].y = 2.5f;
		m_ModelInfoList[5].z = -r*5.0f;

		m_ModelInfoList[6].x = -r*3.0f;
		m_ModelInfoList[6].y = 2.5f;
		m_ModelInfoList[6].z = -r*5.0f;

		m_ModelInfoList[7].x = -r*7.0f;
		m_ModelInfoList[7].y = 2.5f;
		m_ModelInfoList[7].z = -r*5.0f;

		m_ModelInfoList[8].x = r*7.0f;
		m_ModelInfoList[8].y = 2.5f;
		m_ModelInfoList[8].z = -r*3.0f;

		m_ModelInfoList[9].x = r*3.0f;
		m_ModelInfoList[9].y = 2.5f;
		m_ModelInfoList[9].z = -r*3.0f;

		m_ModelInfoList[10].x = -r*1.0f;
		m_ModelInfoList[10].y = 2.5f;
		m_ModelInfoList[10].z = -r*3.0f;

		m_ModelInfoList[11].x = -r*5.0f;
		m_ModelInfoList[11].y = 2.5f;
		m_ModelInfoList[11].z = -r*3.0f;
	}
	
	
	return true;
}

void ModelList::Close()
{
	_DELETE_ARRAY(m_ModelInfoList);
}

int ModelList::GetModelCount()
{
	return m_modelCount;
}

void ModelList::GetData(int index, float &positionX, float &positionY, float &positionZ)
{
	positionX = m_ModelInfoList[index].x;
	positionY = m_ModelInfoList[index].y;
	positionZ = m_ModelInfoList[index].z;
}