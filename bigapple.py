import RPi.GPIO as GPIO
import struct
import sys
import time

GPIO.setmode(GPIO.BCM)

class OutPin(object):

    def __init__(self, bcm):
        self.bcm = bcm
        GPIO.setup(self.bcm, GPIO.OUT)
        GPIO.output(self.bcm, 0)
        self.val = 0

    def set(self, val):
        if self.val != val:
            self.val = val
            GPIO.output(self.bcm, self.val)

    def switch(self):
        self.val = 1 - self.val
        GPIO.output(self.bcm, self.val)

class Stepper(object):

    def __init__(self, direction_pin, step_pin):
        self.direction_pin = direction_pin
        self.step_pin = step_pin

    def step_up(self):
        self.direction_pin.set(1)
        self.step_pin.switch()

    def step_down(self):
        self.direction_pin.set(0)
        self.step_pin.switch()

horizontal_stepper = Stepper(OutPin(6), OutPin(16))
vertical_stepper = Stepper(OutPin(24), OutPin(22))

delta_t = 0.001

# Instructions

HORIZONTAL_STEP_CC  = 0b00000001
HORIZONTAL_STEP_CCW = 0b00000010
VERTICAL_STEP_CC    = 0b00000100
VERTICAL_STEP_CCW   = 0b00001000

DirectionPinValues = [0, 0]
StepPinValues = [0, 0]

while True:
    raw_instruction = sys.stdin.read(1)
    start = time.time()
    if not raw_instruction:
        break
    (instruction,) = struct.unpack('B', raw_instruction)
    if instruction & HORIZONTAL_STEP_CC:
        horizontal_stepper.step_up()
    if instruction & HORIZONTAL_STEP_CCW:
        horizontal_stepper.step_down()
    if instruction & VERTICAL_STEP_CC:
        vertical_stepper.step_up()
    if instruction & VERTICAL_STEP_CCW:
        vertical_stepper.step_down()
    time.sleep(delta_t)

GPIO.cleanup()
