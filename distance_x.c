#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

void help( void )
{
    printf("distance_x -- find the average distance between random points in\n") ;
    printf("\ta unit N-cube using Monti Carlo method.\n");
    printf("\n");
    printf("By Paul H Alfille 2021 -- MIT license\n") ;
    printf("This version uses Mersenne Twister code found at https://github.com/cslarsen/mersenne-twister\n") ;
    printf("\n");
    printf("Output is CSV file format to make easy manipulation.\n");
    printf("A number of metrics are used including\n");
    printf("\t1\t(Manhattan) |x1-x2| in each dimension\n");
    printf("\t2\t(Euclidean) sqrt(sum((x1-x2)^2)) \n");
    printf("\t3\t[sum((x1-x2)^3)]^1/3 \n");
    printf("\n");
    printf("Syntax:\n");
    printf("\tdistance [options]\n");
    printf("Options:\n");
    printf("\t-d 100\tmax dimensions\n");
    printf("\t-p 3\tmax power (metric)\n");
    printf("\t-r 1000000\trandom points each measure\n");
    printf("\t-n\tnormalize (to longest diagonal)\n");
    printf("\t-h\tthis help\n");
    exit(0) ;
}

/* Includes for Mersenne Twister code
 * from http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/VERSIONS/C-LANG/mt19937-64.c
 * Some extra functions removed
 */
double genrand64_real1(void) ;

// Separate mantissa and exponent
// can be a little loose
// This is an object-oriented approach to C
//
// uses the C-library frexp and ldexp functions with give mantissa and 2^exponent
struct Exp {
    double m ; // mantissa
    int e ; // exponent
} ;

// s -> E
#define ExpEncode( scalar, res_exp ) do { \
    (res_exp).m = frexp( (scalar) , &( (res_exp).e) ) ; \
    } while (0)
    
// E -> s
#define ExpDecode( _exp ) ldexp( (_exp).m, (_exp).e ) 
    
void _ExpPrint( char * s, struct Exp * pexp ) {
    printf( "%s %g : %d = %g\n",s,pexp->m,pexp->e,ExpDecode(*pexp) );
}

// E displayed (wrapper of _ExpPrint to avoid pointer syntax in use)
#define ExpPrint( string, _exp ) _ExpPrint( (string) , & (_exp) )

// multiply mantissa, and calculate new expoment
// E * s -> E
#define ExpMult( _exp, scalar, res_exp ) do { \
    int e ; \
    (res_exp).m = frexp( (_exp).m * (scalar) , &e ) ; \
    (res_exp).e = e + (_exp).e ; \
    } while (0)
    
// shift 'b' to 'a' exponent, add, and calculate new exponent from offset
// E + E -> E
#define ExpAdd( a_exp, b_exp, res_exp ) do { \
    int e ; \
    if ( (a_exp).e > (b_exp).e ) { \
        (res_exp).m = frexp( (a_exp).m + ldexp( (b_exp).m, (b_exp).e - (a_exp).e ) , &e ) ;\
        (res_exp).e = e + (a_exp).e; \
    } else { \
        (res_exp).m = frexp( (b_exp).m + ldexp( (a_exp).m, (a_exp).e - (b_exp).e ) , &e ) ;\
        (res_exp).e = e + (b_exp).e; \
    } \
    } while (0)

// get fancy -- take integer part of exponent/(root) separately
// E ** s -> s
#define ExpRoot( _exp, root, result ) do {\
    int e, mult = (_exp).e / (root) ; \
    result = ldexp( pow( ldexp( (_exp).m, (_exp).e % (root) ), 1./(root) ) , mult ) ; \
    } while (0)
    
