/*
    Classic AMR2WAV Converter
    Codename Cattleya
    Developed by Pichu
    Jan. 18th, '19

    ClassAMR2WAV C File

    ref: 
    http://blog.csdn.net/dinggo/article/details/2002298
    https://www.cnblogs.com/wangguchangqing/p/5957531.html

*/ 

#include "ClassicAMRCodec.h"
#include "ClassicAMRCodecDPL.h"
#include "ClassicAMRCodecDPP.h"
/**
 * 写 WAV 文件头
 * 单声道 8KHz 16bit 
 * @param   wav文件、int帧
 * @return  void
 */
void WAVFileHeaderWriter(FILE *file_wav, int Frame) {
	RIFFChunk riff; 
	ChunkHeader chunk;
	WAVExtBlock wav_fmt_ext;
	
    char tag[10] = "";

    // 1. 写 RIFF chunk 头
    strcpy(tag, "RIFF");
    memcpy(riff.riff_ID, tag, 4);                       // 写 RIFF chunk
    riff.riff_size = 4                                  // WAV
                     + sizeof(ChunkHeader)              // fmt
                     + sizeof(WAVExtBlock)              // WAVEFORMATX
                     + sizeof(ChunkHeader)              // DATA
                     + Frame * 160 * sizeof(short);
    strcpy(tag, "WAVE");
    memcpy(riff.riff_format, tag, 4);
    fwrite(&riff, 1, sizeof(RIFFChunk), file_wav);

    // 2. 写 Format chunk
    strcpy(tag, "fmt ");
    memcpy(chunk.chunk_ID, tag, 4);
    chunk.chunk_size = sizeof(WAVExtBlock);
    fwrite(&chunk, 1, sizeof(ChunkHeader), file_wav);
    memset(&wav_fmt_ext, 0, sizeof(WAVExtBlock));
    wav_fmt_ext.format_tag = 1;
    wav_fmt_ext.channels = 1;         			// 单声道
    wav_fmt_ext.samples_per_sec = 8000; 		// 8KHz 
    wav_fmt_ext.avg_bytes_per_sec = 16000;
    wav_fmt_ext.block_align = 2;
    wav_fmt_ext.bits_per_sample = 16; 			// 16位
    fwrite(&wav_fmt_ext, 1, sizeof(WAVExtBlock), file_wav);

    // 3. 写 data chunk 头
    strcpy(tag, "data");
    memcpy(chunk.chunk_ID, tag, 4);
    chunk.chunk_size = Frame * 160 * sizeof(short);
    fwrite(&chunk, 1, sizeof(ChunkHeader), file_wav);
}

/**
 * 计算当前帧大小
 * @param   frame_header
 * @return  frame_size
 */
int calcFrameSize(unsigned char frame_header) {
    int mode, frame_size;
    int tmp = 0;

    // 编码方式编号 = 帧头的3-6位
    frame_header &= 0x78; // 0111-1000
    frame_header >>= 3;
 
    mode = amrEncodeMode[frame_header];
 
    // 计算amr音频数据帧大小
    // 原理: amr一帧对应20ms，那么一秒有50帧的音频数据
    tmp = (double)(((double) mode / (double) AMR_FRAME_COUNT_PER_SECOND) / (double) 8);
    tmp = ((int)(tmp + 0.5));
    frame_size = ((int)((((double)tmp + 0.5) + 0.5)));
    return frame_size;
}

/**
 * 读首帧作为参考帧
 * @param   amr文件，对应的buffer，帧大小，帧头
 * @return  0 for FALSE, 1 for TRUE
 */
int readFirstFrame(FILE *file_amr, unsigned char frame_buffer[], int *std_frame_size, unsigned char *std_frame_header) {
    // Init
    memset(frame_buffer, 0, sizeof(frame_buffer));
 
    // 读帧头
    fread(std_frame_header, 1, sizeof(unsigned char), file_amr);
    if (feof(file_amr)) return 0;
 
    // 根据帧头计算帧大小
    *std_frame_size = calcFrameSize(*std_frame_header);
 
    // 读首帧数据
    frame_buffer[0] = *std_frame_header;
    fread(&(frame_buffer[1]), 1, (*std_frame_size-1)*sizeof(unsigned char), file_amr);
    if (feof(file_amr)) return 0;
 
    return 1;
}

