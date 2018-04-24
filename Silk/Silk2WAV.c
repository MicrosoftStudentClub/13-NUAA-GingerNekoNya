/*
    Classic AMR2WAV Converter
    Codename Cattleya
    Developed by Pichu
    Jan. 18th, '19

    Launcher
*/ 

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("\nSLIK2WAV Converter for Windows\n");
		printf("Codename Cattleya\n");
		printf("Developed by Pichu\n");
		printf("Version 1.2.0002 Compile date: Feb 27th, '18\n\n");
		printf("Syntax: Silk2WAV.exe filename.amr filename.wav\n\n");
	}
	else {
		const char *AMR_filename = argv[1];
		const char *WAV_filename = argv[2];
		char dest1[300] = {0};
		char dest2[300] = {0};
		char filename_tmp[300] = {0};
		char *silk = "SilkDecoder.exe";
		char *silk_para = " -quiet";
		char *ffmpeg = "ffmpeg -v quiet -y -f s16le -ar 24000 -ac 1 -i";
		int n = snprintf(filename_tmp, sizeof(filename_tmp), "%s%s", AMR_filename, ".pcm");
		if(n>=sizeof(filename_tmp)) {
			printf("ERROR");
			exit(1);
		}
		n = snprintf(dest1, sizeof(dest1), "%s %s %s %s", silk, AMR_filename, filename_tmp, silk_para);
		if(n>=sizeof(dest1)) {
			printf("ERROR");
			exit(1);
		}
		n = snprintf(dest2, sizeof(dest2), "%s %s %s", ffmpeg, filename_tmp, WAV_filename);
		if(n>=sizeof(dest2)) {
			printf("ERROR");
			exit(1);
		}
		system(dest1);
		system(dest2);
		remove(filename_tmp);
	}
	return 0;
}
