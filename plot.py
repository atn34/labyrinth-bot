#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np

data = np.genfromtxt('log.txt', delimiter=',', names=['motor_x', 'motor_y', 'x', 'y'])

fig = plt.figure()

plt.plot(data['motor_x'], label='motor_x')
plt.plot(data['motor_y'], label='motor_y')
plt.plot(data['x'], label=r'x')
plt.plot(data['y'], label=r'y')
plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
           ncol=2, mode="expand", borderaxespad=0.)
plt.show()
