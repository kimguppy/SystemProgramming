#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const char *messages[] = {
    "Hit:1 http://archive.ubuntu.com/ubuntu focal InRelease",
    "Get:2 http://security.ubuntu.com/ubuntu focal-security InRelease [114 kB]",
    "Get:3 http://archive.ubuntu.com/ubuntu focal-updates InRelease [114 kB]",
    "Get:4 http://archive.ubuntu.com/ubuntu focal-backports InRelease [108 kB]",
    "Get:5 http://ppa.launchpad.net/graphics-drivers/ppa/ubuntu focal InRelease [23.9 kB]",
    "Fetched 5,378 kB in 3s (1,842 kB/s)",
    "Reading package lists... Done",
    "Building dependency tree... Done",
    "Reading state information... Done",
    "Calculating upgrade... Done",
    "The following packages will be upgraded:",
    "  libc6 libstdc++6 linux-image-generic linux-libc-dev",
    "4 upgraded, 0 newly installed, 0 to remove and 0 not upgraded.",
    "Need to get 17.2 MB of archives.",
    "After this operation, 1,024 kB of additional disk space will be used.",
    "Do you want to continue? [Y/n] y",
    "Get:1 http://archive.ubuntu.com/ubuntu focal-updates libc6 2.31-0ubuntu9.9 [2,722 kB]",
    "Preparing to unpack .../libc6_2.31-0ubuntu9.9_amd64.deb ...",
    "Unpacking libc6:amd64 (2.31-0ubuntu9.9) over (2.31-0ubuntu9.8) ...",
    "Setting up libc6:amd64 (2.31-0ubuntu9.9) ...",
    "Processing triggers for libc-bin (2.31-0ubuntu9.8) ...",
    "Processing triggers for man-db (2.9.1-1) ...",
    "Processing triggers for initramfs-tools (0.136ubuntu6.6) ...",
    "update-initramfs: Generating /boot/initrd.img-5.4.0-91-generic",
    "Done"
};

int main(int argc, char *argv[]) {
    int slow = 0;
    int fast = 0;
    int clear_each = 0;

    // 옵션 처리
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--slow") == 0)
            slow = 1;
        else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fast") == 0)
            fast = 1;
        else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--clear-each") == 0)
            clear_each = 1;
        else {
            fprintf(stderr, "사용법: %s [-s|--slow] [-f|--fast] [-c|--clear-each]\n", argv[0]);
            exit(1);
        }
    }

    int count = sizeof(messages) / sizeof(messages[0]);
    srand(time(NULL));

    while (1) {
        if (clear_each) {
            printf("\033[2J\033[H"); // 화면 지우기 및 커서 맨 위로
        }

        for (int i = 0; i < count; ++i) {
            printf("%s\n", messages[i]);
            fflush(stdout);

            if (fast)
                usleep(30000 + rand() % 30000); // 0.03~0.06초
            else if (slow)
                usleep(300000 + rand() % 200000); // 0.3~0.5초
            else
                usleep(150000 + rand() % 150000); // 기본: 0.15~0.3초
        }

        printf("\nSystem update completed successfully.\n\n");
        sleep(2);
    }

    return 0;
}
