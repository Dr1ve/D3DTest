#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "NetworkClass.h"
#include <iostream>

NetworkClass::NetworkClass()
{
	m_online = false;
}

NetworkClass::NetworkClass(const NetworkClass& other)
{
}

NetworkClass::~NetworkClass()
{
}

bool NetworkClass::Initialize()
{
	bool result;

	// Инициализируем winsock
	result = InitializeWinsock();
	if (!result)
	{
		return false;
	}

	return true;
}

void NetworkClass::Shutdown()
{
	// Если мы не в сети то не надо завершать соединение))
	if (!m_online)
	{
		return;
	}

	// Отправляю серверу сообщение что клиент завершает соединение
	SendDisconnectMessage();

	// Устанавливаю флаг что мы не в сети
	m_online = false;

	// Жду завершения сетевого потока
	while (m_threadActive)
	{
	}

	// Закрываю сокет
	closesocket(m_clientSocket);

	// Завершаю winsock
	ShutdownWinsock();

	return;
}

void NetworkClass::Frame()
{
	ProcessLatency();

	return;
}

bool NetworkClass::InitializeWinsock()
{
	unsigned short version;
	WSADATA wsaData;
	int error;
	unsigned long bufferSize;
	WSAPROTOCOL_INFOW* protocolBuffer;
	int protocols[2];

	// Создание макроса для проверки версии
	version = MAKEWORD(2, 0);

	// Получаю данные , можем ли мы использовать данную версию winsock
	error = WSAStartup(version, &wsaData);
	if (error != 0)
	{
		return false;
	}

	// Проверка что файл dll является версией 2.0
	if ((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 0))
	{
		return false;
	}

	// Запрашиваем размер доступного буфера для протокола
	WSAEnumProtocols(NULL, NULL, &bufferSize);

	// Создаю буфер для структуры информации протокола
	protocolBuffer = new WSAPROTOCOL_INFOW[bufferSize];
	if (!protocolBuffer)
	{
		return false;
	}

	// Создаю список протоколов
	protocols[0] = IPPROTO_TCP;
	protocols[1] = IPPROTO_UDP;

	// Получаю информацию о доступных транспортных протоколах, если нет то ошибка, мы не сможем использовать сокет
	error = WSAEnumProtocols(protocols, protocolBuffer, &bufferSize);
	if (error == SOCKET_ERROR)
	{
		return false;
	}

	// Очищаю буфер протоколов
	delete[] protocolBuffer;
	protocolBuffer = 0;

	return true;
}

void NetworkClass::ShutdownWinsock()
{
	WSACleanup();
	return;
}

bool NetworkClass::ConnectToServer(char *ipAddress, unsigned short portNumber)
{
	unsigned long setting, inetAddress, startTime, threadId;
	MSG_GENERIC_DATA connectMessage, *message;
	int error, bytesSent, bytesRead;
	bool done, gotId;
	char recvBuffer[4096];
	HANDLE threadHandle;

	// Создаю UDP socket
	m_clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	// Отключаю блокировку функций I/O
	setting = 1;
	error = ioctlsocket(m_clientSocket, FIONBIO, &setting);
	if (error == SOCKET_ERROR)
	{
		return false;
	}

	// Сохраняю вес структуры адреса сервера
	m_addressLength = sizeof(m_serverAddress);

	// Устанавливаю адрес сервера куда будем слать данные
	inetAddress = inet_addr(ipAddress);
	memset((char*)&m_serverAddress, 0, m_addressLength);
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(portNumber);
	m_serverAddress.sin_addr.S_un.S_addr = inetAddress;

	// Настройки сообщений о соединении для отправки на сервер
	connectMessage.type = MSG_CONNECT;
	connectMessage.toId = ID_SERVER;
	connectMessage.fromId = ID_UNKNOWN;

	// Отправка сообщения на сервер
	bytesSent = sendto(m_clientSocket, (char*)&connectMessage, sizeof(MSG_GENERIC_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent < 0)
	{
		return false;
	}

	// Запишим время когда был послан пакет
	startTime = timeGetTime();

	done = false;
	gotId = false;

	while (!done)
	{
		// Проверка ответа от сервера
		bytesRead = recvfrom(m_clientSocket, recvBuffer, 4096, 0, (struct sockaddr*)&m_serverAddress, &m_addressLength);
		if (bytesRead > 0)
		{
			done = true;
			gotId = true;
		}

		// Выходим из цикла если он уже работает более 2 секунд.
		if (timeGetTime() > (startTime + 2000))
		{
			done = true;
			gotId = false;
		}
	}

	// Если не получили ответ с ид за 2 секунды от сервера значит он не отвечает
	if (!gotId)
	{
		return false;
	}

	message = (MSG_GENERIC_DATA*)recvBuffer;

	// Проверяю что это сообщение с ид
	if (message->type != MSG_ID)
	{
		return false;
	}

	if (message->toId == -1)
	{
		return false;
	}

	// Сохраняю ид клиента для дальшейшей связи 
	m_idNumber = message->toId;

	// Устанавливаю что клиент теперь в сети
	m_online = true;

	// Запускаю новый поток общения с сервером
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetworkReadFunction, (void*)this, 0, &threadId);
	if (threadHandle == NULL)
	{
		return false;
	}

	// Инициализирую переменную сетевой задержки
	m_latency = 0;
	m_pingTime = timeGetTime();

	return true;
}

