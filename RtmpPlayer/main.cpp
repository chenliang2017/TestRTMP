#include <stdio.h>
#include "player.h"
#include "../Global/LibRtmp/log.h"

#define MAX_STRING_LENGTH 256

int main(int arcg, char** argv)
{
	char rtmp_url[MAX_STRING_LENGTH] = { 0 };
	char file_path[MAX_STRING_LENGTH] = { 0 };

	//printf("Please input rtmp url:");
	//scanf_s("%s", rtmp_url, MAX_STRING_LENGTH);
	//printf("Please input file path:");
	//scanf_s("%s", file_path, MAX_STRING_LENGTH);

	RTMPPlayer* rtmp_player = new RTMPPlayer();
	if (rtmp_player->Init("rtmp://127.0.0.1/live/livestream", "59.flv") == -1){
		RTMP_Log(RTMP_LOGERROR, "init error");
		goto ERROR;
	}
	rtmp_player->Play();

ERROR:
	rtmp_player->Destroy();
	delete rtmp_player, rtmp_player = NULL;

	getchar();
	getchar();
	printf("printf any key to continue");
	return 0;
}