rm qresize qresize1.4
gcc -g main.c divide.c quantum_interpole.c `pkg-config gdlib --libs --cflags` -fstack-protector-strong -Wall -o qresize
gcc -g qresize-1.4.c quantum_interpole-1.4.c `pkg-config gdlib --libs --cflags` -fstack-protector-strong -Wall -o qresize1.4
