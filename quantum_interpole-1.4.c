/*
Copyright (c) 2019 "tropposite" as on https://github.com/t-site

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include<stdio.h>
#include<gd.h>
#include<string.h>
#include<limits.h>

#define OUT_KNL_SIZE 7
#define IN_KNL_SIZE 5
#define HALF_IN_KNL_SZ 2
#define MAX 255
#define BRIGHT 256
#define COLORS 3
#define RA_SIZE 4194304

static gdImagePtr input_kernel;
static gdImagePtr cand_kernel;
static gdImagePtr output_kernel;
static int mse[COLORS][IN_KNL_SIZE][IN_KNL_SIZE];
static unsigned int imageSX;
static unsigned int imageSY;
static FILE *randomfd;
static long mse_all[COLORS];
static int t = 2;
static signed char randarray[RA_SIZE];
static int randpointer = 0;

static void half_psnr_mse( void )
{
	int halved_pixels[COLORS][ IN_KNL_SIZE ][ IN_KNL_SIZE ];
	int input_pixels[COLORS][ IN_KNL_SIZE ][ IN_KNL_SIZE ];
	unsigned int x,y,c;
	mse_all[0]=0;
	mse_all[1]=0;
	mse_all[2]=0;

	for( y=0 ; y < IN_KNL_SIZE ; y++  )
	{
		for( x=0 ; x < IN_KNL_SIZE ; x++  )
		{
			int color;
			color = gdImageGetTrueColorPixel( input_kernel , x , y );

			input_pixels[0][y][x] = gdTrueColorGetRed(color);
			input_pixels[1][y][x] = gdTrueColorGetGreen(color);
			input_pixels[2][y][x] =  gdTrueColorGetBlue(color);
		}
	}
	cand_kernel = gdImageScale ( output_kernel , IN_KNL_SIZE , IN_KNL_SIZE );
	for( y=0 ; y < IN_KNL_SIZE ; y++  )
	{
		for( x=0 ; x < IN_KNL_SIZE ; x++  )
		{
			int color;
			color = gdImageGetTrueColorPixel( cand_kernel , x , y );

			halved_pixels[0][y][x] = gdTrueColorGetRed(color);
			halved_pixels[1][y][x] = gdTrueColorGetGreen(color);
			halved_pixels[2][y][x] =  gdTrueColorGetBlue(color);
		}
	}

	for( y = 0 ; y < IN_KNL_SIZE ; y++)
	{
		for( x=0 ; x < IN_KNL_SIZE ; x++ )
		{
			for ( c=0 ; c < COLORS ; c++ )
			{
				long a,b;
				a = (long)input_pixels[c][y][x];
				b = (long)halved_pixels[c][y][x];
				mse_all[c]  += (( a - b )*( a - b ));
				mse[c][y][x]= ( a - b ) / 4;
				if( mse[c][y][x] < 0 )
					mse[c][y][x] = ( b - a ) / 4 ;
				if (mse[c][y][x] < 2 )
					mse[c][y][x] = 2;
			}
		}
	}
	gdImageDestroy( cand_kernel );
}
static signed char getrand(void)
{
	signed char rand;
	if( randpointer >= RA_SIZE )
	{
		randpointer = 0;
	}
	if(randpointer == 0)
	{
		fread(randarray,sizeof(signed char),RA_SIZE,randomfd);
	}
	rand = randarray[randpointer];
	randpointer++;
	return rand;

}

static int quantum_art( void )
{
	unsigned int x,y,c;
	half_psnr_mse() ;
	int output_pixels [COLORS][OUT_KNL_SIZE][OUT_KNL_SIZE] ;

	for( y=0 ; y < OUT_KNL_SIZE ; y++ )
	{
		for( x=0 ; x < OUT_KNL_SIZE ; x++  )
		{
			int color;
			color = gdImageGetTrueColorPixel( output_kernel , x , y );
			output_pixels[0][y][x] = gdTrueColorGetRed(color);
			output_pixels[1][y][x] = gdTrueColorGetGreen(color);
			output_pixels[2][y][x] =  gdTrueColorGetBlue(color);
		}
	}
	for(int counter = 0 ;;)
	{
		for( y=0 ; y < OUT_KNL_SIZE ; y++)
		{
			for( x=0 ; x < OUT_KNL_SIZE ; x++)
			{
				for ( c=0 ; c<COLORS ; c++)
				{
	
					signed char rand;
					counter++;
					rand = getrand();
					output_pixels[c][y][x] += (int)rand % ( mse[c][y*5/7][x*5/7] /2 ) ;
					if(output_pixels[c][y][x] >= BRIGHT )
						output_pixels[c][y][x] = BRIGHT-1 ;
					else if(output_pixels[c][y][x] < 0 )
						output_pixels[c][y][x] = 0 ;
				}
			}
		}
		/*CPU cycle counter detects stall.*/
		if ( counter >= INT_MAX / (IN_KNL_SIZE * 6 * 7 + OUT_KNL_SIZE * 2 * 7))
		{
			t++;
			printf ("t=%d\n",t);
			counter = 0;
		}
		half_psnr_mse() ;
		if (  mse_all[0] / ( IN_KNL_SIZE * IN_KNL_SIZE * OUT_KNL_SIZE * OUT_KNL_SIZE * (MAX /3 ) ) < t )
		{
			if (  mse_all[1] / ( IN_KNL_SIZE * IN_KNL_SIZE * OUT_KNL_SIZE * OUT_KNL_SIZE * (MAX /3) ) < t )
			{
				if (  mse_all[2] / ( IN_KNL_SIZE * IN_KNL_SIZE * OUT_KNL_SIZE * OUT_KNL_SIZE * (MAX /3 ) ) < t )
				{
					break;
				}
			}
		}
	}
	for( y=0 ; y < OUT_KNL_SIZE ; y++ )
	{
		for( x=0 ; x < OUT_KNL_SIZE ; x++  )
		{
			int pixel;
			pixel = gdImageColorClosest(output_kernel,output_pixels[0][y][x],output_pixels[1][y][x],output_pixels[2][y][x]);
			gdImageSetPixel(output_kernel,x,y,pixel);
		}
	}
	return 0;
}

