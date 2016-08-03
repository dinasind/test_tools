#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
        int fd, ret;
        int i,j, continue_num;

        char buf[4];

        fd = open("/sys/si1132/uv_sensor_data", O_RDONLY);
        if(fd < 0){
                printf("test_uv failed\n");
                return -1;
        }

        ret = read(fd, buf, 4);
        if(ret < 0){
                printf("test_uv failed\n");
                close(fd);
                return -1;
        }
		if((buf[0] != 0) && (buf[0] != 48)){
	        printf("test_uv ok\n");
		}else{
			printf("test_uv failed\n");
			return -1;
		}

        return 0;
}
