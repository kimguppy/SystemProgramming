#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pid, child, status;

    printf("[%d] 부모 프로세스 시작\n", getpid());

    pid = fork();
    if (pid < 0) {
        // fork 실패 처리
        perror("fork 실패");
        exit(1);
    }

    if (pid == 0) {
        // 자식 프로세스
        printf("[%d] 자식 프로세스 시작\n", getpid());
        exit(1);  // 종료 코드 1로 종료
    }

    // 부모 프로세스
    child = wait(&status);  // 자식 프로세스가 끝나기를 기다림
    printf("[%d] 자식 프로세스 %d 종료\n", getpid(), child);

    if (WIFEXITED(status)) {
        printf("\t종료 코드 %d\n", WEXITSTATUS(status));
    } else {
        printf("\t정상 종료되지 않음\n");
    }

    return 0;
}
