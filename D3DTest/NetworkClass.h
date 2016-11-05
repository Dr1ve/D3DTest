#pragma once

const bool NETWORK_ENABLED = false;
//static char SERVER_ADDRESS[] = "94.253.109.241";
//static char SERVER_ADDRESS[] = "192.168.1.34";
static char SERVER_ADDRESS[] = "127.0.0.1";
const unsigned short SERVER_PORT = 7531;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include <WinSock2.h>
#include <mmsystem.h>
#include <atomic>

#include "NetworkMessage.h"

class NetworkClass
{
public:
	NetworkClass();
	NetworkClass(const NetworkClass&);
	~NetworkClass();

	bool Initialize();
	void Shutdown();
	void Frame();

	int GetLantency();
	bool ConnectToServer(char*, unsigned short);
	
	void SetThredActive();
	void SetThredInactive();
	bool Online();
	SOCKET GetClientSocket();
	void ReadNetworkMessage(char*);

private:
	bool InitializeWinsock();
	void ShutdownWinsock();
	void ProcessLatency();
	void SendPing();
	void HandlePingMessage();
	void SendDisconnectMessage();

private:
	SOCKET m_clientSocket;
	int m_addressLength;
	struct sockaddr_in m_serverAddress;
	short m_idNumber;
	bool m_online;
	std::atomic<bool> m_threadActive;
	int m_latency;
	unsigned long m_pingTime;
};

void NetworkReadFunction(void*);