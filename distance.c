#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define TRIES 1000000
#define MAXDIM 100

struct try {
    double l1 ;
    double l2 ;
    double l3 ;
};

int main() 
{
    srand(time(0));
    int d ;
    int t ;
    double scale = 1.0 / RAND_MAX ;
    
    for ( d=1 ; d <= MAXDIM ; ++d ) {
        struct try sum = {0.,0.,0.};
        for (t = 0 ; t < TRIES ; ++t ) {
            int dd ;
            struct try segment = {0.,0.,0.} ;
            for (dd = 0 ; dd < d ; ++dd) {
                double dx = fabs((rand() - rand()) * scale ) ;
                segment.l1 += dx ;
                segment.l2 += dx*dx ;
                segment.l3 += dx*dx*dx ;
            }
            sum.l1 += segment.l1 ;
            sum.l2 += sqrt(segment.l2);
            sum.l3 += pow(segment.l3,.33333333333);
        }
        printf( "%d, %g, %g, %g\n",d,sum.l1/TRIES,sum.l2/TRIES,sum.l3/TRIES );
    }
    
    return 0 ;
}

            
            
    
