import random

# Create a histogram of 1-dim distances
# randomly chosen on unit line segment
#
# Paul H Alfille 2021
# http://github.com/alfille/distance

bins = 100
randoms = 1000 * bins
sum = [0]*(bins+1)

for _ in range(randoms):
  sum[int( bins*abs(random.random()-random.random()) )] += 1

print("bin,count,")
for i in range(bins+1):
  print("{}, {},".format(i,sum[i]/randoms))
