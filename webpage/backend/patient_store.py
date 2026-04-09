"""
patient_store.py
----------------
Thread-safe in-memory store for patient sensor readings.

Data enters exclusively via the `ingest()` function, which is called by the
Flask POST /api/ingest endpoint. Hardware I/O lives entirely in uart_connector.py.

Public API:
    ingest(raw)                      - store one parsed JSON packet
    get_all_patients() -> list       - [{did, last_seen, latest}, ...]
    get_patient(did) -> dict|None    - latest reading for one patient
    get_patient_history(did, n) -> list|None
    get_pill_status(did, interval_seconds) -> dict|None
    confirm_pill(did) -> dict
"""

import threading
from collections import deque
from datetime import datetime, timezone

RING_BUFFER_SIZE = 50

_lock: threading.Lock = threading.Lock()
_store: dict = {}


def _now_iso() -> str:
    return datetime.now(timezone.utc).isoformat(timespec="milliseconds").replace("+00:00", "Z")


def _now_dt() -> datetime:
    return datetime.now(timezone.utc)


def ingest(raw: dict) -> None:
    """Stamp a timestamp on a parsed packet and write it into the store."""
    did = raw.get("id")
    if did is None:
        return

    ts = _now_iso()
    reading = {**raw, "ts": ts}

    with _lock:
        if did not in _store:
            _store[did] = {
                "last_seen": ts,
                "latest": reading,
                "history": deque(maxlen=RING_BUFFER_SIZE),
                "pill_confirmed_at": None,
            }
        _store[did]["last_seen"] = ts
        _store[did]["latest"] = reading
        _store[did]["history"].append(reading)


def get_all_patients() -> list:
    """Return [{did, last_seen, latest}, ...] sorted by did."""
    with _lock:
        return [
            {
                "id": did,
                "last_seen": entry["last_seen"],
                "latest": entry["latest"],
            }
            for did, entry in sorted(_store.items())
        ]


def get_patient(did: str) -> dict | None:
    """Return the latest reading for patient `did`, or None if unknown."""
    with _lock:
        entry = _store.get(did)
        if entry is None:
            return None
        return entry["latest"]


def get_patient_history(did: str, n: int = 50) -> list | None:
    """
    Return the last `n` readings for patient `did` (oldest first),
    or None if the patient is unknown.
    """
    with _lock:
        entry = _store.get(did)
        if entry is None:
            return None
        snapshot = list(entry["history"])
    return snapshot[-n:]


def get_pill_status(did: str, interval_seconds: int) -> dict | None:
    """
    Return pill timer status for patient `did`, or None if unknown.
    Result: { overdue, minutes_remaining, last_confirmed }
    minutes_remaining is negative when overdue.
    """
    with _lock:
        entry = _store.get(did)
        if entry is None:
            return None
        confirmed_at = entry["pill_confirmed_at"]

    now = _now_dt()

    if confirmed_at is None:
        return {
            "overdue": True,
            "minutes_remaining": -interval_seconds / 60,
            "last_confirmed": None,
        }

    elapsed_seconds = (now - confirmed_at).total_seconds()
    remaining_seconds = interval_seconds - elapsed_seconds
    remaining_minutes = remaining_seconds / 60

    return {
        "overdue": remaining_seconds <= 0,
        "minutes_remaining": remaining_minutes,
        "last_confirmed": confirmed_at.isoformat(timespec="milliseconds").replace("+00:00", "Z"),
    }


def confirm_pill(did: str, interval_seconds: int) -> dict:
    """Record that a pill was taken now. Creates the patient entry if needed."""
    now = _now_dt()
    with _lock:
        if did not in _store:
            _store[did] = {
                "last_seen": _now_iso(),
                "latest": {},
                "history": deque(maxlen=RING_BUFFER_SIZE),
                "pill_confirmed_at": now,
            }
        else:
            _store[did]["pill_confirmed_at"] = now

    remaining_seconds = interval_seconds
    return {
        "overdue": False,
        "minutes_remaining": remaining_seconds / 60,
        "last_confirmed": now.isoformat(timespec="milliseconds").replace("+00:00", "Z"),
    }
