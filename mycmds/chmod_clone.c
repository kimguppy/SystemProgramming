#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>

int verbose = 0;
int changes_only = 0;
int silent = 0;
int recursive = 0;
int follow_links = 0;      // -L 옵션
int change_parents = 0;    // -p 옵션
int show_success = 0;      // -s 옵션
int dry_run = 0;           // -n 옵션

void print_help(const char *progname) {
    printf("사용법: %s [옵션] MODE FILE...\n", progname);
    printf("옵션:\n");
    printf("  -v           : 변경된 파일 정보를 출력\n");
    printf("  -c           : 실제로 변경된 경우만 출력\n");
    printf("  -f           : 오류 메시지 출력 안 함\n");
    printf("  -R           : 디렉터리 내부까지 재귀적으로 처리\n");
    printf("  -L           : 심볼릭 링크를 따라가서 처리\n");
    printf("  -p           : 부모 디렉터리도 함께 권한 변경\n");
    printf("  -s           : 성공한 파일만 출력\n");
    printf("  -n           : 실제 변경 없이 모의 실행 (dry run)\n");
    printf("  --help       : 도움말 출력\n");
    printf("  --version    : 버전 정보 출력\n");
}

void print_version() {
    printf("chmod-clone version 1.0\n");
}

// 부모 디렉터리 권한 변경 함수 (재귀적으로 위로 올라감)
void change_parents_mode(const char *path, mode_t mode) {
    char parent[4096];
    strncpy(parent, path, sizeof(parent));
    parent[sizeof(parent) - 1] = '\0';

    while (1) {
        char *slash = strrchr(parent, '/');
        if (!slash) break;  // 더 이상 상위 디렉터리 없음
        if (slash == parent) {
            // 루트 디렉터리 / 까지 변경
            slash[1] = '\0';
            if (dry_run) {
                if (verbose || show_success)
                    printf("dry-run: would change mode of '%s' to %o\n", parent, mode);
            } else if (chmod(parent, mode) == 0) {
                if (verbose || show_success)
                    printf("mode of '%s' changed to %o\n", parent, mode);
            } else {
                if (!silent) perror(parent);
            }
            break;
        }
        *slash = '\0';  // 부모 디렉터리만 남김

        if (dry_run) {
            if (verbose || show_success)
                printf("dry-run: would change mode of '%s' to %o\n", parent, mode);
        } else if (chmod(parent, mode) == 0) {
            if (verbose || show_success)
                printf("mode of '%s' changed to %o\n", parent, mode);
        } else {
            if (!silent) perror(parent);
            break;
        }
    }
}

int change_mode(const char *path, mode_t mode) {
    struct stat st;
    int stat_res;

    if (follow_links)
        stat_res = stat(path, &st);       // 심볼릭 링크 타겟 정보 얻기
    else
        stat_res = lstat(path, &st);      // 링크 자체 정보 얻기

    if (stat_res != 0) {
        if (!silent) perror(path);
        return -1;
    }

    int already_mode = (st.st_mode & 07777);

    if (dry_run) {
        if (verbose || show_success)
            printf("dry-run: would change mode of '%s' from %o to %o\n", path, already_mode, mode);
        return 0;
    }

    if (chmod(path, mode) != 0) {
        if (!silent) perror(path);
        return -1;
    }

    if (verbose || (changes_only && already_mode != mode) || show_success) {
        printf("mode of '%s' changed to %o\n", path, mode);
    }

    // 부모 디렉터리 권한 변경 요청시
    if (change_parents) {
        change_parents_mode(path, mode);
    }

    return 0;
}

void process_file(const char *path, mode_t mode) {
    struct stat st;
    int stat_res;

    if (follow_links)
        stat_res = stat(path, &st);
    else
        stat_res = lstat(path, &st);

    if (stat_res != 0) {
        if (!silent) perror(path);
        return;
    }

    if (S_ISDIR(st.st_mode) && recursive) {
        DIR *dir = opendir(path);
        if (!dir) {
            if (!silent) perror(path);
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char fullpath[4096];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
            process_file(fullpath, mode);
        }

        closedir(dir);
    }

    change_mode(path, mode);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_help(argv[0]);
        return 1;
    }

    int opt_index = 1;

    // 옵션 처리
    while (opt_index < argc && argv[opt_index][0] == '-') {
        if (strcmp(argv[opt_index], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (strcmp(argv[opt_index], "--version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(argv[opt_index], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[opt_index], "-c") == 0) {
            changes_only = 1;
        } else if (strcmp(argv[opt_index], "-f") == 0) {
            silent = 1;
        } else if (strcmp(argv[opt_index], "-R") == 0) {
            recursive = 1;
        } else if (strcmp(argv[opt_index], "-L") == 0) {
            follow_links = 1;
        } else if (strcmp(argv[opt_index], "-p") == 0) {
            change_parents = 1;
        } else if (strcmp(argv[opt_index], "-s") == 0) {
            show_success = 1;
        } else if (strcmp(argv[opt_index], "-n") == 0) {
            dry_run = 1;
        } else {
            fprintf(stderr, "알 수 없는 옵션: %s\n", argv[opt_index]);
            print_help(argv[0]);
            return 1;
        }
        opt_index++;
    }

    if (opt_index >= argc) {
        fprintf(stderr, "MODE 값이 필요합니다.\n");
        return 1;
    }

    // 모드 파싱 (8진수)
    mode_t mode = strtol(argv[opt_index], NULL, 8);
    opt_index++;

    if (opt_index >= argc) {
        fprintf(stderr, "파일이 하나 이상 필요합니다.\n");
        return 1;
    }

    for (int i = opt_index; i < argc; i++) {
        process_file(argv[i], mode);
    }

    return 0;
}
