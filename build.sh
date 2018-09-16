rm qresize
gcc -g main.c quantum_interpole.c `pkg-config gdlib --libs --cflags` -fstack-protector-strong -o qresize
