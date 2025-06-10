# 기본 타겟 (실행 파일을 만드는 규칙)
main: main.o copy.o
	gcc -o main main.o copy.o

# main.o 생성 규칙
main.o: main.c copy.h
	gcc -c main.c

# copy.o 생성 규칙
copy.o: copy.c
	gcc -c copy.c

clean:
	rm -f main.o copy.o main
