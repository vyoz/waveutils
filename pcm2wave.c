#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 2048

/**
	dst:二进制字符串
	numChannel:1=单声道，2=多声道
	sampleRate：采样率 8000/16000
*/
int pcmToWavHeader(int numChannel, int sampleRate, int bitsPerSample, long audioLen, unsigned char *header) 
{
	int longSampleRate = sampleRate;
	int byteRate = bitsPerSample * sampleRate * numChannel / 8;
	int totalAudioLen = audioLen;
	int totalDataLen = totalAudioLen + 36;

	fprintf(stderr, "numChannel:%d, bitsPerSample:%d, longSampleRate:%d, byteRate:%d, totalAudioLen:%d, totalDataLen:%d\n", numChannel, bitsPerSample, longSampleRate, byteRate, totalAudioLen, totalDataLen);
	
	// RIFF/WAVE header
	header[0] = 'R';
	header[1] = 'I';
	header[2] = 'F';
	header[3] = 'F';
	header[4] = (unsigned char)(totalDataLen & 0xff);
	header[5] = (unsigned char)((totalDataLen >> 8) & 0xff);
	header[6] = (unsigned char)((totalDataLen >> 16) & 0xff);
	header[7] = (unsigned char)((totalDataLen >> 24) & 0xff);
	//WAVE
	header[8] = 'W';
	header[9] = 'A';
	header[10] = 'V';
	header[11] = 'E';
	// 'fmt ' chunk
	header[12] = 'f';
	header[13] = 'm';
	header[14] = 't';
	header[15] = ' ';
	// 4 bytes: size of 'fmt ' chunk
	header[16] = 16;
	header[17] = 0;
	header[18] = 0;
	header[19] = 0;
	// format = 1
	header[20] = 1;
	header[21] = 0;
	header[22] = (unsigned char)numChannel;
	header[23] = 0;
	header[24] = (unsigned char)(longSampleRate & 0xff);
	header[25] = (unsigned char)((longSampleRate >> 8) & 0xff);
	header[26] = (unsigned char)((longSampleRate >> 16) & 0xff);
	header[27] = (unsigned char)((longSampleRate >> 24) & 0xff);
	header[28] = (unsigned char)(byteRate & 0xff);
	header[29] = (unsigned char)((byteRate >> 8) & 0xff);
	header[30] = (unsigned char)((byteRate >> 16) & 0xff);
	header[31] = (unsigned char)((byteRate >> 24) & 0xff);
	// block align
	header[32] = (unsigned char)(numChannel * bitsPerSample / 8);
	header[33] = 0;
	// bits per sample
	header[34] = bitsPerSample;
	header[35] = 0;
	//data
	header[36] = 'd';
	header[37] = 'a';
	header[38] = 't';
	header[39] = 'a';
	header[40] = (unsigned char)(totalAudioLen & 0xff);
	header[41] = (unsigned char)((totalAudioLen >> 8) & 0xff);
	header[42] = (unsigned char)((totalAudioLen >> 16) & 0xff);
	header[43] = (unsigned char)((totalAudioLen >> 24) & 0xff);
	
	return 0;
}

int main(int argc, char **argv)
{

	if( argc != 3 )
	{
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "\t%s <file-pcm> <file-wav>\n", argv[0]);
		return -1;
	}

	int numChannel = 1;
	int sampleRate = 16000;
	int bitsPerSample = 16;
	long audioLen = 0;
	unsigned char wavHeader[44];
	char buf[BUF_LEN];
	int bufLen = BUF_LEN;
	const char *file_in = argv[1];
	const char *file_out = argv[2];

	fprintf(stderr, "processing raw file:%s and save to wave file:%s\n", file_in, file_out);

	FILE *fp = fopen(file_in, "rb");
	if( !fp)
	{
		fprintf(stderr, "cannot open file %s\n", file_in);
		return -1;
	}

	fprintf(stderr, "reading file length...\n");
	fseek(fp, 0, SEEK_END);
	fprintf(stderr, "reading file length...2\n");
        audioLen = ftell(fp);
	fprintf(stderr, "reading file length...3\n");
        fseek(fp, 0, SEEK_SET);

        fprintf(stderr, "audio length:%d bytes\n", audioLen);


	FILE *fp_out = fopen(file_out, "wb");
	if( !fp_out )
	{

		fprintf(stderr, "cannot write to file:%s, please check write permission or disk usage\n", file_out);
		fclose(fp);
		return -1;
	}

	fprintf(stderr, "prepare to run pcmToWavHeader...\n");

	pcmToWavHeader(numChannel, sampleRate, bitsPerSample, audioLen, wavHeader);

	fprintf(stderr, "prepare to write header...\n");

	fwrite(wavHeader, 1, 44, fp_out);

	int n = 0;

	do{
		n = fread(buf, 1, bufLen, fp);
		if( n>0)
		{
			fwrite(buf, 1, n, fp_out);
		}else{
			break;
		}
	}while(!feof(fp));

	fclose(fp_out);
	fclose(fp);

	fprintf(stdout, "done, output file:%s\n", file_out);
}
