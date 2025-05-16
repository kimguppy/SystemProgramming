#include <stdio.h>
#include "student.h"

#define MAX_STUDENTS 100

// 함수 프로토타입
void add_student(struct student students[], int *count);
void display_students(const struct student students[], int count);
int find_student_by_id(const struct student students[], int count, int id);

int main() {
    struct student students[MAX_STUDENTS];
    int count = 0;
    int choice;

    while (1) {
        printf("\n학생 데이터베이스\n");
        printf("1. 학생 추가\n");
        printf("2. 모든 학생 출력\n");
        printf("3. ID로 학생 검색\n");
        printf("4. 종료\n");
        printf("선택하세요: ");
        scanf("%d", &choice);

        if (choice == 1) {
            add_student(students, &count);
        } else if (choice == 2) {
            display_students(students, count);
        } else if (choice == 3) {
            int id;
            printf("검색할 학생 ID를 입력하세요: ");
            scanf("%d", &id);
            int index = find_student_by_id(students, count, id);
            if (index != -1) {
                printf("이름: %s, ID: %d, 점수: %d\n", students[index].name, students[index].id, students[index].score);
            } else {
                printf("학생을 찾을 수 없습니다.\n");
            }
        } else if (choice == 4) {
            break;
        } else {
            printf("잘못된 선택입니다.\n");
        }
    }

    return 0;
}

// 학생 추가 함수
void add_student(struct student students[], int *count) {
    if (*count >= MAX_STUDENTS) {
        printf("더 이상 학생을 추가할 수 없습니다.\n");
        return;
    }

    printf("학생 이름 입력: ");
    scanf("%s", students[*count].name);
    students[*count].id = START_ID + *count;
    printf("점수 입력: ");
    scanf("%d", &students[*count].score);

    printf("학생 추가 완료. ID: %d\n", students[*count].id);
    (*count)++;
}

// 학생 목록 출력
void display_students(const struct student students[], int count) {
    for (int i = 0; i < count; i++) {
        printf("이름: %s, ID: %d, 점수: %d\n", students[i].name, students[i].id, students[i].score);
    }
}

// ID로 학생 검색
int find_student_by_id(const struct student students[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            return i;
        }
    }
    return -1;
}