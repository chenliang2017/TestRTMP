#include"FlvParser.h"

FlvParser::FlvParser()
{
	m_fp = NULL;
	m_file_size = 0;
	m_current_tag = NULL;
	m_log = NULL;
}

FlvParser::~FlvParser()
{
	this->destroy();
}

int FlvParser::Init(void)
{
	m_log = fopen("output_log.txt", "wb+");
	if (!m_current_tag){
		m_current_tag = (FLVTAG*)malloc(sizeof(FLVTAG));
		memset(m_current_tag, 0, sizeof(FLVTAG));
		m_current_tag->buffer = (char*)malloc(MAX_TAG_SIZE);
	}
	return 1;
}

int FlvParser::OpenRead(const char* filename)
{
	m_fp = fopen(filename, "rb+");
	if (!m_fp){
		fprintf(m_log,"open flv failed.\n");
		return -1;
	}
	fseek(m_fp, 0, SEEK_END);
	m_file_size = ftell(m_fp);

	fseek(m_fp, FLV_HEADER_LENGTH + PREVIOUS_TAG_SIZE, SEEK_SET);
	return 0;
}

int FlvParser::CreateFlvFile(const char* filename)
{
	const char FlvHeader[] = { 'F', 'L', 'V', 0x01, 0x05, 0x00, 0x00, 0x00, 0x09 };
	m_fp = fopen(filename, "wb+");
	if (!m_fp){
		printf("open flv for write failed.\n");
		return -1;
	}
	char previous_tag_size_0th[4] = { 0 };
	fwrite(FlvHeader, 1, 9, m_fp);//flv header
	fwrite(previous_tag_size_0th, 1, 4, m_fp);//previous_tag_size_#0
	return 0;
}

FLVTAG* FlvParser::ReadTag(void)
{
	if (!read_u8(&(m_current_tag->type), m_fp)){
		fprintf(m_log, "read type error.\n");
		return NULL;
	}
	if (!read_u24(&(m_current_tag->datalength), m_fp)){
		fprintf(m_log, "read datalength error.\n");
		return NULL;
	}
	if (!read_utime(&(m_current_tag->timestamp), m_fp)){
		fprintf(m_log, "read timestamp error.\n");
		return NULL;
	}
	if (!read_u24(&(m_current_tag->streamid), m_fp)){
		fprintf(m_log, "read streamid error.\n");
		return NULL;
	}
	if (m_current_tag->datalength > MAX_TAG_SIZE){
		m_current_tag->buffer = (char*)realloc(m_current_tag->buffer, m_current_tag->datalength);
	}
	if (fread(m_current_tag->buffer, 1, m_current_tag->datalength, m_fp) != m_current_tag->datalength){
		fprintf(m_log, "read tag error\n");
		return NULL;
	}
	if (!read_u32(&(m_current_tag->taglength), m_fp)){
		fprintf(m_log, "read previous tag size error.\n");
		return NULL;
	}

	if (m_current_tag->type == RTMP_PACKET_TYPE_VIDEO){
		parser_video_tag_data();
	} else if (m_current_tag->type == RTMP_PACKET_TYPE_AUDIO){
		//parser_audio_tag_data();
	} else if (m_current_tag->type == RTMP_PACKET_TYPE_INFO){
		//parser_metadata(void);
	} else{
		fprintf(m_log, "unknow type:%d", m_current_tag->type);
	}

	return m_current_tag;
}

int FlvParser::WriteTag(FLVTAG* tag)
{
	if ((tag->datalength + 11) != tag->taglength){
		fprintf(m_log, "tag format error.\n");
		return -1;
	}
	write_u8(tag->type, m_fp);
	write_u24(tag->datalength, m_fp);
	write_utime(tag->timestamp, m_fp);
	write_u24(tag->streamid, m_fp);
	fwrite(tag->buffer, 1, tag->datalength, m_fp);
	write_u32(tag->taglength, m_fp);
	fflush(m_fp);
	return 0;
}

int FlvParser::Close(void)
{
	this->destroy();
	return 0;
}

long FlvParser::GetFileSize(void)
{
	return m_file_size;
}

int FlvParser::read_u8(int* u8, FILE* fp)
{
	if (fread(u8, 1, 1, fp) != 1)
		return 0;
	return 1;
}

