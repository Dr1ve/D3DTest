#pragma once

#define ID_SERVER -2
#define ID_UNKNOWN -1

#define MSG_CONNECT 1000
#define MSG_ID 1001
#define MSG_PING 1002
#define MSG_DISCONNECT 1003

typedef struct
{
	short type;
	short toId;
	short fromId;
} MSG_GENERIC_DATA;