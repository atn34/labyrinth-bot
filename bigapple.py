import RPi.GPIO as GPIO
import time
import sys
from itertools import chain
from collections import deque

GPIO.setmode(GPIO.BCM)

DirectionPins = [6, 24]
StepPins = [16, 22]

for pin in chain(DirectionPins, StepPins):
	GPIO.setup(pin, GPIO.OUT)
	GPIO.output(pin, 0)

time_between_up_and_down = 0.001

while True:
	for DirectionPin in DirectionPins:
		GPIO.output(DirectionPin, 1)
	for i in xrange(100):
		for StepPin in StepPins:
			GPIO.output(StepPin, 1)
		time.sleep(time_between_up_and_down)
		for StepPin in StepPins:
			GPIO.output(StepPin, 0)
		time.sleep(time_between_up_and_down)
	for DirectionPin in DirectionPins:
		GPIO.output(DirectionPin, 0)
	time.sleep(0.01)
	for i in xrange(200):
		for StepPin in StepPins:
			GPIO.output(StepPin, 1)
		time.sleep(time_between_up_and_down)
		for StepPin in StepPins:
			GPIO.output(StepPin, 0)
		time.sleep(time_between_up_and_down)
	for DirectionPin in DirectionPins:
		GPIO.output(DirectionPin, 1)
	time.sleep(0.011)
	for i in xrange(100):
		for StepPin in StepPins:
			GPIO.output(StepPin, 1)
		time.sleep(time_between_up_and_down)
		for StepPin in StepPins:
			GPIO.output(StepPin, 0)
		time.sleep(time_between_up_and_down)
	time.sleep(1)


GPIO.cleanup()
