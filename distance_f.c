#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

void help( void )
{
    printf("distance_f -- find the average distance between random points in\n") ;
    printf("\ta unit N-cube using Monti Carlo method.\n");
    printf("\n");
    printf("By Paul H Alfille 2021 -- MIT license\n") ;
    printf("\n");
    printf("Output is CSV file format to make easy manipulation.\n");
    printf("Uses f-metric l^p\n");
    printf("\t1\tD = Sum( x1^p + x2^p + ... )\n");
    printf("\t2\tNote that there is no ^(1/p) for the sum\n");
    printf("\t3\tThe norm is no homogeneous -- i.e.\n");
    printf("\t3\t\tD(c*x) != c*D(x) for vector x\n");
    printf("\n");
    printf("\t1\tArbitrary -- any values >0 including non-integer are allowed\n");
    printf("\n");
    printf("Syntax:\n");
    printf("\tdistance [options]\n");
    printf("Options:\n");
    printf("\t-d 100\tmax dimensions\n");
    printf("\t-p 20\tmetric power -- single\n");
    printf("\t-p \"1-20\"\tmetric power -- range 1 to 20\n");
    printf("\t-p \"1-20_3\"\tmetric power -- range with increment\n");
    printf("\t-p \".5,.75,2.5\"\tmetric power -- floats allowed\n");
    printf("\t-r 1000000\trandom points each measure\n");
    printf("\t-n\tnormalize (to longest diagonal)\n");
    printf("\t-h\tthis help\n");
    exit(0) ;
}

struct rangelist {
    int size ;
    int alloc ;
    double * val ;
} ;

struct rangelist * rangelist_init( void ) {
    struct rangelist * rl = malloc( sizeof ( struct rangelist ) ) ;
    rl->alloc = 100 ;
    rl->size = 0 ;
    rl->val = malloc( rl->alloc * sizeof( double ) ) ;
    return rl ;
}

void rangelist_inc( struct rangelist * rl ) {
    rl->alloc += 100 ;
    rl->val = realloc( rl->val, rl->alloc * sizeof(double) ) ;
}

void rangelist_add( double p, struct rangelist * rl ) {
    if ( rl->size == rl->alloc ) {
        rangelist_inc( rl ) ;
    }
    rl->val[rl->size++] = p ;
}

void rangelist_free( struct rangelist * rl ) {
    free( rl ) ;
}

void rangelist_print( struct rangelist * rl ) {
    printf("Ranglist size=%d alloc=%d values= ",rl->size,rl->alloc);
    for (int i = 0 ; i < rl->size ; ++i ) {
        printf("%f ",rl->val[i]);
    }
    printf("\n") ;
}

struct rangelist * range( char * p_string )
{
    // Interpret the string as a list of power values
    // individual 1,2, 5
    // ranges 1 _ 6

    char * rcopy = strdup( p_string ) ;
    //printf("Full %s\n",rcopy) ;

    struct rangelist * rl = rangelist_init() ;
    
    char * rcomma ;
    char * rthis = strtok_r( rcopy, ",", &rcomma ) ;

    while ( rthis != NULL ) {
        //printf("Comma %s\n",rthis);

        char * rbar ;
        char * rthat = strtok_r( rthis, "_", &rbar ) ;
        double p_val[3] ; // extent of range
        int p_num = 0 ; // range elements

        while ( rthat != NULL ) {
            //printf("\t Bar %s\n",rthat) ;
            if ( p_num == 3 ) {
                // only 3 entries in range allowed
                break ;
            }
            double v = atof(rthat) ;
            if (v <= 0.) {
                v = 1. ;
            }
            p_val[p_num++] = v ;

            rthat = strtok_r( NULL, "_", &rbar ) ;
        }

        switch (p_num) {
        case 0:
            // no entries
            break ;
        case 1:
            // single entry
            p_val[1] = p_val[0] ;
            // fall through ...
        case 2:
            // straight range
            p_val[2] = 1 ;
            // fall through ...
        case 3:
            for ( double v = p_val[0]; v <= p_val[1] ; v += p_val[2] ) {
                rangelist_add( v, rl ) ;
            }
        }

        rthis = strtok_r( NULL, "," , &rcomma ) ;
    }

    free( rcopy ) ;

    return rl ;
} 
     
int main( int argc, char **argv )
{
    int Dimensions = 100 ;
    long Randoms = 1000000 ;
    int Normalize = 0;

    double scale = 1.0 / RAND_MAX ;

    struct rangelist * powerlist = NULL ;

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
            powerlist = range( optarg ) ;
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

    if ( powerlist==NULL ) {
        // default power range
        powerlist = range("1_3");
    }

    // Initialize random seed
    srand(time(0));

    // Initialize totals to zero
    double totals[Dimensions+1][powerlist->size];
    for (int d=0; d <= Dimensions; ++d) {
        for (int ip=0; ip<powerlist->size; ++ip) {
            totals[d][ip] = 0.;
        }
    }

    // zero out the first row (i.e. zero dimensional case) of the sums
    double sums[Dimensions+1][powerlist->size];
    for (int ip=0; ip<powerlist->size; ++ip) {
        sums[0][ip] = 0.;
    }

    // Generate and add up sums of coordinate differences at various dimensions
    // from two randomly generated points in the hypercube.
    for (long r = 0; r < Randoms; ++r) {
        // fill in the sum of powers for a single sample at various dimensions
        // and powers.
        for (int d=1; d <= Dimensions; ++d) {
            // For each dimension, get 2 coordinates in this dimension.
            // we only care about dx, the delta in the coordinate
            // use abs value for odd powers calculation
            double dx = fabs((rand() - rand()) * scale);

            // for each power, the sum will be the entry from the row above
            // plus dx raised to that power.
            for (int ip=0; ip<powerlist->size; ++ip) {
                sums[d][ip] = sums[d-1][ip] + pow(dx,powerlist->val[ip]) ;
            }
        }

        // Add the pth root of each sum to the totals
        for (int d=1; d <= Dimensions; ++d) {
            for (int ip=0; ip<powerlist->size; ++ip) {
                totals[d][ip] += sums[d][ip] ;
            }
        }
    }

    // Title line
    printf("DIM\\Power, ");
    for (int ip=0;ip<powerlist->size;++ip) {
        printf("%.2f, ",powerlist->val[ip]);
    }
    printf("\n");

    // Loop though dimensions
    for (int d=1; d <= Dimensions; ++d) {
        // Start line with dimension
        printf("%d, ",d);

        // Print out the distances
        for (int ip=0;ip<powerlist->size;++ip) {
            if (Normalize) {
                printf("%g, ", totals[d][ip]/Randoms/d);
            } else {
                printf("%g, ", totals[d][ip]/Randoms);
            }
        }
        // finish line
        printf("\n");
    }

    // success
    rangelist_free( powerlist ) ;
    return 0 ;
}
