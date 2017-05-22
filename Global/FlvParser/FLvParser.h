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
	CodecVideoAVC = 7,   //H264编码格式
};

enum CodecVideoAVCPacketType
{
	CodecVideoAVCTypeReserved = 3,			//set to the max value to reserved,for array map.
	CodecVideoAVCTypeSequenceHeader = 0,	//AVC sequence header
	CodecVideoAVCTypeNALU = 1,				//AVC NALU
	CodecVideoAVCTypeSequenceHeaderEOF = 2,	//AVC end of sequence(do not need)
};

typedef struct _FlvTag{
	int type;		//类型，1字节,0x08音频,0x09视频
	int datalength; //数据长度，3字节
	int timestamp;	//时间戳，4字节
	int streamid;	//流ID，3字节
	char*	 buffer;	//存储音视频数据信息
	int taglength;      //该帧总长度，4字节
	int frame_type;		//帧类型
	int code_type;		//编码类型
	int packet_type;	//tag数据类型
	int composition_time;	//这里用来区分四合一中的tag属于哪一个flv
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
