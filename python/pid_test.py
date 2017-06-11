import matplotlib.pyplot as plt
import math

samples = 1000

class plant:

   def __init__(self):
      self.y = 0.0
      self.a = 0.95

   def __call__(self, input):
      self.y = (self.y * self.a) + (input * (1.0 - self.a))
      return self.y * 0.7 # lossy

class pid:

   def __init__(self):
      self.dt = 0.001

      self.p = 0.05
      self.i = 0.5
      self.d = 0.001

      self.error = 0.0
      self.integral = 0.0

   def __call__(self, set_point, process_val):
      error = set_point - process_val

      p_ = self.p * error

      self.integral += (error * self.dt)
      i_ = self.i * self.integral

      derivative = (error - self.error) / self.dt;
      d_ = self.d * derivative;
      self.error = error;

      return p_ + i_ + d_;

val = 0.0
the_pid = pid()
the_plant_in = plant()
the_plant = plant()

input = []
plant_result_in = []
plant_result = []

for x in range(samples):
   plant_result_in.append(the_plant_in(1.0))
   s = the_plant(val)
   plant_result.append(s);
   val += the_pid(1.0, s)
   input.append(val);

ax = plt.subplot(111)
ax.grid(True)
ax.plot(range(samples), input)
ax.plot(range(samples), plant_result)
ax.plot(range(samples), plant_result_in)
plt.show()

