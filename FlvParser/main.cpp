#include <stdio.h>
#include <stdlib.h>
#include "FLvParser.h"

#define MAX_FILE_PATH 256

int main(int argc, char** argv)
{
	//char file_path[MAX_FILE_PATH] = { 0 };
	//printf("Please input file path:");
	//scanf_s("%s", file_path, MAX_FILE_PATH);

	FlvParser* reader = new FlvParser();
	reader->Init();
	reader->OpenRead("0508.flv");
	long file_size = reader->GetFileSize();
	printf("reader size:%d", file_size);

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
	}
	reader->Close(); 
	delete reader; 
	reader = NULL;
	writer->Close();
	delete writer; writer = NULL;

	getchar();
	getchar();
	return 0;
}