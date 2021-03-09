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

    // Initialize totals to zero
    double totals[Dimensions+1][Powers];
    int d,p;
    for (d=0; d <= Dimensions; ++d) {
        for (p=0; p<Powers; ++p) {
            totals[d][p] = 0.;
        }
    }

    // zero out the first row (i.e. zero dimensional case) of the sums
    double sums[Dimensions+1][Powers];
    for (p=0; p<Powers; ++p) {
        sums[0][p] = 0.;
    }

    // Generate and add up sums of coordinate differences at various dimensions
    // from two randomly generated points in the hypercube.
    for (long r = 0; r < Randoms; ++r) {
        // fill in the sum of powers for a single sample at various dimensions
        // and powers.
        for (d=1; d <= Dimensions; ++d) {
            // For each dimension, get 2 coordinates in this dimension.
            // we only care about dx, the delta in the coordinate
            // use abs value for odd powers calculation
            double dx, dx_cumprod;
            dx = fabs((rand() - rand()) * scale);

            // for each power, the sum will be the entry from the row above
            // plus dx raised to that power.
            double cumprod = 1;
            for (p=0; p<Powers; ++p) {
                cumprod *= dx;
                sums[d][p] = sums[d-1][p] + cumprod;
            }
        }

        // Add the pth root of each sum to the totals
        for (d=1; d <= Dimensions; ++d) {
            for (p=0; p<Powers; ++p) {
                totals[d][p] += pow(sums[d][p], 1./(p+1));
            }
        }
    }

    // Title line
    printf("DIM\\Power, ");
    for (p=1;p<=Powers;++p) {
        printf("%d, ",p);
    }
    printf("\n");

    // Loop though dimensions
    for (d=1; d <= Dimensions; ++d) {
        // Start line with dimension
        printf("%d, ",d);

        // Print out the distances
        for (p=0;p<Powers;++p) {
            if (Normalize) {
                // note p is 0-indexed in C, but 1-indexed for calculation
                printf("%g, ", totals[d][p]/Randoms/pow(d,1/(1.+p)));
            } else {
                printf("%g, ", totals[d][p]/Randoms);
            }
        }
        // finish line
        printf("\n");
    }

    // success
    return 0 ;
}

