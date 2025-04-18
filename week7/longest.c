#include <stdio.h>
#include <string.h>

#define MAXLINE 100  // 최대 입력 줄 길이

char line[MAXLINE];     // 현재 입력 줄
char longest[MAXLINE];  // 지금까지 가장 긴 줄

// from 배열을 to 배열에 복사하는 함수
void copy(char from[], char to[]) {
    int i = 0;
    while ((to[i] = from[i]) != '\0')
        ++i;
}

int main() {
    int len;       // 현재 줄 길이
    int max = 0;   // 최대 줄 길이

    // 입력이 끝날 때까지 줄을 반복해서 읽음
    while (fgets(line, MAXLINE, stdin) != NULL) {
        len = strlen(line);  // 현재 줄 길이 계산

        // 더 긴 줄이면 longest에 복사
        if (len > max) {
            max = len;
            copy(line, longest);
        }
    }

    // 입력된 줄이 있다면 가장 긴 줄 출력
    // Linux/macOS/WSL: Ctrl + D

    // Windows CMD/PowerShell: Ctrl + Z 누르고 Enter
    if (max > 0)
        printf("%s", longest);

    return 0;
}
