# C와 Java, Python에서의 `#include`와 `import` 차이

이 문서는 `#include`와 `import`의 차이를 설명하고, 각각의 사용 언어에서 어떻게 다르게 동작하는지에 대해 설명합니다.

## 1. `#include` (C, C++, C# 등)

- **주로 사용하는 언어**: C, C++, C#
- **기능**: 다른 파일의 **내용을 코드에 삽입**합니다.
- **방법**:  
  `#include`는 **전처리기 지시어**로, 컴파일 전에 해당 파일을 **직접 코드로 복사**하는 방식입니다. 보통 헤더 파일(`.h`)을 포함시키는 데 사용됩니다.

### 예시 (C)
```c
#include <stdio.h>  // 표준 라이브러리 파일 포함
#include "myheader.h"  // 사용자 정의 헤더 파일 포함

int main() {
    printf("Hello, World!\n");
    return 0;
}
