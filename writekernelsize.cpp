#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char u8;

int main()
{
	FILE* file = fopen("build/kernel.bin", "r");
	if (!file) { printf("Failed to open\n"); return 1; }
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	rewind(file);

	long finalFileSize = (fileSize / 512) * 512;
	if (finalFileSize < fileSize)
		finalFileSize += 512;
	u8 sectors = finalFileSize / 512;

	char* buffer = new char[finalFileSize];
	if (fread(buffer, sizeof(char), fileSize, file) != fileSize)
	{ printf("Read error\n"); return 1; }
	fclose(file);

	if (finalFileSize > fileSize)
		memset(buffer + fileSize, 0, sizeof(char) * (finalFileSize - fileSize));

	if (finalFileSize >= 512)
		buffer[509] = sectors;

	file = fopen("build/kernel.bin", "wb");
	if (!file) { printf("Failed to re-open\n"); return 1; }
	if (fwrite(buffer, sizeof(char), finalFileSize, file) != finalFileSize)
	{ printf("Write error\n"); return 1; }
	fclose(file);
	printf("%ld -> %ld (%d sectors)\n", fileSize, finalFileSize, sectors);
	return 0;
}
