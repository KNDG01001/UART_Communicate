# UART VREF Logger

STM32 Nucleo-F103RB 보드를 사용하여 내부 기준 전압(VREF)을 ADC로 읽고, UART를 통해 실시간으로 전송하는 프로젝트입니다.

## 📋 프로젝트 개요

이 프로젝트는 STM32F103RBTx 마이크로컨트롤러의 내부 기준 전압을 주기적으로 측정하고, UART 통신을 통해 시리얼 모니터로 데이터를 전송합니다. 전원 공급 안정성 모니터링, 디버깅, 또는 교육 목적으로 활용할 수 있습니다.

📚 **[프로젝트 상세 문서 (Notion)](https://www.notion.so/Uart-communicate-2f388056fa6a806cbde3e6a4b4f986bd?source=copy_link)**

## 🛠️ 하드웨어 사양

- **MCU**: STM32F103RBTx
- **보드**: NUCLEO-F103RB
- **클럭**: 64 MHz (HSI with PLL x16)
- **주변장치**:
  - ADC1: 내부 VREFINT 채널 사용
  - USART2: 115200 baud, 8N1
  - GPIO: PA5 (LD2 - Green LED), PC13 (B1 - Blue Push Button)

## 🔌 핀 구성

| 핀 번호 | 기능 | 설명 |
|---------|------|------|
| PA2 | USART2_TX | UART 송신 |
| PA3 | USART2_RX | UART 수신 |
| PA5 | GPIO Output | LED (LD2) |
| PA13 | SWD | 디버그 인터페이스 (SWDIO) |
| PA14 | SWD | 디버그 인터페이스 (SWCLK) |
| PC13 | GPIO EXTI | 푸시 버튼 (B1) |

## 💻 CLI 명령어

시리얼 터미널에서 다음 명령어를 사용할 수 있습니다:

| 명령어 | 설명 |
|--------|------|
| `help` | 사용 가능한 명령어 목록 표시 |
| `adc read` | 현재 VREF ADC 값을 즉시 읽어서 출력 |
| `stream start` | 자동 스트리밍 시작 (설정된 주기로 데이터 전송) |
| `stream stop` | 자동 스트리밍 정지 |
| `rate <ms>` | 스트리밍 주기 설정 (10~5000ms) |

### 사용 예시

```
> help
Commands: 
  help
  adc read
  stream start
  stream stop
  rate <ms>
> adc read
VREF_RAW=1492
> rate 500
OK rate set
> stream start
Ok stream on
S,1234,1492
S,1734,1491
S,2234,1493
...
> stream stop
OK Stream off
```

## 🚀 빌드 및 실행

1. **내부 기준 전압 측정**
   - ADC1을 사용하여 STM32의 내부 VREFINT 값을 읽어옵니다
   - 샘플링 시간: 239.5 사이클
   - 12-bit 해상도 (0-4095)

2. **대화형 CLI (Command Line Interface)**
   - UART를 통한 양방향 통신
   - 실시간 명령어 처리 및 응답
   - 인터럽트 기반 UART 수신으로 안정적인 입력 처리

3. **스트리밍 제어**
   - 시작/정지 제어 가능
   - 스트리밍 주기 설정 (10~5000ms)
   - 타임스탬프 포함 데이터 출력 (`S,<tick_ms>,<adc_value>`)

4. **UART 통신**
   - Baud Rate: 115200
   - 부팅 시 "BOOT OK" 메시지 및 프롬프트(`>`) 표시
   - 에코 기능으로 입력 확인 가능

## 🚀 빌드 및 실행

### 필수 요구사항

- **IDE**: STM32CubeIDE
- **펌웨어 패키지**: STM32Cube FW_F1 V1.8.6
- **툴체인**: ARM GCC

### 빌드 방법

1. **프로젝트 클론**
   ```bash
   git clone https://github.com/KNDG01001/UART_Communicate.git
   cd UART_Communicate
   ```

2. **STM32CubeIDE에서 프로젝트 열기**
   - File → Open Projects from File System
   - `uart_vref_logger` 폴더 선택

3. **프로젝트 빌드**
   - Project → Build All (Ctrl+B)

4. **보드에 플래시**
   - Run → Debug (F11) 또는 Run (Ctrl+F11)

### 실행 방법

1. **UART 연결**
   - USB 케이블로 Nucleo 보드를 PC에 연결
   - 시리얼 터미널 프로그램 실행 (Tera Term, PuTTY 등)
   - 설정: 115200 baud, 8N1, No flow control

2. **대화형 CLI 사용**
   ```
   BOOT OK
   > help
   Commands: 
     help
     adc read
     stream start
     stream stop
     rate <ms>
   > stream start
   Ok stream on
   S,1000,1492
   S,2000,1491
   ...
   ```

## 🐍 Python 로거 도구

`tools/logger.py`는 STM32에서 전송되는 스트림 데이터를 자동으로 CSV 파일에 기록하는 Python 스크립트입니다.

### 사용 방법

1. **필수 패키지 설치**
   ```bash
   pip install pyserial
   ```

2. **포트 설정 확인**
   - `tools/logger.py` 파일을 열고 `PORT` 변수를 사용 중인 COM 포트로 설정
   ```python
   PORT = "COM7"  # 본인의 포트 번호로 변경
   ```

3. **스크립트 실행**
   ```bash
   python tools/logger.py
   ```

4. **보드에서 스트리밍 시작**
   - 시리얼 터미널에서 `stream start` 명령어 입력
   - 또는 별도 터미널에서 명령어 전송

5. **데이터 수집**
   - 스크립트는 `S,<tick_ms>,<adc_value>` 형식의 데이터만 수집
   - `log_YYYYMMDD_HHMMSS.csv` 파일에 자동 저장
   - Ctrl+C로 종료

### 출력 예시

생성된 CSV 파일:
```csv
tick_ms,vref_raw
1000,1492
2000,1491
3000,1493
4000,1492
```

## 📊 출력 데이터 해석

### ADC 값 (`VREF_RAW` 또는 스트림 데이터)
- **범위**: 12-bit ADC 값 (0-4095)
- **VREFINT**: STM32F103의 내부 기준 전압은 일반적으로 약 1.2V
- **실제 전압 계산 공식**:
  ```
  VDDA = 1.2V × 4095 / ADC_Value
  ```

### 스트림 출력 형식
```
S,<tick_ms>,<adc_value>
```
- `tick_ms`: HAL_GetTick()로부터의 밀리초 타임스탬프
- `adc_value`: VREFINT ADC 측정값

## 🔧 개발 환경

- **STM32CubeMX**: 6.16.1
- **HAL 라이브러리**: STM32F1 HAL Driver
- **컴파일러**: GCC

## 📝 코드 구조

```
uart_vref_logger/
├── Core/
│   ├── Inc/           # 헤더 파일
│   └── Src/
│       └── main.c     # 메인 애플리케이션
├── Drivers/           # HAL 드라이버
├── Debug/             # 빌드 출력
├── .cproject          # Eclipse 프로젝트 설정
├── .project
└── uart_vref_logger.ioc  # STM32CubeMX 설정 파일
```

## 🤝 기여

이슈 제보 및 풀 리퀘스트를 환영합니다!

## 📄 라이센스

이 프로젝트는 MIT 라이센스 하에 배포됩니다. 자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

## 👤 작성자

KNDG01001

## 📮 문의

프로젝트 관련 문의사항은 GitHub Issues를 통해 남겨주세요.
