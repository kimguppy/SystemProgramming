#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 파일 타입을 검사한다. */
int main(int argc, char *argv[]) {
    struct stat buf;

    for (int i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);

        if (lstat(argv[i], &buf) < 0) {
            perror("lstat()");
            continue;
        }

        if (S_ISREG(buf.st_mode))
            printf("일반 파일\n");
        else if (S_ISDIR(buf.st_mode))
            printf("디렉터리\n");
        else if (S_ISCHR(buf.st_mode))
            printf("문자 장치 파일\n");
        else if (S_ISBLK(buf.st_mode))
            printf("블록 장치 파일\n");
        else if (S_ISFIFO(buf.st_mode))
            printf("FIFO 파일\n");
        else if (S_ISLNK(buf.st_mode))
            printf("심볼릭 링크\n");
        else if (S_ISSOCK(buf.st_mode))
            printf("소켓\n");
        else
            printf("알 수 없는 타입\n");
    }

    return 0;
}
