#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <getopt.h>

const char* file_type(mode_t mode) {
    if (S_ISREG(mode))  return "일반 파일";
    if (S_ISDIR(mode))  return "디렉터리";
    if (S_ISCHR(mode))  return "문자 장치 파일";
    if (S_ISBLK(mode))  return "블록 장치 파일";
    if (S_ISFIFO(mode)) return "FIFO 파일";
    if (S_ISLNK(mode))  return "심볼릭 링크";
    if (S_ISSOCK(mode)) return "소켓";
    return "알 수 없는 타입";
}

const char* file_type_color(mode_t mode) {
    if (S_ISDIR(mode))  return "\033[34m";  // 파랑
    if (S_ISLNK(mode))  return "\033[36m";  // 시안
    if (S_ISCHR(mode) || S_ISBLK(mode)) return "\033[33m";  // 노랑
    return "\033[0m";  // 일반
}

void permissions_str(mode_t mode, char *str) {
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';
    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0';
}

void print_help(const char *progname) {
    printf("사용법: %s [옵션] 파일...\n", progname);
    printf("옵션:\n");
    printf("  -a  파일 이름과 타입 출력 (기본값)\n");
    printf("  -t  타입만 출력\n");
    printf("  -s  파일 크기 출력\n");
    printf("  -p  퍼미션 출력 (rwx 형식)\n");
    printf("  -o  UID:GID 출력\n");
    printf("  -n  사용자명:그룹명 출력\n");
    printf("  -c  컬러 출력\n");
    printf("  -j  JSON 포맷 출력\n");
    printf("  -l  long format 출력 (모든 정보)\n");
    printf("  -h  도움말 출력\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int show_type = 1, show_name = 1, show_size = 0, show_perm = 0, show_uidgid = 0;
    int show_json = 0, use_color = 0, show_namegroup = 0;

    while ((opt = getopt(argc, argv, "atspojnclh")) != -1) {
        switch (opt) {
            case 'a': show_type = 1; show_name = 1; break;
            case 't': show_type = 1; show_name = 0; break;
            case 's': show_size = 1; break;
            case 'p': show_perm = 1; break;
            case 'o': show_uidgid = 1; break;
            case 'n': show_namegroup = 1; break;
            case 'c': use_color = 1; break;
            case 'j': show_json = 1; break;
            case 'l':
                show_type = show_size = show_perm = 1;
                show_name = show_uidgid = 1;
                break;
            case 'h':
            default:
                print_help(argv[0]);
                return 0;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "오류: 검사할 파일을 지정하세요.\n");
        print_help(argv[0]);
        return 1;
    }

    for (int i = optind; i < argc; i++) {
        struct stat buf;
        if (lstat(argv[i], &buf) < 0) {
            perror(argv[i]);
            continue;
        }

        if (show_json) {
            char perm[10];
            permissions_str(buf.st_mode, perm);
            struct passwd *pw = getpwuid(buf.st_uid);
            struct group  *gr = getgrgid(buf.st_gid);
            printf("{\n");
            printf("  \"name\": \"%s\",\n", argv[i]);
            printf("  \"type\": \"%s\",\n", file_type(buf.st_mode));
            printf("  \"size\": %lld,\n", (long long)buf.st_size);
            printf("  \"permissions\": \"%s\",\n", perm);
            printf("  \"uid\": %d,\n", buf.st_uid);
            printf("  \"gid\": %d,\n", buf.st_gid);
            printf("  \"owner\": \"%s\",\n", pw ? pw->pw_name : "unknown");
            printf("  \"group\": \"%s\"\n", gr ? gr->gr_name : "unknown");
            printf("}\n");
            continue;
        }

        if (show_name) {
            if (use_color) printf("%s", file_type_color(buf.st_mode));
            printf("%s: ", argv[i]);
            if (use_color) printf("\033[0m");
        }

        if (show_type) printf("%s ", file_type(buf.st_mode));
        if (show_size) printf("크기: %lldB ", (long long)buf.st_size);
        if (show_perm) {
            char perm[10];
            permissions_str(buf.st_mode, perm);
            printf("퍼미션: %s ", perm);
        }
        if (show_uidgid)
            printf("UID:%d GID:%d ", buf.st_uid, buf.st_gid);
        if (show_namegroup) {
            struct passwd *pw = getpwuid(buf.st_uid);
            struct group  *gr = getgrgid(buf.st_gid);
            printf("소유자:%s 그룹:%s ", 
                pw ? pw->pw_name : "unknown", 
                gr ? gr->gr_name : "unknown");
        }

        printf("\n");
    }

    return 0;
}



/* 
./fileinfo -a test.txt           # 기본: 파일명 + 타입
./fileinfo -t test.txt           # 타입만 출력
./fileinfo -s test.txt           # 크기만 출력
./fileinfo -p test.txt           # 퍼미션만 출력
./fileinfo -o test.txt           # UID/GID 출력
./fileinfo -l test.txt           # 전체 정보 출력
./fileinfo -l -c file.txt        # 컬러 + 전체 정보
./fileinfo -j file.txt           # JSON 형식 출력
./fileinfo -n file.txt           # 사용자 이름, 그룹 이름 출력
./fileinfo -t -c file.txt        # 타입만 컬러 출력
총 10개 옵션
*/