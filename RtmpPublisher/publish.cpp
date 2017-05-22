#include "publish.h"
#include "../Global/LibRtmp/log.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <time.h>

PublishRTMP::PublishRTMP()
{
	m_rtmp = NULL;
	m_current_tag = NULL;
	m_flv_parser = NULL;
}

PublishRTMP::~PublishRTMP()
{
}

int PublishRTMP::Init(const char* rtmp_url, const char* file_path)
{
	sock_init();
	RTMP_LogLevel loglevel = RTMP_LOGDEBUG;
	RTMP_LogSetLevel(loglevel);
	FILE* log_file = fopen("publish.log", "wb+");
	RTMP_LogSetOutput(log_file);

	m_rtmp = PILI_RTMP_Alloc();
	PILI_RTMP_Init(m_rtmp);
	m_rtmp->Link.timeout = 5;//connection timeout 5s
	PILI_RTMP_SetupURL(m_rtmp,rtmp_url,NULL);
	PILI_RTMP_EnableWrite(m_rtmp);

	if (!PILI_RTMP_Connect(m_rtmp, NULL, NULL)){
		RTMP_Log(RTMP_LOGERROR, "connect server error");
		return -1;
	}
	PILI_RTMP_SetChunkSize(m_rtmp, NULL, 4096);
	if (!PILI_RTMP_ConnectStream(m_rtmp, 0, NULL)){
		RTMP_Log(RTMP_LOGERROR, "connect stream error");
		return -1;
	}

	m_flv_parser = new FlvParser();
	m_flv_parser->Init();
	m_flv_parser->OpenRead(file_path);
	return 0;
}

int PublishRTMP::Publish(void)
{
	PILI_RTMPPacket packet;
	PILI_RTMPPacket_Alloc(&packet, 1024 * 1064);
	PILI_RTMPPacket_Reset(&packet);
	packet.m_hasAbsTimestamp = 0;
	packet.m_nChannel = 0x14;
	packet.m_nInfoField2 = 0;

	long perframetime = 0;
	long start = clock() - 1000;
	long timestamp = 0;
	while (1)
	{
		//if ((clock() - start) < perframetime){
		//	//RTMP_Log(RTMP_LOGDEBUG, "TimeStamp:%d ms", perframetime);
		//	Sleep(500);
		//	continue;
		//}
		Sleep(40);

		m_current_tag = m_flv_parser->ReadTag();
		if (!m_current_tag){
			RTMP_Log(RTMP_LOGERROR, "file read error");
			PILI_RTMPPacket_Free(&packet);
			return -1;
		}
		if (m_current_tag->type != 0x08 && m_current_tag->type != 0x09){
			//continue;
		}

		memcpy(packet.m_body, m_current_tag->buffer, m_current_tag->datalength);
		packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
		packet.m_nTimeStamp = timestamp;//m_current_tag->timestamp;
		packet.m_packetType = m_current_tag->type;
		packet.m_nBodySize = m_current_tag->datalength;

		if (!PILI_RTMP_IsConnected(m_rtmp)){
			RTMP_Log(RTMP_LOGERROR, "Rtmp is not connect");
			PILI_RTMPPacket_Free(&packet);
			return -1;
		}
		if (!PILI_RTMP_SendPacket(m_rtmp, &packet, 0, NULL)){
			RTMP_Log(RTMP_LOGERROR,"Send Err");
			PILI_RTMPPacket_Free(&packet);
			return -1;
		}
		perframetime = m_current_tag->timestamp;
		timestamp += 40;

	}
	RTMP_Log(RTMP_LOGINFO, "Send Data Over");
	PILI_RTMPPacket_Free(&packet);
	return 0;
}

int PublishRTMP::Destroy(void)
{
	if (m_rtmp){
		PILI_RTMP_Close(m_rtmp, NULL);
		PILI_RTMP_Free(m_rtmp);
		m_rtmp = NULL;
	}
	if (m_flv_parser){
		m_flv_parser->Close();
		delete m_flv_parser, m_flv_parser = NULL;
	}
	m_current_tag = NULL;
	sock_cleanup();
	return 0;
}

int PublishRTMP::sock_init(void)
{
#ifdef WIN32
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsaData) != 0){
		return -1;
	}
#endif
	return 0;
}

int PublishRTMP::sock_cleanup(void)
{
#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}
