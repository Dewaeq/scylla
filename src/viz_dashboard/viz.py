import sys
import os
import math
import lcm
import pygame
import time

from scylla_msgs import lidar_t
from scylla_msgs import drive_command_t

SPEED = 0.5
MAX_DIST_M = 8.0
SCALE = 0
SIZE = 800
CENTER = (SIZE // 2, SIZE // 2)

pygame.init()
lc = lcm.LCM()
screen = pygame.display.set_mode((SIZE, SIZE))
display = pygame.surface.Surface((SIZE, SIZE), pygame.SRCALPHA)
font = pygame.font.SysFont("monospace", 18)

msg_lidar = None
last_cmd = drive_command_t()
last_cmd.timestamp = 0
last_cmd.throttle = 0.0
last_cmd.steering = 0.0


def lidar_handler(channel, data):
    global msg_lidar
    msg_lidar = lidar_t.decode(data)


sub = lc.subscribe("sensors/lidar", lidar_handler)


def draw_grid():
    # draw circles every meter
    for i in range(1, int(MAX_DIST_M) + 1):
        radius = int(i * SCALE)
        pygame.draw.circle(display, (255, 255, 255, 50), CENTER, radius, 1)
        label = font.render(f"{i}m", True, (200, 200, 200))
        display.blit(label, (CENTER[0] + 5, CENTER[1] - radius - 10))

    pygame.draw.line(display, (0, 255, 0, 100), CENTER, (SIZE, CENTER[1]), 2)
    pygame.draw.line(display, (255, 0, 0, 100), CENTER, (CENTER[0], 0), 2)


def draw_lidar():
    if msg_lidar is None:
        return

    info = font.render(f"Points: {msg_lidar.num_points}", True, (255, 255, 0))
    display.blit(info, (10, 10))

    for i in range(msg_lidar.num_points):
        dist = msg_lidar.ranges[i]
        angle = msg_lidar.angles[i]

        if dist < 0.1 or dist > MAX_DIST_M:
            continue

        x_m = dist * math.cos(angle)
        y_m = dist * math.sin(angle)

        px = CENTER[0] + int(y_m * SCALE)
        py = CENTER[1] - int(x_m * SCALE)

        intensity = max(50, min(255, int(255 * (1.0 - dist / MAX_DIST_M))))
        display.set_at((px, py), (255, 0, 255, intensity))


def process_input():
    global last_cmd
    cmd = drive_command_t()
    cmd.timestamp = int(time.time() * 1000000)

    keys = pygame.key.get_pressed()

    if keys[pygame.K_UP]:
        cmd.throttle = 1.0 * SPEED
    elif keys[pygame.K_DOWN]:
        cmd.throttle = -1.0 * SPEED
    else:
        cmd.throttle = 0.0

    if keys[pygame.K_LEFT]:
        cmd.steering = -1.0
    elif keys[pygame.K_RIGHT]:
        cmd.steering = 1.0
    else:
        cmd.steering = 0.0

    # only publish on change or after 100ms
    if (
        (cmd.timestamp - last_cmd.timestamp >= 100e3)
        or (cmd.throttle != last_cmd.throttle)
        or (cmd.steering != last_cmd.steering)
    ):
        lc.publish("drive_command", cmd.encode())
        last_cmd = cmd


def main():
    global SCALE
    SCALE = (SIZE / 2) / MAX_DIST_M

    running = True
    while running:
        lc.handle_timeout(5)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        screen.fill((0, 0, 0))
        display.fill((0, 0, 0))

        draw_grid()
        draw_lidar()

        screen.blit(display, (0, 0))
        pygame.display.flip()
        process_input()

    pygame.quit()


if __name__ == "__main__":
    main()
