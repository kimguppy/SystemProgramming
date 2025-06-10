#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main() {
    // stdin 입력 버퍼 비우기 (명령어가 stdin으로 들어오는 현상 방지)
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    char buffer[100];

    while (1) {
        printf("문자를 입력하세요 (종료하려면 0): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("입력을 읽을 수 없습니다.\n");
            continue;
        }

        // 개행 문자 제거
        buffer[strcspn(buffer, "\n")] = '\0';

        // 입력이 하나의 문자만 있는지 확인
        if (strlen(buffer) != 1) {
            printf("다시입력하시오.\n");
            continue;
        }

        char ch = buffer[0];

        if (ch == '0') {
            break;
        }

        if (isalpha(ch)) {
            if (islower(ch)) {
                printf("%c의 대문자는 %c 입니다.\n", ch, toupper(ch));
            } else {
                printf("%c의 소문자는 %c 입니다.\n", ch, tolower(ch));
            }
        } else {
            printf("알파벳만 입력하세요.\n");
        }
    }

    return 0;
}
