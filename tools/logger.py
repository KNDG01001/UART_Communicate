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
        w.writerow(["seq", "tick_ms", "vref_raw", "vdda_mv"])

        lines = 0
        missing = 0
        last_seq = None
        start = time.time()

        while True:
            raw = ser.readline()
            if not raw:
                continue

            try:
                line = raw.decode("ascii", errors="ignore").strip()
            except Exception:
                continue

            # 스트리밍 라인만 수집: S,<seq>,<tick_ms>,<raw>,<vdda>
            if not line.startswith("S,"):
                continue

            parts = line.split(",")
            if len(parts) != 5:
                continue

            try:
                seq = int(parts[1])
                tick = int(parts[2])
                vref = int(parts[3])
                vdda = int(parts[4])
            except ValueError:
                continue

            # 누락 체크
            if last_seq is not None and seq != last_seq + 1:
                missing += (seq - last_seq - 1)

            last_seq = seq

            w.writerow([seq, tick, vref, vdda])
            lines += 1

            # 진행 상황 출력(1초마다)
            if time.time() - start >= 1.0:
                print(f"\r[+] lines={lines}, missing={missing}", end="")
                start = time.time()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n[!] stopped")
        sys.exit(0)
