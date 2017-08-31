import pygame
import socket
import struct
import sys
import time

pygame.init()
size = (800, 600)
screen = pygame.display.set_mode(size)
pygame.display.set_caption("Labyrinth arrow key control")
pygame.key.set_repeat(1, 1)

s = socket.socket(
    socket.AF_INET, socket.SOCK_STREAM)
s.connect(("bigapple", 10000))

HORIZONTAL = 0
VERTICAL = 1

def write_instruction(motor, value):
    s.sendall(struct.pack('ii', motor, value))

stepper_values = [0, 0]

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    keys_pressed = pygame.key.get_pressed()

    instruction = 0b00000000

    if keys_pressed[pygame.K_LEFT]:
        stepper_values[HORIZONTAL] -= 1
        write_instruction(HORIZONTAL, stepper_values[HORIZONTAL])
    if keys_pressed[pygame.K_RIGHT]:
        stepper_values[HORIZONTAL] += 1
        write_instruction(HORIZONTAL, stepper_values[HORIZONTAL])
    if keys_pressed[pygame.K_UP]:
        stepper_values[VERTICAL] -= 1
        write_instruction(VERTICAL, stepper_values[VERTICAL])
    if keys_pressed[pygame.K_DOWN]:
        stepper_values[VERTICAL] += 1
        write_instruction(VERTICAL, stepper_values[VERTICAL])

    time.sleep(0.001)
