# Script for generating Sample.csv

# Only 100000 samples per point (for speed)
# up to power=10
# up to dimension 200
# normalized to (dim)^(1/power) since max dx=1
#   so (1^p+1^p+...)^(1/p) = d^(1/p)
./distance -p 10 -r 100000 -d 200 -n > Sample.csv
