#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
	int fd, fd1, fd2, ret;
	int i,j, continue_num, count;

	char buf[6] = {'\0'};
	char buf1[9] = {'\0'};
	fd = open("/sys/class/i2c-adapter/i2c-2/2-0077/pressure", O_RDONLY);
	if(fd < 0){
	//	printf("test_bmp280 failed-1\n");
		fd1 = open("/sys/class/i2c-adapter/i2c-2/2-0044/name", O_RDONLY);
		if(fd1 < 0){
			printf("test_pps210 failed-1\n");
			return -1;
		}

		fd2 = open("/sys/class/i2c-adapter/i2c-2/2-0044/enable", O_RDONLY);
		if(fd2 < 0){
			printf("test_pps210 failed-1\n");
			return -1;
		}
	}

	if(!(fd < 0)){
		ret = read(fd, buf, 6);
		if(ret < 0){
			printf("test_bmp280 failed-2\n");
			close(fd);
			return -1;
		}
		if(!strncmp(buf, "74104", 5)){
			printf("test_barometer ok \n");
			return 0;
		}
	}else{
		ret = read(fd1, buf1, 9);
		if(ret < 0){
			printf("test_pps210 failed-2\n");
			close(fd1);
			return -1;
		}
		if(!strncmp(buf1, "pps210a2", 8)){
			printf("test_barometer ok \n");
			return 0;
		}
	}



	return -1;
}
