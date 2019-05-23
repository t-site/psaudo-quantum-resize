rm qresize qresize1.5
gcc -O2 -march=native main.c quantum_interpole.c `pkg-config gdlib --libs --cflags` -o qresize
gcc -O2 -march=native qresize-1.5.c quantum_interpole.c `pkg-config gdlib --libs --cflags` -o qresize1.5
