#include <stdio.h>
#include <stdlib.h>
#include "../Global/FlvParser/FLvParser.h"

#define MAX_FILE_PATH 256

int main(int argc, char** argv)
{
	char file_path[MAX_FILE_PATH] = { 0 };
	printf("Please input file path:");
	scanf_s("%s", file_path, MAX_FILE_PATH);

	FlvParser* reader = new FlvParser();
	reader->Init();
	reader->OpenRead(file_path);
	long file_size = reader->GetFileSize();
	printf("reader size:%d\n", file_size);

	FlvParser* writer = new FlvParser;
	writer->Init();
	writer->CreateFlvFile("new.flv");

	FLVTAG* tag = NULL;
	while (1){
		tag = reader->ReadTag();
		if (!tag){
			printf("read over.\n");
			break;
		}
		
		if (tag->type == RTMP_PACKET_TYPE_AUDIO)
			continue;

		//int i = 1;
		//if (tag->type == RTMP_PACKET_TYPE_VIDEO){//video tag write 4 times
		//	i = 4;
		//}
		//for (; i > 0; i--){
		//	tag->buffer[2] = 0;
		//	tag->buffer[3] = 0;
		//	tag->buffer[4] = i;
		//	writer->WriteTag(tag);
		//}
		writer->WriteTag(tag);
	}
	reader->Close(); 
	delete reader; 
	reader = NULL;
	writer->Close();
	delete writer; writer = NULL;

	printf("printf any key to continuen.\n");
	getchar();
	getchar();
	return 0;
}