int NetworkClass::GetLantency()
{
	return m_latency;
}

void NetworkClass::SetThredActive()
{
	m_threadActive = true;
	return;
}

void NetworkClass::SetThredInactive()
{
	m_threadActive = false;
	return;
}

bool NetworkClass::Online()
{
	return m_online;
}

SOCKET NetworkClass::GetClientSocket()
{
	return m_clientSocket;
}

void NetworkReadFunction(void* ptr)
{
	NetworkClass *networkClassPtr;
	struct sockaddr_in serverAddress;
	int addressLength;
	int bytesRead;
	char recvBuffer[4096];

	// Получаю адресс объекта
	networkClassPtr = (NetworkClass*)ptr;

	// Уведомляю главный поток что этот поток теперь активный
	networkClassPtr->SetThredActive();

	addressLength = sizeof(serverAddress);

	// Цикл чтения сообщений от сервера
	while (networkClassPtr->Online())
	{
		// Проверка есть ли сообщение от сервера
		bytesRead = recvfrom(networkClassPtr->GetClientSocket(), recvBuffer, 4069, 0, (struct sockaddr*)&serverAddress, &addressLength);
		if (bytesRead > 0)
		{
			networkClassPtr->ReadNetworkMessage(recvBuffer);
		}
	}

	// Сообщаю главному потоку, что поток больше не активен
	networkClassPtr->SetThredInactive();

	return;
}

void NetworkClass::ReadNetworkMessage(char *recvBuffer)
{
	MSG_GENERIC_DATA *message;

	message = (MSG_GENERIC_DATA*)recvBuffer;

	switch (message->type)
	{
	case MSG_PING:
			HandlePingMessage();
			break;
	default:
		break;
	}

	return;
}

void NetworkClass::ProcessLatency()
{
	// Если прошло 5 секунд отправляю сообщение на сервер пинга)
	if (timeGetTime() >= (m_pingTime + 5000))
	{
		m_pingTime = timeGetTime();
		SendPing();
	}

	return;
}

void NetworkClass::SendPing()
{
	MSG_GENERIC_DATA message;
	int bytesSent;

	// Создаю сообщение с пингом
	message.type = MSG_PING;
	message.toId = ID_SERVER;
	message.fromId = m_idNumber;

	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_GENERIC_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_GENERIC_DATA))
	{
		// Если не отправились данные на сервер
	}

	return;
}

void NetworkClass::HandlePingMessage()
{
	m_latency = timeGetTime() - m_pingTime;

	return;
}

void NetworkClass::SendDisconnectMessage()
{
	MSG_GENERIC_DATA message;
	int bytesSent;

	// Создаю сообщение с дисконнектом
	message.type = MSG_DISCONNECT;
	message.toId = ID_SERVER;
	message.fromId = m_idNumber;

	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_GENERIC_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_GENERIC_DATA))
	{
		// Если не отправились данные на сервер
	}

	return;
}