static gdImagePtr twice(gdImagePtr input_image)
{
	gdImagePtr output_image  ;
	int input_imageSX , input_imageSY ;
	input_imageSX = gdImageSX(input_image);
	input_imageSY = gdImageSY(input_image);
	imageSX = input_imageSX * 7 / 5;
	imageSY = input_imageSY * 7 / 5;
	gdImageSetInterpolationMethod(input_image,GD_BELL);
	output_image = gdImageScale(input_image,  imageSX , imageSY );

	input_kernel = gdImageCreateTrueColor( IN_KNL_SIZE , IN_KNL_SIZE );
	output_kernel = gdImageCreateTrueColor( OUT_KNL_SIZE , OUT_KNL_SIZE );
	return output_image;
}

gdImagePtr quantum_interpole(gdImagePtr input_image , int threshold)
{
	gdImagePtr output_image;
	unsigned int x,y;
	unsigned int input_SX,input_SY;
	t = threshold;
	input_SX = gdImageSX(input_image);
	input_SY = gdImageSY(input_image);
	randomfd = fopen("/dev/urandom","r");
	if(randomfd == NULL)
		return NULL;
	output_image = twice(input_image);
	for( y=0 ; y < input_SY ; y++)
	{
		for( x=0; x < input_SX ; x++)
		{
			int out_x , out_y ;
			out_x = x * 7 / 5;
			out_y = y * 7 / 5;
			gdImageCopy(input_kernel , input_image , 0,0, x - HALF_IN_KNL_SZ , y - HALF_IN_KNL_SZ ,IN_KNL_SIZE , IN_KNL_SIZE );
			gdImageCopy(output_kernel , output_image , 0,0, out_x - 3 , out_y - 3 , OUT_KNL_SIZE , OUT_KNL_SIZE );
			quantum_art();
			gdImageCopy(output_image,output_kernel,out_x -1 , out_y -1 , 2,2,3,3);
		}
	}
	fclose(randomfd);
	gdImageDestroy(input_kernel);	
	gdImageDestroy(output_kernel);
	return output_image;
}
