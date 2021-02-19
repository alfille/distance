#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

void help( void )
{
    printf("distance -- find the average distance between random points in\n") ;
    printf("\ta unit N-cube using Monti Carlo method.\n");
    printf("\n");
    printf("By Paul H Alfille 2021 -- MIT license\n") ;
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

int main( int argc, char **argv )
{
    int Dimensions = 100 ;
    int Powers = 3 ;
    long Randoms = 1000000 ;
    int Normalize = 0;

    double scale = 1.0 / RAND_MAX ;


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

    // Initialize random seed
    srand(time(0));

    // Title line
    int p ;
    printf("DIM\\Power, ");
    for (p=1;p<=Powers;++p) {
        printf("%d, ",p);
    }
    printf("\n");

    // Loop though dimensions
    int d ;
    for ( d=1 ; d <= Dimensions ; ++d ) {
        double sum[Powers] ;
        int p ;

        // Start line with dimension
        printf("%d, ",d);

        // zero out sums
        for (p=0;p<Powers;++p) {
            sum[p] = 0. ;
        }

        // Random segments (i.e. 2 points of increasing dimension)
        long r ;
        for (r = 0 ; r < Randoms ; ++r ) {

            // Zero out segment
            double seg[Powers];
            for (p=0;p<Powers;++p) {
                seg[p] = 0. ;
            }

            // Add up for this dimension
            int dd ;
            for (dd = 0 ; dd < d ; ++dd) {
                double sx = 1. ;
                // For each dimension, get 2 coordinates in this dimension -- rand() and rand() scaled
                // we only care about dx, the delta in the coordinate
                // use abs value for odd powers calculation
                double dx = fabs((rand() - rand()) * scale ) ;
                // compute increasing powers of dx the easy way
                for (p=0;p<Powers;++p) {
                    sx *= dx ;
                    seg[p] += sx ;
                }
            }

            // Add segments to sum, taking the appropriate root
            sum[0] += seg[0] ; // Manhattan or Taxi
            sum[1] += sqrt(seg[1]) ; // Euclidean
            for (p=2;p<Powers;++p) { // this is why Powers must be 3 or higher
                // note p is 0-indexed in C, but 1-indexed for calculation
                sum[p] += pow(seg[p],1/(1.+p));
            }
        }

        if (Normalize) {
            for (p=0;p<Powers;++p){
                // note p is 0-indexed in C, but 1-indexed for calculation
                printf("%g, ",sum[p]/Randoms/pow(d,1/(1.+p)));
            }
        } else {
            for (p=0;p<Powers;++p){
                printf("%g, ",sum[p]/Randoms);
            }
        }
        // finish line
        printf("\n");
    }

    // success
    return 0 ;
}

