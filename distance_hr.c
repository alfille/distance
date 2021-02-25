#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

void help( void )
{
    printf("distance_hr -- find the average distance between random points in\n") ;
    printf("\ta unit N-cube using Monti Carlo method.\n");
    printf("\n");
    printf("By Paul H Alfille 2021 -- MIT license\n") ;
    printf("This version uses GMP and MPFR high resolution floating point libraries\n") ;
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

    // random state
    gmp_randstate_t rstate;
    gmp_randinit_mt(rstate);

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

    // Assume that .001 is the practical lower limit of the random variable so multiplied by itself Diminsion times is still significant
    // .001 is 1/1000 =~ 2^-10 -- plus a little slack
    int bits = 10*Dimensions + 5 ;
    mpfr_set_default_prec( bits ) ;

    // create work variables here (Because GMP is a bit of a nuisance about this)
    mpfr_t x1 ;
    mpfr_init(x1);

    mpfr_t x2 ;
    mpfr_init(x2);

    mpfr_t dx ;
    mpfr_init(dx);

    mpfr_t cumprod ;
    mpfr_init(cumprod);

    mpfr_t root ;
    mpfr_init(root);

    // Initialize totals to zero
    // zero out the first row (i.e. zero dimensional case) of the sums
    // for GMP have to initialize them all
    mpfr_t totals[Dimensions+1][Powers];
    mpfr_t sums[Dimensions+1][Powers];
    int d,p;
    for (d=0; d <= Dimensions; ++d) {
        for (p=0; p<Powers; ++p) {

            mpfr_init( totals[d][p] );
            mpfr_set_zero( totals[d][p], MPFR_RNDN );
            
            mpfr_init( sums[d][p] );
            mpfr_set_zero( sums[d][p], MPFR_RNDN );
        }
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
            mpfr_urandomb( x1 , rstate );
            mpfr_urandomb( x2 , rstate );
            mpfr_sub( dx, x1, x2, MPFR_RNDN );
            mpfr_abs( dx, dx, MPFR_RNDN ) ;

            // for each power, the sum will be the entry from the row above
            // plus dx raised to that power.
            mpfr_set_d( cumprod, 1.0, MPFR_RNDN ) ;
            for (p=0; p<Powers; ++p) {
                mpfr_mul( cumprod, cumprod, dx, MPFR_RNDN );
                mpfr_add( sums[d][p], sums[d-1][p], cumprod, MPFR_RNDN );
            }
        }

        // Add the pth root of each sum to the totals
        for (d=1; d <= Dimensions; ++d) {
            for (p=0; p<Powers; ++p) {
                // note p is 0-indexed in C, but 1-indexed for calculation
                mpfr_rootn_ui( root, sums[d][p], p+1, MPFR_RNDN ); // root used as a scratch variable
                mpfr_add( totals[d][p], totals[d][p], root, MPFR_RNDN ); 
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

        // Average, normalize and print
        for (p=0;p<Powers;++p) {
            // Average in place
            mpfr_div_ui( totals[d][p], totals[d][p], Randoms, MPFR_RNDN ) ;
            
            // Possibly normalize to longest diagonal
            if (Normalize) {
                mpfr_set_ui( root, d, MPFR_RNDN ) ; // root used as a scratch variable
                // note p is 0-indexed in C, but 1-indexed for calculation
                mpfr_rootn_ui( root, root, p+1, MPFR_RNDN ) ;

                // Normalize in place
                mpfr_div( totals[d][p], totals[d][p], root, MPFR_RNDN ); 
            }

            // Print out distances
            mpfr_printf("%.32Rf, ", totals[d][p]);
        }
        // finish line
        printf("\n");
    }

    // success
    return 0 ;
}

