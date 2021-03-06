#!/usr/bin/python3

# Program to stitch together CSV files
# removing first row of subsequent files
# and optionally adding text to header row column names 

try:
    import argparse # for parsing the command line
except:
    print("Please install the argparse module")
    print("\tit should be part of the standard python3 distribution")
    raise

try:
    import sys # for actual arguments
except:
    print("Please install the sys module")
    print("\tit should be part of the standard python3 distribution")
    raise

try:
    import os.path # for basename
except:
    print("Please install the os.path module")
    print("\tit should be part of the standard python3 distribution")
    raise

try:
    import csv # for csv read and write
except:
    print("Please install the csv module")
    print("\tit should be part of the standard python3 distribution")
    raise

def list2csv( lst ):
    return ','.join(lst)
    

class CSVfile:
    firstcol = True
    
    def __init__( self, fname, farg, dslice="::" ):
        self.reader = csv.reader( farg, delimiter=',',quotechar='"')

        # only first class instance keeps first column
        self.firstcol = type(self).firstcol
        type(self).firstcol = False # block all others

        # get first line (to figure length, actually) -- as a list
        self.firstline = next(self.reader)

        # empty last element (trailing comma) removed here and all lines
        self.length = len(self.firstline)
        if self.firstline[-1].strip() == '' :
            self.length -= 1

        # do slice magic
        sl = (dslice+"::").split(':')[:3]
        for i in range(3):
            try:
                sl[i] = int(sl[i])
            except:
                sl[i] = None

        self.slice = slice(*tuple(sl))
        
        # If slice does not include last element, set a flag to add it on.
        self.lastcol = ( list(range(self.length))[1:][self.slice][-1] < self.length-1 )

        self.name = fname

    def rslice( self, vallist ):
        # apply slice and first and last

        # possibly include first column
        v = vallist[0:1] if self.firstcol else []

        # Add sliced data
        v += vallist[1:self.length][self.slice]

        # possibly add last real item
        if self.lastcol:
            v.append(vallist[self.length-1])
        return v

    @property
    def next( self ):
        return list2csv(self.rslice(next(self.reader)))

    @property
    def first( self ):
        return list2csv(self.rslice(['"{}{}"'.format(self.name,c.replace('"','')) for c in self.firstline]))
        
def CommandLine():
    """Setup argparser object to process the command line"""
    cl = argparse.ArgumentParser(description="Stitch together multiple CSV files using one the first column frlm tbhe first one.\n 2021 by Paul H Alfille\nsee http://github.com/alfille/distance")
    cl.add_argument("CSV",help="CSV file[2] -- at least one is needed. All will be stitched together width-wise",type=argparse.FileType('r'),nargs='+')
    cl.add_argument("-s","--slice",help='python format slice of data row entries (not including first column) -- e.g. "::2" for every other point. Last value always included',nargs='?',default="::")
    return cl.parse_args()

if __name__ == '__main__': # command line
    args = CommandLine() # Get args from command line

    names = [os.path.splitext(os.path.basename(a))[0] for a in sys.argv][1:]

    CSVclasses = [CSVfile(*z,args.slice) for z in zip( names, args.CSV )]

    print( list2csv([c.first for c in CSVclasses]) )
    while True:
        try:
            print( list2csv([c.next for c in CSVclasses]) )
        except:
            break


    
    
