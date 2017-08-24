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

HORIZONTAL_STEP_CC  = 0b00000001
HORIZONTAL_STEP_CCW = 0b00000010
VERTICAL_STEP_CC    = 0b00000100
VERTICAL_STEP_CCW   = 0b00001000

def write_instruction(instruction):
    s.sendall(struct.pack('B', instruction))

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    keys_pressed = pygame.key.get_pressed()

    instruction = 0b00000000

    if keys_pressed[pygame.K_LEFT]:
        instruction |= HORIZONTAL_STEP_CCW
    if keys_pressed[pygame.K_RIGHT]:
        instruction |= HORIZONTAL_STEP_CC
    if keys_pressed[pygame.K_UP]:
        instruction |= VERTICAL_STEP_CC
    if keys_pressed[pygame.K_DOWN]:
        instruction |= VERTICAL_STEP_CCW
    if instruction != 0:
        write_instruction(instruction)
    time.sleep(0.001)
