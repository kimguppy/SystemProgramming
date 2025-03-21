#include <stdio.h>

int main() {
    char name[10];
    int num;

    printf("학번을 입력하세요: ");
    scanf("%d", &num); 

    printf("이름을 입력하세요: ");
    scanf("%s", name);
     // name은 배열이므로 배열 자체가 이미 주소를 가리킴
    
    printf("학번: %d, 이름: %s\n", num, name);

    return 0;
}