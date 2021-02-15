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

![Square](images/2D.png)

There is more than one way to measure the distance between the points:

* Taxicab or Manhattan -- distance in each dimension separately
 * Red in the diagram
 * This makes the problem separable
 * Each dimension adds 1/3 to the total average since each dimension is independent. 
* Euclidean -- the conventional distance
 * Blue in the diagram
 * The equation is non-linear:
 * ![Euclidean](images/2Deuclid.png)
* General [Lp metric](https://en.wikipedia.org/wiki/Lp_space): 
 * ![Lp](images/Lp.png) 
 * Taxicab p=1
 * Euclidean p=2

 ## Higher Dimensions
 ![Cube](images/3D.png)

 * The program can be generalized to arbitrarily higher dimensions
 * One characteristic distance to note is the longest diagonal
  * from point (0,0,0...) to (1,1,1,...)
  * For dimension D and Lp metric p: 
  * diagonal = D^(1/p)

  ## Monti Carlo method
  
* Rather than solving the equtions directly, simulate with random points
 * Each coordinate of each point is evenly distributed from 0.0 to 1.0
 * Distance can be measured using the applicable metric
 * The average distance can be measured

  ![MontiCarlo](images/MontiCarlo.png)
  
# Program
## Getting it
[Github location](https://github.com/alfille/distance)

## Simple installation
```
# Get code
git clone http://github.com/alfille/distance

# Go to program directory
cd distance

# Compile and make executable
make
chmod +x distance

# optional to install
sudo cp distance /usr/bin/distance

# otherwise run from directory
./distance
```
The only requirements are a working C complier and git
Actually, if you download the code you only need any C compiler
`cc -o distance distance.c`

## Usage
`./distance -h`

```
distance -- find the average distance between random points in
	a unit N-cube using Monti Carlo method.

By Paul H Alfille 2021 -- MIT license

Output is CSV file format to make easy manipulation.
A number of metrics are used including
	1	(Manhattan) |x1-x2| in each dimension
	2	(Euclidean) sqrt(sum((x1-x2)^2)) 
	3	[sum((x1-x2)^3)]^1/3 

Syntax:
	distance [options]
Options:
	-d 100	max dimensions
	-p 3	max power (metric)
	-r 1000000	random points each measure
	-n	normalize (to longest diagonal)
	-h	this help

```
The output is a CSV (comma-separated-values) file that can be imported into man programs (e.g. Excel)

## Example
`./distance -p 10 -r 100000 -d 200 -n > Sample.csv`

In fact that result is in the `example` directory

Result:
![](example/Distance.png)

## Precision
Calculations are done in [double precision](https://en.wikipedia.org/wiki/Double-precision_floating-point_format) floating point which limits the exponent to about 10^-300. Since the calculation involves multiplying numbers from 0.0 to 1.0 by themselves, dimension = 200 is probably safe. (i.e. 0.1^200)

## Results
Note that the normalized (to the longest diagonal) appears to reach an assymptotic value for each power at higher dimensions.

![Assymptotic](images/Assymptote.png)

* Here power (p in Lp metric) is on x-axis
* Normalized average distance is y-axis
* Plots after Dim=25 .. Dim=200 are shown
* Since Linf devolves to Max metric, it makes sense that values approach 1.0

# Discussion
If you have references to a better mathematical treatment, please include in the github comments, or to me directly at [paul.alfille@gmail.com](mailto:paul.alfille@gmail.com)
