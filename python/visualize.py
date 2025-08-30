from typing import List
import lcm
import pygame
from math import inf, radians, cos, sin

from messages.LaserScan import LaserScan
from messages.ScanPoint import ScanPoint


SIZE = 600
display = pygame.display.set_mode((600, 600))

msg: LaserScan | None = None

def loop():
    lc.handle()

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()

    if msg == None:
        return

    max_dist = 0
    min_dist = inf
    points: List[ScanPoint] = []

    for point in msg.points:
        if point.distance == 0:
            continue
        max_dist = max(max_dist, point.distance)
        min_dist = min(min_dist, point.distance)
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
