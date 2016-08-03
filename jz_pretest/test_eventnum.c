#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	char buf1[50];
	char buf2[50];
	int row;
	char *dev_str;
	char event_num_str[50];
	int event_num;
	if (argc != 3) {
		printf("args error, use: test_eventnum [log_file] [dev_name]\n");
		return -1;
	}
	memset(buf1, 0, sizeof(buf1));
	memset(buf2, 0, sizeof(buf2));
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("open file error\n");
		return -1;
	}
	for (row = 0; row < 12; ) {
		if (fgets(buf1, sizeof(buf1), fp) == NULL) {
			fclose(fp);
			return -1;
		}
		row ++;
		if (fgets(buf2, sizeof(buf2), fp) == NULL) {
			fclose(fp);
			return -1;
		}
		row ++;
		if (strstr(buf2, argv[2]) != NULL) {
			dev_str = strstr(buf1, "event");
			if(dev_str == NULL) {
				return -1;
			}
			strcpy(event_num_str, (dev_str + 5));
			event_num = atoi(event_num_str);
			if (event_num < 0 || event_num >= 10)
				return -1;

			fclose(fp);
			return event_num;
		}
		if (feof(fp))
			break;
		memset(buf1, 0, sizeof(buf1));
		memset(buf2, 0, sizeof(buf2));
	}
	fclose(fp);
	return -1;
}
