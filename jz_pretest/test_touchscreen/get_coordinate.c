#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	char buf[50];
	int row;
	int count;
	char *co_str;
	char x_co_str[50];
	char y_co_str[50];
	char x_co[]="0003 0035";/*在getevent中，触摸的x坐标信息的前八位为0003 0035*/
	char y_co[]="0003 0036";/*在getevent中，触摸的y坐标信息的前八位为0003 0036*/
	if (argc != 2) {
		printf("args error, use: get_coordinate [log_file] ");
		return -1;
	}
	/*从[log_file]获取getevent的信息，解析触摸的x,y坐标信息并打印出来，成功返回0，
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
			printf("X:%s",x_co_str);
			fflush(stdout);
			count++;
		}
		if (strstr(buf, y_co) != NULL) {
			co_str = strstr(buf, y_co);
			if(co_str == NULL) {
				return -1;
			}
			strcpy(y_co_str, (co_str + 10));
			printf("Y:%s\n",y_co_str);
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
