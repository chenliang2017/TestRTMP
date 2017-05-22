#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include "../Global/LibRtmp/rtmp.h"
#include "../Global/FlvParser/FLvParser.h"

class RTMPPlayer
{
public:
	RTMPPlayer();
	~RTMPPlayer();
public:
	int Init(const char* rtmp_url, const char* file_path);
	int Play(void);
	int Destroy(void);
private:
	int sock_init(void);
	int sock_cleanup(void);
private:
	PILI_RTMP* m_rtmp;
	FLVTAG* m_current_tag;
	FlvParser* m_flv_parser;
};
#endif
