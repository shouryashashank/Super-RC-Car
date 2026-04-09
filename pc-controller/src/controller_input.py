from dataclasses import dataclass

import pygame


@dataclass(slots=True)
class ControlState:
    throttle: float
    steering: float
    brake: bool


class KeyboardController:
    def read(self, keys: pygame.key.ScancodeWrapper) -> ControlState:
        throttle = 0.0
        steering = 0.0

        if keys[pygame.K_w] or keys[pygame.K_UP]:
            throttle += 1.0
        if keys[pygame.K_s] or keys[pygame.K_DOWN]:
            throttle -= 1.0
        if keys[pygame.K_d] or keys[pygame.K_RIGHT]:
            steering += 1.0
        if keys[pygame.K_a] or keys[pygame.K_LEFT]:
            steering -= 1.0

        brake = bool(keys[pygame.K_SPACE])
        if brake:
            throttle = 0.0
            steering = 0.0

        return ControlState(throttle=throttle, steering=steering, brake=brake)
