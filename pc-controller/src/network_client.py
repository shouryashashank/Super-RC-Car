import time
from dataclasses import dataclass

import httpx


@dataclass(slots=True)
class NetworkSnapshot:
    last_ok: bool
    last_error: str
    applied_seq: int
    last_send_ms: int


class MotorApiClient:
    def __init__(self, base_url: str) -> None:
        self._client = httpx.Client(base_url=base_url, timeout=0.25)
        self._seq = 0
        self._last_ok = False
        self._last_error = ""
        self._applied_seq = 0
        self._last_send_ms = 0

    def send_motors(self, wheels: dict[str, int], ttl_ms: int) -> bool:
        self._seq += 1
        payload = {
            "fl": int(wheels["fl"]),
            "fr": int(wheels["fr"]),
            "rl": int(wheels["rl"]),
            "rr": int(wheels["rr"]),
            "seq": self._seq,
            "ttl_ms": int(ttl_ms),
        }

        t0 = time.perf_counter()
        try:
            response = self._client.post("/api/v1/motors", json=payload)
            response.raise_for_status()
            body = response.json()
            self._applied_seq = int(body.get("applied_seq", self._seq))
            self._last_ok = bool(body.get("ok", True))
            self._last_error = ""
        except Exception as ex:
            self._last_ok = False
            self._last_error = str(ex)
        self._last_send_ms = int((time.perf_counter() - t0) * 1000)
        return self._last_ok

    def stop(self) -> bool:
        try:
            response = self._client.post("/api/v1/stop")
            response.raise_for_status()
            body = response.json()
            self._last_ok = bool(body.get("ok", True))
            self._last_error = ""
            return self._last_ok
        except Exception as ex:
            self._last_ok = False
            self._last_error = str(ex)
            return False

    def health(self) -> dict | None:
        try:
            response = self._client.get("/api/v1/health")
            response.raise_for_status()
            return response.json()
        except Exception as ex:
            self._last_error = str(ex)
            return None

    def snapshot(self) -> NetworkSnapshot:
        return NetworkSnapshot(
            last_ok=self._last_ok,
            last_error=self._last_error,
            applied_seq=self._applied_seq,
            last_send_ms=self._last_send_ms,
        )
