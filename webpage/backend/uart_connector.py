"""
uart_connector.py
-----------------
Standalone connector: reads JSON from one UART port and pushes each packet
to the Flask ingest endpoint via HTTP POST.

Run one instance per SensorTag:
    python uart_connector.py --port COM3
    python uart_connector.py --port COM4 --baud 115200
    python uart_connector.py --mock               # simulates did=1 and did=2
    python uart_connector.py --mock --did 3       # simulates a single did=3
"""

import argparse
import json
import logging
import math
import random
import threading
import time

import requests

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
)
log = logging.getLogger(__name__)


# ---------------------------------------------------------------------------
# Serial mode
# ---------------------------------------------------------------------------

def run_serial(port: str, baud: int, url: str) -> None:
    import serial

    log.info("Connecting to %s at %d baud → %s", port, baud, url)
    while True:
        try:
            with serial.Serial(port, baud, timeout=1) as ser:
                log.info("Serial port %s open", port)
                while True:
                    raw_line = ser.readline()
                    log.debug("raw: %s", raw_line)
                    if not raw_line:
                        continue
                    text = raw_line.decode("utf-8", errors="replace").strip()
                    if not text:
                        continue
                    try:                       
                        parsed = json.loads(text)
                    except json.JSONDecodeError:
                        log.debug("Non-JSON line: %s", text)
                        continue
                    log.info("Sending packet: %s", parsed)
                    threading.Thread(target=_post, args=(url, parsed), daemon=True).start()
        except Exception as exc:
            log.warning("Serial error (%s). Retrying in 5 s…", exc)
            time.sleep(5)


# ---------------------------------------------------------------------------
# Mock mode
# ---------------------------------------------------------------------------

_BASELINES = {
    "00124B0001000001": dict(temp=22.0, hum=45.0, a=1.0),
    "00124B0001000002": dict(temp=24.5, hum=52.0, a=1.0),
}

_DEFAULT_BASE = dict(temp=23.0, hum=48.0, a=1.0)


def run_mock(url: str, device_id: str | None) -> None:
    targets = {device_id: _BASELINES.get(device_id, _DEFAULT_BASE)} if device_id else _BASELINES
    log.info("Mock mode for id=%s → %s", list(targets.keys()), url)
    t = 0.0
    while True:
        for d, base in targets.items():
            packet = {
                "id":    d,
                "temp":  round(base["temp"] + math.sin(t) * 0.5,    2),
                "hum":   round(base["hum"]  + random.gauss(0, 1),   2),
                "b1":    False,
                "b2":    False,
                "a":     round(base["a"]    + random.gauss(0, 0.05), 2),
                "alarm": False,
            }
            _post(url, packet)
        t += 0.1
        time.sleep(2)


# ---------------------------------------------------------------------------
# HTTP helper
# ---------------------------------------------------------------------------

def _post(url: str, data: dict) -> None:
    try:
        resp = requests.post(url, json=data, timeout=3)
        if not resp.ok:
            log.warning("Ingest rejected (HTTP %d): %s", resp.status_code, resp.text)
    except requests.RequestException as exc:
        log.warning("Could not reach server (%s). Packet dropped.", exc)


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(description="UART → HTTP connector for SensorTag data")
    parser.add_argument("--port", help="Serial port, e.g. COM3 or /dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument("--url", default="http://localhost:5000/api/ingest",
                        help="Flask ingest endpoint URL")
    parser.add_argument("--mock", action="store_true",
                        help="Run in mock mode (no hardware required)")
    parser.add_argument("--id", dest="device_id", type=str, default=None,
                        help="Device ID for mock mode (default: all baselines)")
    args = parser.parse_args()

    if args.mock:
        run_mock(args.url, args.device_id)
    elif args.port:
        run_serial(args.port, args.baud, args.url)
    else:
        parser.error("Provide --port <PORT> for real hardware or --mock for simulated data.")


if __name__ == "__main__":
    main()
