# 이진수 변환 및 분석 프로그램

0~255 사이의 정수를 입력받아 다음 정보를 출력하는 간단한 프로그램입니다:
- 8비트 이진수 표현
- 이진수에서 1의 개수
- 상위 4비트 값

## 기능

- **입력 검증**: 0~255 범위 내 정수인지 확인
- **이진수 변환**: 8비트 이진수로 표현 (예: 170 → 10101010)
- **1의 개수 계산**: 이진수에서 1의 개수 카운트
- **상위 비트 추출**: 8비트 중 상위 4비트 추출

## 사용 방법

```python
python binary_converter.py
```

## 예제

```
0에서 255 사이의 정수를 입력하세요: 170
입력한 숫자: 170
2진수 표현: 10101010
1의 개수: 4
상위 4비트: 1010
```

## 코드

```python
def analyze_number(number):
    # 입력 값이 0~255 범위인지 확인
    if not 0 <= number <= 255:
        return "오류: 입력 값은 0에서 255 사이여야 합니다."
    
    # 정수를 8비트 2진수로 변환 (앞에 0b 제거하고 8자리로 맞춤)
    binary = format(number, '08b')
    
    # 1의 개수 세기
    count_of_ones = binary.count('1')
    
    # 상위 4비트 추출
    upper_4_bits = binary[:4]
    
    # 결과 반환
    return {
        "입력 숫자": number,
        "2진수 표현": binary,
        "1의 개수": count_of_ones,
        "상위 4비트": upper_4_bits
    }

def main():
    try:
        # 사용자로부터 0~255 사이의 정수 입력 받기
        number = int(input("0에서 255 사이의 정수를 입력하세요: "))
        
        # 입력값 분석
        result = analyze_number(number)
        
        # 결과가 오류 메시지라면 그대로 출력
        if isinstance(result, str):
            print(result)
        else:
            # 결과 출력
            print(f"입력한 숫자: {result['입력 숫자']}")
            print(f"2진수 표현: {result['2진수 표현']}")
            print(f"1의 개수: {result['1의 개수']}")
            print(f"상위 4비트: {result['상위 4비트']}")
            
    except ValueError:
        print("오류: 올바른 정수를 입력하세요.")

# 프로그램 실행
if __name__ == "__main__":
    main()
```