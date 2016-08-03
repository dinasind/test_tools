#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main(void)
{
        int fd, fd1, ret, ret_strstr;
        int i,j, continue_num, length, res;
		int flog = 0;
        char buf[128];
		char string[10];
		char buf_state[2];
		char *p = "$GP";

		fd1 = open("/sys/class/rfkill/rfkill1/state", O_WRONLY);
		if(fd1 < 0){
                printf("test_gps failed\n");
                goto oh_no;
        }
        fd = open("/dev/ttyS1", O_RDONLY);
        if(fd < 0){
			printf("test_gps failed\n");
			goto oh_no;
		}
		buf_state[1] = '\0';
		read(fd1, buf_state, 1);
		if(buf_state[0] = '1'){
			strcpy(string, "0\n");
			length = strlen(string);
			printf("strlen(0.n) : %d  \n", length);
			if ((res = write(fd1, string, length)) != length)
			{
				printf("Error writing 0 to the file.\n");
				goto oh_no;
			}
			sleep(1);
			strcpy(string, "1\n");
			length = strlen(string);
			if ((res = write(fd1, string, length)) != length)
			{
				printf("Error writing 0-1 to the file.\n");
				goto oh_no;
			}
		}else{
			strcpy(string, "1\n");
			length = strlen(string);
			if ((res = write(fd1, string, length)) != length)
			{
				printf("Error writing 1 to the file.\n");
				goto oh_no;
			}
		}
//		fcntl(fd, F_SETFL, O_NONBLOCK);
//		delay(1);
        ret = read(fd, buf, 4);
        if(ret < 0){
                printf("test_gps failed-1\n");
                close(fd);
                goto oh_no;
        }
		ret_strstr = strstr(buf, p);
		if((buf[0] == 0 && buf[1] == '$' && buf[2] == 'G' && buf[3] =='P') || (buf[0] == '$' && buf[1] == 'G' && buf[2] == 'P') || (ret_strstr != NULL)){
	        printf("test_gps ok\n");
			goto oh_yes;
		}else{
			printf("test_gps failed-2\n");
			printf("hex-buf[0] %c, 1:%c, 2:%c 3:%c\n", buf[0], buf[1], buf[2], buf[3]);
			printf("10-buf[0] %d, 1:%d, 2:%d 3:%d\n", buf[0], buf[1], buf[2], buf[3]);
			goto oh_no;
		}
oh_no:
		strcpy(string, "0\n");
			length = strlen(string);
			printf("strlen(0.n) : %d  \n", length);
			if ((res = write(fd1, string, length)) != length)
			{
				printf("OH -- Error writing 0 to the file.\n");
				return -1;
			}
        return -1;
oh_yes:
		strcpy(string, "0\n");
			length = strlen(string);
			printf("strlen(0.n) : %d  \n", length);
			if ((res = write(fd1, string, length)) != length)
			{
				printf("OH -- Error writing 0 to the file.\n");
				return 0;
			}
        return 0;
}
