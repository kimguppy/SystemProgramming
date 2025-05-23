#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <utime.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    struct stat buf;       // 파일 상태 저장용 구조체
    struct utimbuf time;   // 접근시간, 수정시간 저장용 구조체

    if (argc < 3) {
        fprintf(stderr, "사용법: %s file1 file2\n", argv[0]);
        exit(1);
    }

    // 첫 번째 파일 상태 가져오기
    if (stat(argv[1], &buf) < 0) {
        perror("stat()");
        exit(1);
    }

    // 첫 번째 파일의 접근 시간과 수정 시간 저장
    time.actime = buf.st_atime;
    time.modtime = buf.st_mtime;

    // 두 번째 파일의 시간 정보를 첫 번째 파일과 동일하게 설정
    if (utime(argv[2], &time) < 0) {
        perror("utime()");
        exit(1);
    }

    return 0;
}


/*./cptime file1.txt file2.txt */