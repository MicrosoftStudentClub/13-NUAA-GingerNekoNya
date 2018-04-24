/*
    Classic AMR2WAV Converter
    Codename Cattleya
    Developed by Pichu
    Jan. 18th, '19

    ClassAMR2WAV Decode Header File

    ref: 
    http://blog.csdn.net/dinggo/article/details/2002298
    https://www.cnblogs.com/wangguchangqing/p/5957531.html

*/ 

#ifndef CLASSIC_AMR_CODEC_H
#define CLASSIC_AMR_CODEC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interf_rom.h"

#define AMR_MAGIC_NUMBER "#!AMR\n"
#define WAV_FRAME_SIZE 160
#define MAX_AMR_FRAME_SIZE 32
#define AMR_FRAME_COUNT_PER_SECOND 50

/*
    WAV使用的RIFF CHUNK定义
    ref: https://www.cnblogs.com/wangguchangqing/p/5957531.html
    struct chunk {
        uint32_t id;   // 块标志
        uint32_t size; // 块大小
        uint8_t data[size]; // 块数据
    };
    一个WAV文件，首先是一个RIFF chunk；RIFF chunk又包含有Format chunk，Data chunk以及可选的Fact chunk。

    RIFF chunk
    id：FOURCC 值为'R' 'I' 'F' 'F'
    size：其data字段中数据的大小 字节数
    data：包含其他的chunk

    Format chunk
    id：FOURCC 值为 'f' 'm' 't' ' '
    size：数据字段包含数据的大小。如无扩展块，则值为16；有扩展块，则值为= 16 + 2字节扩展块长度 + 扩展块长度或者值为18（只有扩展块的长度为2字节，值为0）
    data：存放音频格式、声道数、采样率等信息
    format_tag：2字节，表示音频数据的格式。如值为1，表示使用PCM格式。
    channels：2字节，声道数。值为1则为单声道，为2则是双声道。
    samples_per_sec：采样率，主要有22.05KHz，44.1kHz和48KHz。
    avg_bytes_per_sec：音频的码率，每秒播放的字节数。samples_per_sec * channels * bits_per_sample / 8，可以估算出使用缓冲区的大小
    block_align：数据块对齐单位，一次采样的大小，值为声道数 * 量化位数 / 8，在播放时需要一次处理多个该值大小的字节数据。
    bits_per_sample：音频sample的量化位数，有16位，24位和32位等。
    cbSize：扩展区的长度
    扩展块内容：22字节，具体介绍，后面补充。
*/

typedef struct {
    char chunk_ID[4];
    int chunk_size;
} ChunkHeader;  // XCHUNKHEADER

typedef struct {
    char riff_ID[4];
    int riff_size;
    char riff_format[4];
} RIFFChunk;    // RIFFHEADER

typedef struct {
    short format_tag;
    short channels;
    int samples_per_sec;
    int avg_bytes_per_sec;
    short block_align;
    short bits_per_sample;
} WAVBlock; // WAVEFORMAT

typedef struct {
    short format_tag;
    short channels;
    int samples_per_sec;
    int avg_bytes_per_sec;
    short block_align;
    short bits_per_sample;
    short ext_size;
} WAVExtBlock; // WAVEFORMATX

typedef struct {
    char format_ID[4];
    int format_size;
    WAVBlock wave;
} WAVChunk; // FMTBLOCK

int amrEncodeMode[] = {4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200}; // amr编码方式

void classicAMR2WAV(const char *AMR_file_name, const char *WAV_file_name);
#endif
