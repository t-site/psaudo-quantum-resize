rm qresize qresize1.4
gcc -O2 -march=native main.c divide.c quantum_interpole.c `pkg-config gdlib --libs --cflags` -o qresize
gcc -O2 -march=native qresize-1.4.c quantum_interpole-1.4.c `pkg-config gdlib --libs --cflags` -o qresize1.4
