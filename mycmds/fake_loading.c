#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *base = "{Loading";
    int max_dots = 4;
    int i = 0;

    // 옵션 처리
    if (argc > 1) {
        if (strcmp(argv[1], "-p") == 0) {
            base = "{Processing";
        } else if (strcmp(argv[1], "-u") == 0) {
            base = "{Updating";
        } else {
            fprintf(stderr, "사용법: %s [-p | -u | -l]\n", argv[0]);
            return 1;
        }
    }

    while (1) {
        printf("\r%s", base);

        for (int j = 0; j < max_dots; j++) {
            if (j < i)
                putchar('.');
            else
                putchar(' ');
        }

        printf("}");
        fflush(stdout);
        usleep(300000);

        i = (i + 1) % (max_dots + 1);
    }

    return 0;
}