int FlvParser::read_u16(int* u16, FILE* fp)
{
	if (fread(u16, 2, 1, fp) != 1)
		return 0;
	*u16 = HTON16(*u16);
	return 1;
}

int FlvParser::read_u24(int* u24, FILE* fp)
{
	if (fread(u24, 3, 1, fp) != 1)
		return 0;
	*u24 = HTON24(*u24);
	return 1;
}

int FlvParser::read_u32(int* u32, FILE* fp)
{
	if (fread(u32, 4, 1, fp) != 1)
		return 0;
	*u32 = HTON32(*u32);
	return 1;
}

int FlvParser::peek_u8(int* u8, FILE* fp)
{
	if (fread(u8, 1, 1, fp) != 1)
		return 0;
	fseek(fp, -1, SEEK_CUR);
	return 1;
}

int FlvParser::read_utime(int* utime, FILE* fp)
{
	if (fread(utime, 4, 1, fp) != 1)
		return 0;
	*utime = HTONTIME(*utime);
	return 1;
}

int FlvParser::write_u8(int u8, FILE* fp)
{
	if (fwrite(&u8, 1, 1, fp) != 1)
		return 0;
	return 1;
}
int FlvParser::write_u16(int u16, FILE* fp)
{
	u16 = HTON16(u16);
	if (fwrite(&u16, 2, 1, fp) != 1)
		return 0;
	return 1;
}
int FlvParser::write_u24(int u24, FILE* fp)
{
	u24 = HTON24(u24);
	if (fwrite(&u24, 3, 1, fp) != 1)
		return 0;
	return 1;
}
int FlvParser::write_u32(int u32, FILE* fp)
{
	u32 = HTON32(u32);
	if (fwrite(&u32, 4, 1, fp) != 1)
		return 0;
	return 1;
}
int FlvParser::write_utime(int utime, FILE* fp)
{
	utime = HTONTIME(utime);
	if (fwrite(&utime, 4, 1, fp) != 1)
		return 0;
	return 1;
}

void FlvParser::destroy()
{
	if (m_fp){
		fclose(m_fp);
		m_fp = NULL;
	}
	if (m_log){
		fclose(m_log);
		m_log = NULL;
	}
	if (m_current_tag){
		free(m_current_tag->buffer);
		free(m_current_tag);
		m_current_tag = NULL;
	}
}

void FlvParser::parser_metadata(void)
{
	fprintf(m_log, "not support metadata parser now.\n");
	return;
}

/*
------------------------------------------------------------------------------------------
Frame Type(4 bit) |  CodeID(4 bit) |  AVCPackettype(1 byte) if CodeID=7  |  Composition
------------------------------------------------------------------------------------------
time(cont)  (3 byte)  if CodeID=7  |  Data   |
----------------------------------------------------------------------
*/
void FlvParser::parser_video_tag_data(void)
{
	if (!m_current_tag){
		fprintf(m_log,"null tag.\n");
		return;
	}
	if (m_current_tag->type != RTMP_PACKET_TYPE_VIDEO){
		fprintf(m_log, "not video tag.\n");
		return;
	}

	m_current_tag->code_type = (m_current_tag->buffer)[0] & 0x0f;
	m_current_tag->frame_type = ((m_current_tag->buffer)[0] >> 4) & 0x0f;
	m_current_tag->packet_type = (m_current_tag->buffer)[1];

	int con_time = 0;
	char* p = (char*)&con_time;
	//p[2] = m_current_tag->buffer[2];
	//p[1] = m_current_tag->buffer[3];
	p[0] = m_current_tag->buffer[4];
	m_current_tag->composition_time = con_time;
	fprintf(m_log, "type:%d,datalength:%6d,timestamp:%6d,streamid:%6d,taglength:%6d,frame_type:%d,code_type:%d,packet_type:%d,flv_seq:%d\n", 
		m_current_tag->type,m_current_tag->datalength,m_current_tag->timestamp,m_current_tag->streamid,
		m_current_tag->taglength,m_current_tag->frame_type,m_current_tag->code_type,m_current_tag->packet_type,
		m_current_tag->composition_time);
	return;
}

void FlvParser::parser_audio_tag_data(void)
{
	fprintf(m_log, "not support audio data parser now.\n");
	return;
}