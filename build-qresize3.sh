rm qresize3
gcc -g -pg  qresize3.c quantum_interpole3.c `pkg-config gdlib --libs --cflags` -fstack-protector-strong -Wall -o qresize3
