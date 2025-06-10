#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void print_by_whitespace(FILE *fp) {
    int c;
    char buffer[1024];
    int idx = 0;
    int in_space = 0;

    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c)) {
            if (in_space) {
                buffer[idx++] = c;
            } else {
                if (idx > 0) {
                    buffer[idx] = '\0';
                    printf("%s", buffer);
                    fflush(stdout);
                    idx = 0;
                    getch();
                }
                buffer[idx++] = c;
                in_space = 1;
            }
        } else {
            if (in_space) {
                buffer[idx] = '\0';
                printf("%s", buffer);
                fflush(stdout);
                idx = 0;
                getch();
                buffer[idx++] = c;
                in_space = 0;
            } else {
                buffer[idx++] = c;
            }
        }

        if (idx >= sizeof(buffer) - 1) {
            buffer[idx] = '\0';
            printf("%s", buffer);
            fflush(stdout);
            idx = 0;
            getch();
        }
    }

    if (idx > 0) {
        buffer[idx] = '\0';
        printf("%s", buffer);
    }
}

void print_by_char(FILE *fp) {
    int c;
    while ((c = fgetc(fp)) != EOF) {
        putchar(c);
        fflush(stdout);
        if (c != '\n') {
            getch();
        }
    }
}

void print_by_line(FILE *fp) {
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
        fflush(stdout);
        getch();
    }
}

void print_usage(const char *progname) {
    fprintf(stderr, "사용법: %s [-w|-o|-l] 파일이름\n", progname);
    fprintf(stderr, "  -w : 띄어쓰기 단위 출력 (기본)\n");
    fprintf(stderr, "  -o : 한 글자씩 출력 (문단 유지)\n");
    fprintf(stderr, "  -l : 한 줄씩 출력\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    char *filename = NULL;
    char mode = 'w'; // 기본 모드 띄어쓰기 단위

    // 옵션 체크
    if (argv[1][0] == '-') {
        if (strcmp(argv[1], "-w") == 0) {
            mode = 'w';
        } else if (strcmp(argv[1], "-o") == 0) {
            mode = 'o';
        } else if (strcmp(argv[1], "-l") == 0) {
            mode = 'l';
        } else {
            print_usage(argv[0]);
            return 1;
        }
        if (argc < 3) {
            fprintf(stderr, "파일 이름이 필요합니다.\n");
            return 1;
        }
        filename = argv[2];
    } else {
        filename = argv[1];
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("파일 열기 실패");
        return 1;
    }

    switch (mode) {
        case 'w':
            print_by_whitespace(fp);
            break;
        case 'o':
            print_by_char(fp);
            break;
        case 'l':
            print_by_line(fp);
            break;
        default:
            print_usage(argv[0]);
            fclose(fp);
            return 1;
    }

    fclose(fp);
    return 0;
}
