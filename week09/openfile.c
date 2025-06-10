#include <stdio.h>
#include <stdlib.h>    // for exit()
#include <unistd.h>    // for close()
#include <fcntl.h>     // for open()
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int fd;

    if (argc < 2) {
        fprintf(stderr, "사용법: %s 파일이름\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDWR)) == -1) {
        perror("파일 열기 오류");
        exit(1);
    }

    printf("파일 %s 열기 성공 : %d\n", argv[1], fd);
    close(fd);
    exit(0);
}
