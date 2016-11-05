#include "Misc.h"

template<typename TYPE> HRESULT CGrowableArray<TYPE>::SetSizeInternal(int nNewMaxSize)
{
	if (nNewMaxSize < 0 || (nNewMaxSize > INT_MAX / sizeof(TYPE)))
	{
		assert(false);
		return E_INVALIDARG;
	}

	if (nNewMaxSize == 0)
	{
		if (m_pData)
		{
			free(m_pData);
			m_pData = NULL;
		}

		m_nMaxSize = 0;
		m_nSize = 0;
	}
	else if (m_pData == NULL || nNewMaxSize > m_nMaxSize)
	{
		int nGrowBy = (m_nMaxSize == 0) ? 16 : m_nMaxSize;

		if ((UINT)m_nMaxSize + (UINT)nGrowBy > (UINT)INT_MAX)
			nGrowBy = INT_MAX - m_nMaxSize;

		nNewMaxSize = __max(nNewMaxSize, m_nMaxSize + nGrowBy);

		if (sizeof(TYPE) > UINT_MAX / (UINT)nNewMaxSize)
			return E_INVALIDARG;

		TYPE *pDataNew = (TYPE*)realloc(m_pData, nNewMaxSize * sizeof(TYPE));
		if (pDataNew == NULL)
			return E_INVALIDARG;

		m_pData = pDataNew;
		m_nMaxSize = nNewMaxSize;
	}

	return S_OK;
}

template<typename TYPE> HRESULT CGrowableArray<TYPE>::SetSize(int nNewMaxSize)
{
	int nOldSize = m_nSize;

	if (nOldSize > nNewMaxSize)
	{
		assert(m_pData);
		if (m_pData)
		{
			// Удаление элементов
			for (int i = nNewMaxSize; i < nOldSize; ++i)
				m_pData[i].~TYPE();
		}
	}

	HRESULT hr = SetSizeInternal(nNewMaxSize);

	if (nOldSize < nNewMaxSize)
	{
		assert(m_pData);
		if (m_pData)
		{
			// Добавляю элементы
			for (int i = nOldSize; i < nNewMaxSize; ++i)
				::new (&m_pData[i])TYPE;
		}
	}

	return hr;
}

template<typename TYPE> HRESULT CGrowableArray<TYPE>::Add(const TYPE &value)
{
	HRESULT hr;
	if (FAILED(hr = SetSizeInternal(m_nSize + 1)))
		return hr;

	assert(m_pData != NULL);

	::new (&m_pData[m_nSize])TYPE;

	m_pData[m_nSize] = value;
	++m_nSize;

	return S_OK;
}

template<typename TYPE> HRESULT CGrowableArray<TYPE>::Insert(int nIndex, const TYPE &value)
{
	HRESULT hr;
	// Проверка на нормальность аргументов
	if (nIndex < 0 || nIndex > m_nSize)
	{
		assert(false);
		return E_INVALIDARG;
	}

	if (FAILED(hr = SetSizeInternal(m_nSize + 1)))
		return hr;

	MoveMemory(&m_pData[nIndex + 1], &m_pData[nIndex], sizeof(TYPE)*(m_nSize - nIndex));

	::new (&m_pData[nIndex])TYPE;

	m_pData[nIndex] = value;
	++m_nSize;

	return S_OK;
}

template<typename TYPE> HRESULT CGrowableArray<TYPE>::SetAt(int nIndex, const TYPE &value)
{
	// Проверка на нормальность аргументов
	if (nIndex < 0 || nIndex >= m_nSize)
	{
		assert(false);
		return E_INVALIDARG;
	}

	m_pData[nIndex] = value;
	return S_OK;
}

template<typename TYPE> int CGrowableArray<TYPE>::IndexOf(const TYPE &value, int iStart, int nNumElement)
{
	// Проверка на нормальность аргументов
	if (iStart < 0 || iStart >= m_nSize || nNumElements < 0 || iStart + nNumElement > m_nSize)
	{
		assert(false);
		return -1;
	}

	// Поиск
	for (int i = iStart; i < (iStart + nNumElements); i++)
	{
		if (value == m_pData[i])
			return i;
	}

	// Не нашел
	return -1;
}

template<typename TYPE> int CGrowableArray<TYPE>::LastIndexOf(const TYPE &value, int nIndex, int nNumElements)
{
	// Проверка на нормальность аргументов
	if (nIndex < = || nIndex >= m_nSize || nNumElements < 0 || nIndex - nNumElements < 0)
	{
		assert(false);
		return -1;
	}

	// Поиск
	for (int i = nIndex; i > (nIndex - nNumElements); i--)
	{
		if (value == m_pData[i])
			return i;
	}

	// Не нашел
	return -1;
}

template<typename TYPE> HRESULT CGrowableArray<TYPE>::Remove(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nSize)
	{
		assert(false);
		return E_INVALIDARG;
	}

	// Вызываю деструкт класса элемента
	m_pData[nIndex].~TYPE();

	MoveMemory(&m_pData[nIndex], &m_pData[nIndex + 1], sizeof(TYPE)*(m_nSize - (nIndex + 1)));
	--m_nSize;

	return S_OK;
}

DXGI_FORMAT MAKE_SRGB(DXGI_FORMAT format)
{
	/*if (!DXUTIsInGammaCorrectMode())
		return format;*/

	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	};

	return format;
}