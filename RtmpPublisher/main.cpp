#include <stdio.h>
#include "publish.h"
#include "../Global/LibRtmp/log.h"

#define MAX_STRING_LENGTH 256

int main(int argc, char** argv)
{
	char rtmp_url[MAX_STRING_LENGTH] = { 0 };
	char file_path[MAX_STRING_LENGTH] = { 0 };

	printf("Please input rtmp url:");
	scanf_s("%s", rtmp_url, MAX_STRING_LENGTH);
	printf("Please input file path:");
	scanf_s("%s", file_path, MAX_STRING_LENGTH);

	PublishRTMP* pub_rtmp = new PublishRTMP();
	if (pub_rtmp->Init(rtmp_url, file_path) == -1){
		RTMP_Log(RTMP_LOGERROR,"init error");
		goto ERROR;
	}
	if (pub_rtmp->Publish() == -1){
		RTMP_Log(RTMP_LOGERROR,"publish error");
		goto ERROR;
	}

ERROR:
	pub_rtmp->Destroy();
	//delete pub_rtmp, pub_rtmp = NULL;

	getchar();
	getchar();
	printf("publish over\n");
	return 0;
}