#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	char buf[30];
	int row;
	int count_0 = 0;

	if (argc != 2) {
		printf("args error\n");
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("open file error\n");
		return -1;
	}
	for (row = 0; row < 8; row++) {
		if ((fgets(buf, sizeof(buf), fp) == NULL) && (row < 7)) {
			fclose(fp);
			return -1;
		}
		if (memcmp(buf, "0003 0010 ", 10) && memcmp(buf, "0003 0011 ", 10) &&
		    memcmp(buf, "0003 000a ", 10)) {
			if (memcmp(buf, "0000 0000 00000000", 18)) {
				fclose(fp);
				return -1;
			} else {
				count_0++;
			}
		}
		if (feof(fp))
			break;
		memset(buf, 0, sizeof(buf));
	}
	if (count_0 > 3) {
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}
