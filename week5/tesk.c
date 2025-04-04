#include <stdio.h>

int count_ones(int n) {
    int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}
void print_binary_8bit(int n) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
    }
}

void print_upper_4bits(int n) {
    for (int i = 7; i >= 4; i--) {
        printf("%d", (n >> i) & 1);
    }
}

int main() {
    int number;

    while (1) {
        printf("0부터 255 사이의 숫자를 입력하세요: ");
        scanf("%d", &number);

        if (number >= 0 && number <= 255) {
            break; // 올바른 값이면 반복 종료
        } else {
            printf("잘못된 입력입니다. 다시 입력해주세요.\n");
        }
    }

    int ones_count = count_ones(number);

    printf("이진수(8비트): ");
    print_binary_8bit(number);
    printf("\n1의 개수를 8비트로 표현한 값: ");
    print_binary_8bit(ones_count);
    printf("\n=>상위 4비트: ");
    print_upper_4bits(ones_count);
    printf("\n");

    return 0;
}