/**
 * 读AMR帧
 * @param   amr文件，对应的buffer，帧大小，帧头
 * @return  0 for FALSE, 1 for TRUE
 */
int readOrdinaryFrame(FILE *file_amr, unsigned char frame_buffer[], int std_frame_size, unsigned char std_frame_header) {
    int bytes = 0;
    unsigned char frame_header;
 
    memset(frame_buffer, 0, sizeof(frame_buffer));
 
    // 与标准帧头对比，若为坏帧，则继续读下一个字节至读到标准帧头
    while(1) {
        bytes = fread(&frame_header, 1, sizeof(unsigned char), file_amr);
        if (feof(file_amr)) return 0;
        if (frame_header == std_frame_header) break;
    }
 
    // 读该帧的语音数据
    frame_buffer[0] = frame_header;
    bytes = fread(&(frame_buffer[1]), 1, (std_frame_size-1)*sizeof(unsigned char), file_amr);
    if (feof(file_amr)) return 0;
 
    return 1;
}

/**
 * 将经典AMR转换成WAV_void
 * @param   AMR文件名，WAV文件名
 * @return  帧计数_int null_void
 */
void classicAMR2WAV(const char *AMR_filename, const char *WAV_filename) {
    FILE *amr_file = NULL;
    FILE *wav_file = NULL;
    char magic[8];
    int *destate;           // ?
    int frame_counter = 0;
    int std_frame_size;
    unsigned char std_frame_header;
 
    unsigned char amr_frame[MAX_AMR_FRAME_SIZE];
    short wav_frame[WAV_FRAME_SIZE];
 
    amr_file = fopen(AMR_filename, "rb");
    if(amr_file==NULL) return 0;
 
    // 检查amr文件头
    fread(magic, sizeof(char), strlen(AMR_MAGIC_NUMBER), amr_file);
    if(strncmp(magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER))) {
        fclose(amr_file);
        return 0;
    }
 
    // 创建并初始化WAV文件
    wav_file = fopen(WAV_filename, "wb");
    WAVFileHeaderWriter(wav_file, frame_counter);
 
    // 初始化解码器
    destate = Decoder_Interface_init();             // ?
 
    // 读首帧作为参考帧
    memset(amr_frame, 0, sizeof(amr_frame));
    memset(wav_frame, 0, sizeof(wav_frame));
    readFirstFrame(amr_file, amr_frame, &std_frame_size, &std_frame_header);
 
    // 解码一个AMR音频帧成WAV数据
    Decoder_Interface_Decode(destate, amr_frame, wav_frame, 0);
    frame_counter++;
    fwrite(wav_frame, sizeof(short), WAV_FRAME_SIZE, wav_file);
 
    // 逐帧解码AMR，写入WAV
    while(1) {
        memset(amr_frame, 0, sizeof(amr_frame));
        memset(wav_frame, 0, sizeof(wav_frame));
        if (!readOrdinaryFrame(amr_file, amr_frame, std_frame_size, std_frame_header))
            break;
        // 解码一个AMR音频帧成WAV数据，参数为 8k 16bit 单声道
        Decoder_Interface_Decode(destate, amr_frame, wav_frame, 0);
        frame_counter++;
        fwrite(wav_frame, sizeof(short), WAV_FRAME_SIZE, wav_file);
    }
 
    Decoder_Interface_exit(destate);    // ?
 
    fclose(wav_file);
 
    // 重写WAV文件头
    wav_file = fopen(WAV_filename, "r+");
    WAVFileHeaderWriter(wav_file, frame_counter);
    fclose(wav_file);
 
    // return frame_counter;
    return;
}

/* 
int main() {
    const char AMR_filename[] = "test.amr";
    const char WAV_filename[] = "test.wav";
    classicAMR2WAV(AMR_filename, WAV_filename);
    return 0;
}
*/
int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("\nClassic AMR2WAV Converter for Windows\n");
		printf("Codename Cattleya\n");
		printf("Developed by Pichu\n");
		printf("Version 1.2.0001 Compile date: Feb 26th, '18\n\n");
		printf("Syntax: ClassicAMR2WAV.exe filename.amr filename.wav\n\n");
		
	}
	else {
		const char *AMR_filename = argv[1];
		const char *WAV_filename = argv[2];
		classicAMR2WAV(AMR_filename, WAV_filename);	
	}
	return 0;
}
