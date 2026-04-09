import threading
import time

import cv2


class StreamReader:
    def __init__(self, stream_url: str) -> None:
        self._stream_url = stream_url
        self._running = False
        self._thread: threading.Thread | None = None
        self._lock = threading.Lock()
        self._latest_frame = None

    def start(self) -> None:
        if self._running:
            return
        self._running = True
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        self._running = False
        if self._thread and self._thread.is_alive():
            self._thread.join(timeout=1.0)

    def get_latest_frame(self):
        with self._lock:
            return None if self._latest_frame is None else self._latest_frame.copy()

    def _run(self) -> None:
        cap = None
        while self._running:
            if cap is None or not cap.isOpened():
                cap = cv2.VideoCapture(self._stream_url)
                time.sleep(0.1)
                continue

            ok, frame = cap.read()
            if not ok or frame is None:
                cap.release()
                cap = None
                time.sleep(0.1)
                continue

            with self._lock:
                self._latest_frame = frame

        if cap is not None:
            cap.release()
