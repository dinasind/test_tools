#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 *去除字符串前连续的‘0‘
 */
char *process_str(char * str)
{
	while (*str == '0') {
		str++;
	}

	return str;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	char buf[50];
	int row;
	int count;
	char *co_str;
	char x_co_str[50];
	char x_co[]="0003 0009";/*在getevent中，proximity距离信息的前八位为0003 0035*/
	if (argc != 2) {
		printf("args error, use: get_coordinate [log_file] ");
		return -1;
	}
	/*从[log_file]获取getevent的信息，解析proximity距离信息并打印出来，成功返回0，
	 *若不成功或没有解析到触摸信号返回1
	 */
	memset(buf, 0, sizeof(buf));
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("open file error\n");
		return -1;
	}
	for (row = 0; row < 50; ) {
		if (fgets(buf, sizeof(buf), fp) == NULL) {
			fclose(fp);
			return -1;
		}

		row ++;
		if (strstr(buf, x_co) != NULL) {
			co_str = strstr(buf, x_co);
			if(co_str == NULL) {
				return -1;
			}
			strcpy(x_co_str, (co_str + 10));
			printf("proximity:%s", process_str(x_co_str));
			fflush(stdout);
			count++;
		}

		if (feof(fp))
			break;

		memset(buf, 0, sizeof(buf));
	}
	fclose(fp);
	if (count > 0)
		return 0;
	else
		return 1;
}
