from typing import List
import lcm
import pygame

from messages import LaserScan
from messages import SteerCommand

pygame.init()
lc = lcm.LCM()

SIZE = pygame.display.Info().current_h * 0.9
display = pygame.display.set_mode((SIZE, SIZE))

msg: LaserScan | None = None
last_cmd = SteerCommand()
sub: lcm.LCMSubscription | None = None

show_filtered = False

def subscribe():
    global sub

    if sub:
        lc.unsubscribe(sub)
    sub = lc.subscribe("filtered_scan" if show_filtered else "laser_scan", handler)


def draw():
    if msg == None:
        return

    max_dist = 10000

    display.fill((0, 0, 0))

    for point in msg.points:
        x = (point.x / max_dist + 1) * SIZE / 2
        y = (point.y / max_dist + 1) * SIZE / 2

        pygame.draw.circle(display, (255, 0, 255), (x, y), 2)

    pygame.draw.circle(display, (255, 255, 255), (SIZE/2, SIZE/2), 5)
    pygame.display.update()

def steer():
    global last_cmd

    cmd = SteerCommand()
    keys = pygame.key.get_pressed()

    if keys[pygame.K_UP]:
        cmd.speed = 100
    if keys[pygame.K_DOWN]:
        cmd.speed = -100
    if keys[pygame.K_LEFT]:
        cmd.turn = -1
    if keys[pygame.K_RIGHT]:
        cmd.turn = 1

    if cmd.turn != last_cmd.turn or cmd.speed != last_cmd.speed:
        lc.publish("steer_command", cmd.encode())
        last_cmd = cmd


def loop():
    global show_filtered

    # lc.handle()
    lc.handle_timeout(5)

    for event in pygame.event.get():
        if event.type == pygame.KEYUP and event.key == pygame.K_SPACE:
            show_filtered = not show_filtered
            subscribe()
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()

    draw()
    steer()


def handler(channel, data):
    global msg
    msg = LaserScan.decode(data)


try:
    subscribe()
    while True:
        loop()
except Exception as ex:
    print(f"error: {ex}")
    pass