int main( int argc, char **argv )
{
    int Dimensions = 100 ;
    int Powers = 3 ;
    long Randoms = 1000000 ;
    int Normalize = 0;

    // Arguments
    int c;
    while ( (c = getopt( argc, argv, "hd:p:r:n" )) != -1 ) {
        switch ( c ) {
        case 'h':
            help() ;
            break ;
        case 'd':
            Dimensions = atoi(optarg);
            if (Dimensions<1) {
                Dimensions = 1 ;
            }
            break ;
        case 'p':
            Powers = atoi(optarg);
            if (Powers<3) {
                Powers = 3 ;
            }
            break ;
        case 'r':
            Randoms = atoi(optarg);
            if (Randoms<1000) {
                Randoms = 1000 ;
            }
            break ;
        case 'n':
            Normalize = 1 ;
            break ;
        }
    }

    // Initialize totals to zero
    double totals[Dimensions+1][Powers];
    int d,p;
    for (d=0; d <= Dimensions; ++d) {
        for (p=0; p<Powers; ++p) {
            totals[d][p] = 0.;
        }
    }

    // Title line
    printf("DIM\\Power, ");
    for (p=0;p<Powers;++p) {
        printf("%d, ",p+1);
    }
    printf("\n");

    // Basically the algorhythm is to compute a dx for each dimension
    // then compute the series dx, dx^2, dx^3 ... (easy interative multiplication)
    // compute sequentially dx^n for each dimension
    // then take ^1/p  for each p and sum.

    struct Exp sums[Dimensions+1][Powers];
    for ( p=0; p<Powers ; ++p ) {
        ExpEncode( 0., sums[0][p] ) ;
    }

    // Generate and add up sums of coordinate differences at various dimensions
    // from two randomly generated points in the hypercube.
    for (long r = 0; r < Randoms; ++r) {
        // fill in the sum of powers for a single sample at various dimensions
        // and powers

        for (d=1; d <= Dimensions; ++d) {
            // For each dimension, get 2 (random) coordinates for this dimension (for each end of the line segment)
            // we only care about dx, the delta in the coordinate
            // use absolute value for odd powers calculation
            double dx ;
            dx = fabs( genrand64_real1() - genrand64_real1() );

            // for each power, the sum will be the entry from the row above
            // plus dx raised to that power.
            struct Exp dx_raised ;
            ExpEncode( 1., dx_raised ) ; // 0-th power
                        
            for (p=0; p<Powers; ++p) {

                // Multiple by dx to get dx^p
                ExpMult( dx_raised, dx, dx_raised ) ;

                // Add the dimension to random segment of prior dimension
                ExpAdd( sums[d-1][p], dx_raised, sums[d][p] ) ;

                // Add the pth root (p-norm) of each vector to the totals
                // get fancy -- take integer part of exponent/(p+1) separately
                double length ; // segment length (in p-norm)
                ExpRoot( sums[d][p], p+1, length ) ;
                totals[d][p] += length;
            }
        }
    }

    // Loop though solutions for averaging (norming) and display
    for (d=1; d <= Dimensions; ++d) {
        // Start line with dimension
        printf("%d, ",d);

        // Print out the distances
        for (p=0;p<Powers;++p) {
            if (Normalize) {
                for (p=0;p<Powers;++p){
                    // note p is 0-indexed in C, but 1-indexed for calculation
                    printf("%g, ", totals[d][p]/Randoms/pow(d,1./(1+p)));
                }
            } else {
                for (p=0;p<Powers;++p){
                    printf("%g, ", totals[d][p]/Randoms);
                }
            }
            // finish line
            printf("\n");
        }
    }

    // success
    return 0 ;
}

/* 
   A C-program for MT19937-64 (2004/9/29 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using init_genrand64(seed)  
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and 
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and 
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */


/* The array for the state vector */
static unsigned long long mt[NN]; 
/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1; 

/* initializes mt[NN] with a seed */
void init_genrand64(void)
{
    mt[0] = 0xA542B234C76 | (unsigned long long) time(NULL); // Use time() for seed -- not great but this isn't crypto
    for (mti=1; mti<NN; mti++) 
        mt[mti] =  (6364136223846793005ULL * (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
}

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(void)
{
    int i;
    unsigned long long x;
    static unsigned long long mag01[2]={0ULL, MATRIX_A};

    if (mti >= NN) { /* generate NN words at one time */

        /* if init_genrand64() has not been called, */
        /* a default initial seed is used     */
        if (mti == NN+1) 
            init_genrand64(); 

        for (i=0;i<NN-MM;i++) {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        for (;i<NN-1;i++) {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        x = (mt[NN-1]&UM)|(mt[0]&LM);
        mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];

        mti = 0;
    }
  
    x = mt[mti++];

    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);

    return x;
}

/* generates a random number on [0,1]-real-interval */
double genrand64_real1(void)
{
    return (genrand64_int64() >> 11) * (1.0/9007199254740991.0);
}
