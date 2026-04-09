from __future__ import annotations

import pygame
import numpy as np
import cv2


BG = (9, 12, 16)
PANEL = (18, 24, 32)
TEXT = (220, 228, 238)
ACCENT = (37, 212, 152)
WARN = (255, 79, 79)


def _font(size: int, bold: bool = False) -> pygame.font.Font:
    return pygame.font.SysFont("consolas", size, bold=bold)


def _draw_bar(screen: pygame.Surface, rect: pygame.Rect, value: float, color: tuple[int, int, int]) -> None:
    pygame.draw.rect(screen, (36, 44, 56), rect, border_radius=6)
    center_x = rect.x + rect.w // 2
    pygame.draw.line(screen, (66, 76, 92), (center_x, rect.y), (center_x, rect.y + rect.h), 1)

    amount = max(-1.0, min(1.0, value))
    if amount >= 0:
        fill = pygame.Rect(center_x, rect.y + int((1.0 - amount) * rect.h), rect.w // 2, int(amount * rect.h))
    else:
        fill = pygame.Rect(center_x + int(amount * (rect.w // 2)), rect.y, int(-amount * (rect.w // 2)), rect.h)
    pygame.draw.rect(screen, color, fill, border_radius=4)


def _frame_surface(frame, target_size: tuple[int, int]) -> pygame.Surface:
    if frame is None:
        surf = pygame.Surface(target_size)
        surf.fill((20, 24, 32))
        return surf

    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    rgb = np.transpose(rgb, (1, 0, 2))
    surf = pygame.surfarray.make_surface(rgb)
    return pygame.transform.smoothscale(surf, target_size)


def draw_ui(
    screen: pygame.Surface,
    frame,
    control_state,
    wheels: dict[str, int],
    net_state,
    health: dict | None,
) -> None:
    w, h = screen.get_size()
    screen.fill(BG)

    pygame.draw.rect(screen, PANEL, (0, 0, w, h), border_radius=0)
    pygame.draw.rect(screen, (26, 36, 48), (0, 0, w, 70))

    title = _font(30, bold=True).render("SUPER RC // DESKTOP PILOT", True, ACCENT)
    screen.blit(title, (24, 18))

    video_rect = pygame.Rect(24, 90, int(w * 0.68), h - 120)
    hud_rect = pygame.Rect(video_rect.right + 16, 90, w - video_rect.right - 40, h - 120)

    frame_surf = _frame_surface(frame, (video_rect.w, video_rect.h))
    screen.blit(frame_surf, video_rect)
    pygame.draw.rect(screen, (56, 72, 90), video_rect, width=2)

    pygame.draw.rect(screen, (14, 19, 26), hud_rect, border_radius=8)
    pygame.draw.rect(screen, (48, 62, 78), hud_rect, width=2, border_radius=8)

    small = _font(20, bold=True)
    normal = _font(18)

    status_text = "ONLINE" if net_state.last_ok else "LINK LOST"
    status_color = ACCENT if net_state.last_ok else WARN
    screen.blit(small.render(f"NET: {status_text}", True, status_color), (hud_rect.x + 16, hud_rect.y + 16))
    screen.blit(normal.render(f"RTT: {net_state.last_send_ms} ms", True, TEXT), (hud_rect.x + 16, hud_rect.y + 50))
    screen.blit(normal.render(f"SEQ: {net_state.applied_seq}", True, TEXT), (hud_rect.x + 16, hud_rect.y + 76))

    throttle_bar = pygame.Rect(hud_rect.x + 18, hud_rect.y + 120, 42, 180)
    steering_bar = pygame.Rect(hud_rect.x + 78, hud_rect.y + 120, 120, 28)
    _draw_bar(screen, throttle_bar, control_state.throttle, ACCENT)
    _draw_bar(screen, steering_bar, control_state.steering, (80, 164, 255))

    screen.blit(normal.render("THROTTLE", True, TEXT), (throttle_bar.x - 2, throttle_bar.bottom + 8))
    screen.blit(normal.render("STEERING", True, TEXT), (steering_bar.x, steering_bar.bottom + 8))

    y = hud_rect.y + 350
    for key in ("fl", "fr", "rl", "rr"):
        value = wheels[key]
        color = ACCENT if value >= 0 else WARN
        row = normal.render(f"{key.upper()}: {value:+4d}", True, color)
        screen.blit(row, (hud_rect.x + 16, y))
        y += 28

    if health:
        ip = health.get("ip", "n/a")
        rssi = health.get("wifi_rssi", "n/a")
        up = health.get("uptime_ms", 0)
        screen.blit(normal.render(f"IP: {ip}", True, TEXT), (hud_rect.x + 16, hud_rect.bottom - 88))
        screen.blit(normal.render(f"RSSI: {rssi}", True, TEXT), (hud_rect.x + 16, hud_rect.bottom - 62))
        screen.blit(normal.render(f"UPTIME: {up} ms", True, TEXT), (hud_rect.x + 16, hud_rect.bottom - 36))

    hint = "Drive: W/A/S/D | E-Stop: SPACE | Quit: ESC/Window"
    screen.blit(normal.render(hint, True, (176, 190, 206)), (24, h - 24))
