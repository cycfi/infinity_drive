import matplotlib.pyplot as plt
import math

sps = 16000.0
freq = 200
samples = 500
   
class dc_block:

   def __init__(self, r = 0.98):
      self.r = r
      self.x = 0.0
      self.y = 0.0

   def __call__(self, s):
      self.y = s - self.x + self.r * self.y;
      self.x = s;
      return self.y;

class integrator:

   def __init__(self, g = 0.1):
      self.y = 0
      self.g = g
      
   def __call__(self, s):
      self.y += s
      return self.y * self.g
   
def synth(x, sps):
   o1 = math.sin(2 * math.pi * freq * x / sps)
   o2 = math.sin(2 * math.pi * freq * 2 * x / sps)
   return o1 * 0.45 + o2 * 0.55
      
input = [synth(x, sps) for x in range(samples)]
ax = plt.subplot(211)
ax.set_xticks(range(0, samples, int(sps/freq)/2))
ax.grid(True)
ax.plot(range(samples), input, "k-")

dc_blk = dc_block()
integr = integrator(0.1)
o1 = [integr(dc_blk(y)) for y in input] 
ax = plt.subplot(212)
ax.set_xticks(range(0, samples, int(sps/freq)/2))
ax.grid(True)
ax.plot(range(samples), o1, "k-")

plt.show()

