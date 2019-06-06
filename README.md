# quantum-resize

quantum-resize is psaudo-quantum process image resizer.

currently,2x multiplied.
## what is quantum-resize
What is "Quantum computing" for modern/past digital computer?

quantum-resize use random numbers from UNIX /dev/urandom device.

first,modern/past digital interpolation is applied. this process eliminate almost quantum computing resource.

next to "psaudo-quantum" process.

each 12x12 pixels are injected random value.

resize to 6x6, compare to each input pixels.

if difference is small , next random number width is small.

and compute 1/PSNR , if 1/PSNR more than threshold ,continue "psaudo-quantum" process.

scale by cascade with a shell script for much more high quality.

it unhide detail.

I cannot know all of quantum computing , but I feel quantum-resize process similar to real quantum annealing .

psaudo-quantum-resize requires C compiler , int >= 32bit  , /dev/urandom , 8bit per color channel , and libgd graphics library.
## qresize command howto

```sh
qresize -i input-file -o output-file -t threshold
```

qresize display scanned line count to stdout.

now auto stall detect implimented.

if qresize stall,it threshold increment automatically.

large threshold value specity for speedup.

### New qresize1.4 command
New qresize1.4 command available.

I feel qresize multiply image data for 2x , sqrt(2) resolution.

2x resolution output cause blur.

1.4 aka 7/5 near equal sqrt(2)

qresize1.4 is an psaudo-quantum 1.4x interpolation. not shrink such as past qresize1.5 .

more condense output. and fast to do it.

also auto stall detect implimented.

## quantum-resize for many photograph

quantum-resize is match for photograph from the real world.

and good for 2D animation frames.

for pure 2D paint ... use waifu2x first.
