#include <stdio.h>
#include <unistd.h>

int main() {
    const char *base = "{Processing";
    int max_dots = 4;
    int i = 0;

    while (1) {
        printf("\r%s", base);

        // 점 찍기
        for (int j = 0; j < max_dots; j++) {
            if (j < i)
                putchar('.');
            else
                putchar(' ');
        }

        printf("}");
        fflush(stdout);
        usleep(300000);

        i++;
        if (i > max_dots) i = 0;  // 0부터 4까지 찍고 다시 0으로
    }

    return 0;
}
