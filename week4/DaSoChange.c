#include <stdio.h>
#include <ctype.h> 

int main() {
    while (1) {
        char input;
        printf("문자를 입력하세요 (종료하려면 0): ");

        scanf(" %c", &input);

        if (input == '0') {
            printf("프로그램이 종료되었습니다.\n");
            break; 
        }

        if (isupper(input)) {
            printf("%c의 소문자는 %c 입니다.\n", input, tolower(input));
        } else if (islower(input)) {
            printf("%c의 대문자는 %c 입니다.\n", input, toupper(input));
        } else {
            printf("다시입력하시오.\n");
        }
    }

    return 0;
}
