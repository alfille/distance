# distance
**Find average distance between random points in an N-dimensional cube.**

This is a fun math exercise.

## One dimension:
![line](images/1D.png)

Consider a pair of point chosen randomly on a unit line segment:

* What is the average distance between them?
* Can you prove it mathematically?
* Can you test it by trying a lot of random points?

### Mathematically:
![1D](images/1Deqn.png) 
Plug into [maxima](http://maxima.cesga.es/):
> integrate((integrate(b-a,b,a,1)+integrate(a-b,b,0,a)),a,0,1);
> 
> (%o1)	1/3

## Two Dimensions
Consider 2 random points in a unit square:


