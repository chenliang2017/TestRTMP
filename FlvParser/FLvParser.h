#ifndef _FLV_PARSER_H_
#define _FLV_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HTON16(x)   ((x>>8&0xff)|(x<<8&0xff00))
#define HTON24(x)   ((x>>16&0xff)|(x<<16&0xff0000)|(x&0xff00))
#define HTON32(x)   ((x>>24&0xff)|(x>>8&0xff00)|(x<<8&0xff0000)|(x<<24&0xff000000))
#define HTONTIME(x) ((x>>16&0xff)|(x<<16&0xff0000)|(x&0xff00)|(x&0xff000000))

#define MAX_TAG_SIZE (1024*1024)
#define FLV_HEADER_LENGTH 9
#define PREVIOUS_TAG_SIZE 4
#define FLV_TAG_HEADER_LENGTH 11

#define RTMP_PACKET_TYPE_AUDIO 0x08
#define RTMP_PACKET_TYPE_VIDEO 0x09
#define RTMP_PACKET_TYPE_INFO  0x12

enum FrameType
{
	KEYFRAME = 1,	//for avc,a seekable frame
	INTERFRAME,		//for avc,a non-seekable frame
	DISPOSABLEINTERFRAME,	//h263 only
	GENERATEDKEYFRAME,		//reserved for server use only
	VIDEOINFOFRAME,
};

enum CodecVideo
{
	// set to the zero to reserved, for array map.
	CodecVideoReserved = 0,
	CodecVideoReserved1 = 1,
	CodecVideoReserved2 = 9,

	// for user to disable video, for example, use pure audio hls.
	CodecVideoDisabled = 8,

	CodecVideoSorensonH263 = 2,
	CodecVideoScreenVideo = 3,
	CodecVideoOn2VP6 = 4,
	CodecVideoOn2VP6WithAlphaChannel = 5,
	CodecVideoScreenVideoVersion2 = 6,
	CodecVideoAVC = 7,   //H264�����ʽ
};

enum CodecVideoAVCPacketType
{
	CodecVideoAVCTypeReserved = 3,			//set to the max value to reserved,for array map.
	CodecVideoAVCTypeSequenceHeader = 0,	//AVC sequence header
	CodecVideoAVCTypeNALU = 1,				//AVC NALU
	CodecVideoAVCTypeSequenceHeaderEOF = 2,	//AVC end of sequence(do not need)
};

typedef struct _FlvTag{
	int type;		//���ͣ�1�ֽ�,0x08��Ƶ,0x09��Ƶ
	int datalength; //���ݳ��ȣ�3�ֽ�
	int timestamp;	//ʱ�����4�ֽ�
	int streamid;	//��ID��3�ֽ�
	char*	 buffer;	//�洢����Ƶ������Ϣ
	int taglength;      //��֡�ܳ��ȣ�4�ֽ�
	int frame_type;		//֡����
	int code_type;		//��������
	int packet_type;	//tag��������
	int composition_time;	//�������������ĺ�һ�е�tag������һ��flv
}FLVTAG;

class FlvParser
{
public:
	FlvParser();
	~FlvParser();
public:
	int Init(void);
	int OpenRead(const char* filename);
	int CreateFlvFile(const char* filename);
	FLVTAG* ReadTag(void);
	int WriteTag(FLVTAG* tag);
	int Close(void);
	long GetFileSize(void);
private:
	int read_u8(int* u8, FILE* fp);
	int read_u16(int* u16, FILE* fp);
	int read_u24(int* u24, FILE* fp);
	int read_u32(int* u32, FILE* fp);
	int peek_u8(int* u8, FILE* fp);
	int read_utime(int* utime, FILE* fp);

	int write_u8(int u8, FILE* fp);
	int write_u16(int u16, FILE* fp);
	int write_u24(int u24, FILE* fp);
	int write_u32(int u32, FILE* fp);
	int write_utime(int utime, FILE* fp);

	void parser_metadata(void);
	void parser_video_tag_data(void);
	void parser_audio_tag_data(void);

	void destroy();
private:
	FILE* m_fp;
	long m_file_size;
	FLVTAG* m_current_tag;
	FILE* m_log;
};
#endif
