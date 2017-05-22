#include "player.h"
#include "../Global/LibRtmp/log.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

RTMPPlayer::RTMPPlayer()
{
	m_rtmp = NULL;
	m_current_tag = NULL;
	m_flv_parser = NULL;
}

RTMPPlayer::~RTMPPlayer()
{

}

int RTMPPlayer::Init(const char* rtmp_url, const char* file_path)
{
	sock_init();
	RTMP_LogLevel loglevel = RTMP_LOGDEBUG;
	RTMP_LogSetLevel(loglevel);
	//FILE* log_file = fopen("player.log", "wb+");
	//RTMP_LogSetOutput(log_file);

	m_rtmp = PILI_RTMP_Alloc();
	PILI_RTMP_Init(m_rtmp);
	m_rtmp->Link.timeout = 30;//connection timeout 30s
	PILI_RTMP_SetupURL(m_rtmp, rtmp_url, NULL);
	PILI_RTMP_SetBufferMS(m_rtmp, 10*1000);//buffer 10s
	if (!PILI_RTMP_Connect(m_rtmp, NULL, NULL)){
		RTMP_Log(RTMP_LOGERROR, "connect server error");
		return -1;
	}
	if (!PILI_RTMP_ConnectStream(m_rtmp, 0, NULL)){
		RTMP_Log(RTMP_LOGERROR, "connect stream error");
		return -1;
	}
	PILI_RTMP_SetChunkSize(m_rtmp, NULL, 4096);

	m_current_tag = (FLVTAG*)malloc(sizeof(FLVTAG));
	memset(m_current_tag, 0, sizeof(FLVTAG));
	m_flv_parser = new FlvParser();
	m_flv_parser->Init();
	m_flv_parser->CreateFlvFile(file_path);
	return 0;
}

int RTMPPlayer::Play(void)
{
	PILI_RTMPPacket* packet = NULL;
	packet = (PILI_RTMPPacket*)malloc(sizeof(PILI_RTMPPacket));
	memset(packet, 0, sizeof(PILI_RTMPPacket));
	PILI_RTMPPacket_Reset(packet);

	while (PILI_RTMP_GetNextMediaPacket(m_rtmp, packet))
	{
		m_current_tag->type = packet->m_packetType;
		m_current_tag->datalength = packet->m_nBodySize;
		m_current_tag->timestamp = packet->m_nTimeStamp;
		m_current_tag->streamid = 0;//always zero
		m_current_tag->buffer = packet->m_body;
		m_current_tag->taglength = packet->m_nBodySize + FLV_TAG_HEADER_LENGTH;

		m_flv_parser->WriteTag(m_current_tag);
		PILI_RTMPPacket_Free(packet);
		PILI_RTMPPacket_Reset(packet);
	}
	free(packet); packet = NULL;
	RTMP_Log(RTMP_LOGINFO, "Player Over");
	return 0;
}

int RTMPPlayer::Destroy(void)
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
	if (m_current_tag){
		free(m_current_tag);
	}
	sock_cleanup();
	return 0;
}

int RTMPPlayer::sock_init(void)
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

int RTMPPlayer::sock_cleanup(void)
{
#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}
