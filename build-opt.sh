rm qresize
gcc -O2 -march=native main.c quantum_interpole.c capsule.c `pkg-config gdlib --libs --cflags` -o qresize
