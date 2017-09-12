import matplotlib.pyplot as plt
import numpy as np

data = np.genfromtxt('log.txt', delimiter=',', names=['touching', 'motor_x',
    'motor_y', 'zero_x', 'zero_y', 'x', 'y', 'vx', 'vy', 'ax', 'ay'])

fig = plt.figure()

plt.plot(data['motor_x'], label='motor_x')
plt.plot(data['zero_x'], label='zero_x')
plt.plot(data['vx'] * 10, label=r'vx * 10')
plt.plot(data['ax'] * 100, label=r'ax * 100')
plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
           ncol=2, mode="expand", borderaxespad=0.)
plt.show()
