from threading import Thread
import RPi.GPIO as GPIO
import struct
import sys
import time

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

delta_t = 0.001

class Stepper(object):

    def __init__(self, direction_pin, step_pin):
        self.direction_pin = direction_pin
        self.step_pin = step_pin
        self.value = None

    def do_steps(self, value):
        if value > 0:
            self.direction_pin.set(1)
        elif value < 0:
            self.direction_pin.set(0)
            value = -value
        else:
            return
        for _ in xrange(value):
            self.step_pin.switch()
            time.sleep(delta_t)

HORIZONTAL = 0
VERTICAL = 1

try:
    GPIO.setmode(GPIO.BCM)

    horizontal_stepper = Stepper(OutPin(6), OutPin(16))
    vertical_stepper = Stepper(OutPin(24), OutPin(22))

    while True:
        instruction = sys.stdin.read(8)
        if not instruction:
            break
        (motor, value) = struct.unpack('ii', instruction)
        if motor == HORIZONTAL:
            thread = Thread(target=horizontal_stepper.do_steps, args=(value,))
            thread.start()
        elif motor == VERTICAL:
            thread = Thread(target=vertical_stepper.do_steps, args=(-value,))
            thread.start()

finally:
    GPIO.cleanup()
