# 이진수 변환 및 분석 프로그램

bc는 "basic calculator" 또는 "bench calculator"의 약자로, 리눅스와 유닉스 시스템에서 사용되는 명령줄 계산기입니다. 수학적 계산을 수행하는 데 사용되는 강력한 도구입니다.
주요 특징:

임의 정밀도 계산 가능
기본적인 산술 연산(+, -, *, /, %, ^)
조건문과 루프
변수 사용 가능
함수 정의 가능

기본 사용법:

터미널에서 bc를 입력하면 대화형 모드로 진입합니다
계산식을 입력하고 Enter를 누르면 결과가 출력됩니다
나가려면 quit 입력하거나 Ctrl+D를 누릅니다

예시:

```
$ bc
10 + 5
15
2 * 3.5
7.0
scale=2
10 / 3
3.33
quit
```

파이프를 사용하여 한 줄로도 사용할 수 있습니다:

```
$ echo "5+3" | bc
8
```
scale=n 명령을 사용하면 소수점 자릿수를 설정할 수 있으며, -l 옵션을 사용하면 수학 라이브러리를 로드해 sin, cos, log 같은 함수들을 사용할 수 있습니다.