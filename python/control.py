import lcm
import pygame
from messages import SteerCommand

pygame.init()

lc = lcm.LCM()

running = True

last_cmd = SteerCommand()

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()

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

    if cmd.get_hash() == last_cmd.get_hash():
        continue

    lc.publish("steer_command", cmd.encode())


