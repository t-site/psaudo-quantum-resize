rm qresize qresize1.5
gcc -g main.c quantum_interpole.c `pkg-config gdlib --libs --cflags` -fstack-protector-strong -o qresize
gcc -g qresize-1.5.c quantum_interpole.c `pkg-config gdlib --libs --cflags` -fstack-protector-strong -o qresize1.5
