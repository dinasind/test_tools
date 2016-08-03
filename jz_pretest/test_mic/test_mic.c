#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
        int fd, ret;
        int i,j, continue_num;

        char buf[1024];

        fd = open("/dev/jz-dmic", O_RDONLY);
        if(fd < 0){
                printf("test_mic failed\n");
                return -1;
        }

        ret = read(fd, buf, 1024);
        if(ret < 0){
                printf("test_mic failed\n");
                close(fd);
                return -1;
        }

        continue_num = 0;
        for(i = 0, j = 1; i <= 8; i++){
                if((buf[j++] & 0xff) == 0x73 && (buf[j++] & 0xff) == 0xfa){
                        continue_num++;
                }
        }

        if(continue_num == 8){
                printf("test_mic failed\n");
                return -1;
        }

        printf("test_mic ok\n");

        return 0;
}
