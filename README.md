# quantum-resize
quantum-resize is psaudo-quantum process image resizer.
currently,2x multiplied.
## what is quantum-resize
What is "Quantum computing" for modern/past digital computer?

quantum-resize use random numbers from UNIX /dev/urandom device.
first,modern/past digital interpolation is applied. this process eliminate almost quantum computing resource.
next to "psaudo-quantum" process.
each 6x6 pixels are injected random value.
resize to 3x3, compare to each input pixels.
if difference is small , next random number width is small.
and compute 1/PSNR , if 1/PSNR more than threshold ,continue "psaudo-quantum" process.
scale by cascade with a shell script for much more high quality.

I cannot know all of quantum computing , but I feel quantum-resize process similar to real quantum computing.

quantum-resize requires C compiler , /dev/urandom , and libgd graphics library.

### New qresize1.5 command
New qresize1.5 command available.
shrink output image to 1.5x for suppressed juggy. but cause blurred image.
Recommend for front-end of cascade scale. 

## quantum-resize for many photograph
quantum-resize is match for photograph from the real world.
and good for 2D animation frames.
for pure 2D paint ... use waifu2x first.

