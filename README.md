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

## 📦 주요 기능

1. **내부 기준 전압 측정**
   - ADC1을 사용하여 STM32의 내부 VREFINT 값을 읽어옵니다
   - 샘플링 시간: 239.5 사이클
   - 12-bit 해상도 (0-4095)

2. **UART 통신**
   - Baud Rate: 115200
   - 부팅 시 "BOOT OK" 메시지 전송
   - 1초 간격으로 `VREF_RAW=<value>` 형식으로 ADC 값 전송

3. **실시간 모니터링**
   - 시리얼 터미널에서 VREF 값을 실시간으로 확인 가능

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

2. **출력 확인**
   ```
   BOOT OK
   VREF_RAW=1234
   VREF_RAW=1235
   VREF_RAW=1233
   ...
   ```

## 📊 출력 데이터 해석

- **VREF_RAW 값**: 12-bit ADC 값 (0-4095 범위)
- STM32F103의 VREFINT는 일반적으로 약 1.2V
- 실제 전압 계산 공식:
  ```
  VDDA = 1.2V × 4095 / ADC_Value
  ```

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
