#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <errno.h>

void usage(const char *prog) {
    printf("사용법: %s [옵션] 파일명\n", prog);
    printf("옵션:\n");
    printf("  -f       삭제 여부 묻지 않고 바로 삭제 (강제 삭제)\n");
    printf("  -r       5분 후 타임아웃 (기본 10분)\n");
    printf("  -s       20분 후 타임아웃\n");
    printf("  -h       파일이 있는 폴더 삭제\n");
    printf("\n");
    printf("옵션이 없으면 10분 타임아웃 후 사용자 확인 후 삭제합니다.\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int force_delete = 0;   // -f
    int delete_dir = 0;     // -h
    int timeout_minutes = 0;  // 기본 10분

    while ((opt = getopt(argc, argv, "frsh")) != -1) {
        switch (opt) {
            case 'f':
                force_delete = 1;
                break;
            case 'r':
                timeout_minutes = 5;
                break;
            case 's':
                timeout_minutes = 20;
                break;
            case 'h':
                delete_dir = 1;
                break;
            default:
                usage(argv[0]);
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "파일명을 지정하세요.\n");
        usage(argv[0]);
        return 1;
    }

    const char *target_path = argv[optind];

    // 삭제 대상 경로
    char path[PATH_MAX];
    strncpy(path, target_path, PATH_MAX - 1);
    path[PATH_MAX - 1] = '\0';

    if (delete_dir) {
        // 폴더 삭제: target_path의 디렉터리 경로 얻기
        char dir_path[PATH_MAX];
        strncpy(dir_path, path, PATH_MAX - 1);
        dir_path[PATH_MAX - 1] = '\0';

        char *dname = dirname(dir_path);
        printf("파일이 있는 디렉터리 '%s' 를 삭제 대상으로 지정했습니다.\n", dname);

        if (force_delete) {
            printf("강제 삭제 옵션 -f 활성화, 즉시 삭제합니다.\n");
            char cmd[PATH_MAX + 20];
            snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", dname);
            int ret = system(cmd);
            if (ret == -1) {
                perror("디렉터리 삭제 실패");
            } else {
                printf("디렉터리 삭제 완료.\n");
            }
            return 0;
        }

        printf("10분 동안 입력이 없으면 삭제 여부를 묻습니다.\n");

        while (1) {
            fd_set readfds;
            struct timeval tv;

            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);

            tv.tv_sec = timeout_minutes * 60;
            tv.tv_usec = 0;

            int retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

            if (retval == -1) {
                perror("select()");
                return 1;
            } else if (retval == 0) {
                // 타임아웃
                printf("\n 10분간 입력 없음! '%s' 디렉터리를 삭제하시겠습니까? (y/n): ", dname);
                fflush(stdout);

                char answer[10];
                if (fgets(answer, sizeof(answer), stdin) != NULL) {
                    if (answer[0] == 'y' || answer[0] == 'Y') {
                        char cmd[PATH_MAX + 20];
                        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", dname);
                        int ret = system(cmd);
                        if (ret == -1) {
                            perror("디렉터리 삭제 실패");
                        } else {
                            printf("디렉터리 삭제 완료.\n");
                        }
                        break;
                    } else {
                        printf("삭제 취소, 계속 모니터링합니다.\n");
                    }
                }
            } else {
                // 입력 있음 -> 초기화
                char buf[1024];
                fgets(buf, sizeof(buf), stdin);
                printf("입력 감지, 타이머 초기화...\n");
            }
        }

        return 0;
    } else {
        // 파일 삭제
        printf("모니터링 대상 파일: %s\n", path);

        if (force_delete) {
            printf("강제 삭제 옵션 -f 활성화, %d분 후 바로 삭제합니다.\n", timeout_minutes);
            sleep(timeout_minutes * 60);
            if (remove(path) == 0) {
                printf("파일 '%s' 삭제 완료.\n", path);
            } else {
                perror("파일 삭제 실패");
            }
            return 0;
        }

        printf(" 10분 동안 입력 없으면 삭제 여부를 묻습니다.\n");
        printf("아무 키나 누르면 타이머가 초기화됩니다.\n");

        while (1) {
            fd_set readfds;
            struct timeval tv;

            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);

            tv.tv_sec = timeout_minutes * 60;
            tv.tv_usec = 0;

            int retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

            if (retval == -1) {
                perror("select()");
                return 1;
            } else if (retval == 0) {
                // 타임아웃
                printf("\n 10분간 입력 없음! '%s' 를 삭제하시겠습니까? (y/n): ", path);
                fflush(stdout);

                char answer[10];
                if (fgets(answer, sizeof(answer), stdin) != NULL) {
                    if (answer[0] == 'y' || answer[0] == 'Y') {
                        if (remove(path) == 0) {
                            printf("파일 '%s' 삭제 완료.\n", path);
                        } else {
                            perror("파일 삭제 실패");
                        }
                        break;
                    } else {
                        printf("삭제 취소, 계속 모니터링합니다.\n");
                    }
                }
            } else {
                // 입력 있음 -> 초기화
                char buf[1024];
                fgets(buf, sizeof(buf), stdin);
                printf("입력 감지, 타이머 초기화...\n");
            }
        }

        return 0;
    }
}
