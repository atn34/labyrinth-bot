import socket
import struct
import sys
import time
from threading import Condition, Lock, Thread

import RPi.GPIO as GPIO


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


DELTA_T = 0.001


class Stepper(object):
    def __init__(self, direction_pin, step_pin):
        self.direction_pin = direction_pin
        self.step_pin = step_pin
        self.current_value = 0
        self.target_value = 0
        self.lock = Lock()
        self.cv = Condition(self.lock)
        self.running = True

    def stop(self):
        with self.lock:
            self.running = False
            self.cv.notify()

    def set_target_value(self, value):
        with self.lock:
            self.target_value = value
            self.cv.notify()

    def move_to_target_forever(self):
        with self.lock:
            while self.running:
                while self.running and self.current_value == self.target_value:
                    self.cv.wait()
                while self.running and self.current_value != self.target_value:
                    if self.current_value > self.target_value:
                        self.direction_pin.set(0)
                        self.current_value -= 1
                    else:
                        self.direction_pin.set(1)
                        self.current_value += 1
                    self.step_pin.switch()
                    self.lock.release()
                    time.sleep(DELTA_T)
                    self.lock.acquire()


HORIZONTAL = 0
VERTICAL = 1

def main():

    # Socket stuff based on
    # https://stackoverflow.com/questions/15869158/python-socket-listening
    s = socket.socket(socket.AF_INET,
                      socket.SOCK_STREAM)
    port = 10000
    s.bind(('', port))
    s.listen(1)

    try:
        GPIO.setmode(GPIO.BCM)

        steppers = [
            Stepper(OutPin(6), OutPin(16)), # HORIZONTAL
            Stepper(OutPin(24), OutPin(22)), # VERTICAL
        ]

        threads = [Thread(target=stepper.move_to_target_forever) for stepper in steppers]

        for thread in threads:
            thread.start()

        while True:
            clientSocket, addr = s.accept()
            print 'got a connection from ' + str(addr)
            while True:
                instruction = clientSocket.recv(8)
                if not instruction:
                    break
                (motor, target_value) = struct.unpack('ii', instruction)
                assert motor in (HORIZONTAL, VERTICAL)
                steppers[motor].set_target_value(target_value)
            for stepper in steppers:
                stepper.set_target_value(0)
                stepper.set_target_value(0)

    finally:
        GPIO.cleanup()
        for stepper in steppers:
            stepper.stop()
        for thread in threads:
            thread.join()


if __name__ == "__main__":
    main()
