SOURCES='parser xompiler'

CXX=clang++

for s in $SOURCES; do
  $CXX -std=c++11 -O1 -g -I../../../src/extra   -I. -I../../../src/include -I../../../src/include  -I/usr/local/include -I../../../src/nmath -I.. -DHAVE_CONFIG_H -fpic -c ${s}.cc -o ${s}.o
done


$CXX -std=c++11 -O1 -g -I../../../src/extra   -I. -I../../../src/include -I../../../src/include  -I/usr/local/include -I../../../src/nmath -I.. -DHAVE_CONFIG_H -fpic test_xompiler.cc parser.o -o test_xompiler


ar r xompiler.a *.o
