from typing import List
import lcm
import pygame
from math import inf, radians, cos, sin

from messages import LaserScan
from messages import ScanPoint
from messages import SteerCommand


SIZE = 600
display = pygame.display.set_mode((600, 600))

msg: LaserScan | None = None
last_cmd = SteerCommand()

def draw():
    if msg == None:
        return

    max_dist = 12
    points: List[ScanPoint] = []

    for point in msg.points:
        if point.distance == 0:
            continue
        points.append(point)

    display.fill((0, 0, 0))

    for point in points:
        d = point.distance / max_dist * SIZE / 2
        z = radians(point.angle)
        x = d * cos(z) + SIZE / 2
        y = d * sin(z) + SIZE / 2

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
    # lc.handle()
    lc.handle_timeout(5)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()

    draw()
    steer()



def handler(channel, data):
    global msg
    msg = LaserScan.decode(data)


lc = lcm.LCM()
sub = lc.subscribe("laser_scan", handler)


try:
    while True:
        loop()
except Exception as ex:
    print(f"error: {ex}")
    pass
