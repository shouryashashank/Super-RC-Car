def _clamp(value: float, low: float, high: float) -> float:
    return max(low, min(high, value))


def _to_int_percent(value: float, scale: int) -> int:
    return int(round(_clamp(value, -1.0, 1.0) * scale))


def arcade_mix(throttle: float, steering: float, scale: int = 100) -> dict[str, int]:
    left = _clamp(throttle + steering, -1.0, 1.0)
    right = _clamp(throttle - steering, -1.0, 1.0)
    return {
        "fl": _to_int_percent(left, scale),
        "fr": _to_int_percent(right, scale),
        "rl": _to_int_percent(left, scale),
        "rr": _to_int_percent(right, scale),
    }
