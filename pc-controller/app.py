import json
from pathlib import Path
from typing import Any

import pygame

from src.controller_input import KeyboardController
from src.mixer import arcade_mix
from src.network_client import MotorApiClient
from src.ui import draw_ui
from src.video_client import StreamReader


def _deep_merge(base: dict[str, Any], override: dict[str, Any]) -> dict[str, Any]:
    result = dict(base)
    for key, value in override.items():
        if isinstance(value, dict) and isinstance(result.get(key), dict):
            result[key] = _deep_merge(result[key], value)
        else:
            result[key] = value
    return result


def load_config() -> dict[str, Any]:
    root = Path(__file__).resolve().parent
    default_path = root / "config.example.json"
    config_path = root / "config.json"

    with default_path.open("r", encoding="utf-8") as f:
        config = json.load(f)

    if config_path.exists():
        with config_path.open("r", encoding="utf-8") as f:
            user_config = json.load(f)
        config = _deep_merge(config, user_config)
    return config


def main() -> None:
    config = load_config()
    width = int(config["window"]["width"])
    height = int(config["window"]["height"])
    fps = int(config["window"]["fps"])
    send_hz = float(config["control"]["send_hz"])
    watchdog_ms = int(config["control"]["watchdog_ms"])
    max_output = int(config["control"]["max_output"])

    pygame.init()
    pygame.display.set_caption("Super RC Car Desktop Controller")
    screen = pygame.display.set_mode((width, height))
    clock = pygame.time.Clock()

    input_controller = KeyboardController()
    api = MotorApiClient(base_url=config["base_url"])
    stream = StreamReader(stream_url=config["stream_url"])
    stream.start()

    running = True
    command_interval = 1.0 / max(send_hz, 1.0)
    time_since_send = 0.0
    time_since_health = 0.0
    health = None
    wheels = {"fl": 0, "fr": 0, "rl": 0, "rr": 0}

    while running:
        dt = clock.tick(fps) / 1000.0
        time_since_send += dt
        time_since_health += dt

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
                wheels = {"fl": 0, "fr": 0, "rl": 0, "rr": 0}
                api.stop()

        keys = pygame.key.get_pressed()
        control_state = input_controller.read(keys)
        wheels = arcade_mix(
            throttle=control_state.throttle,
            steering=control_state.steering,
            scale=max_output,
        )

        if time_since_send >= command_interval:
            api.send_motors(wheels=wheels, ttl_ms=watchdog_ms)
            time_since_send = 0.0

        if time_since_health >= 1.0:
            health = api.health()
            time_since_health = 0.0

        frame = stream.get_latest_frame()
        draw_ui(
            screen=screen,
            frame=frame,
            control_state=control_state,
            wheels=wheels,
            net_state=api.snapshot(),
            health=health,
        )
        pygame.display.flip()

    api.stop()
    stream.stop()
    pygame.quit()


if __name__ == "__main__":
    main()
