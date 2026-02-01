import serial
import time
import csv
import sys
from datetime import datetime

PORT = "COM7"        
BAUD = 115200
OUT = f"log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

def main():
    print(f"[+] Opening {PORT} @ {BAUD}")
    ser = serial.Serial(PORT, BAUD, timeout=1)

    # 터미널/보드 초기 출력 섞임 방지: 몇 줄 버리고 시작
    time.sleep(0.5)
    ser.reset_input_buffer()

    print(f"[+] Writing to {OUT}")
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["tick_ms", "vref_raw"])

        lines = 0
        start = time.time()

        while True:
            raw = ser.readline()
            if not raw:
                continue

            try:
                line = raw.decode("ascii", errors="ignore").strip()
            except Exception:
                continue

            # 스트리밍 라인만 수집: S,<ms>,<adc>
            if not line.startswith("S,"):
                continue

            parts = line.split(",")
            if len(parts) != 3:
                continue

            try:
                tick = int(parts[1])
                vref = int(parts[2])
            except ValueError:
                continue

            w.writerow([tick, vref])
            lines += 1

            # 진행 상황 출력(1초마다)
            if time.time() - start >= 1.0:
                print(f"\r[+] lines={lines}", end="")
                start = time.time()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n[!] stopped")
        sys.exit(0)
