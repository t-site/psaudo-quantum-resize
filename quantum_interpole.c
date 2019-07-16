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
#include<limits.h>

#define KNL 12
#define HALF_KNL 6
#define MAX 256
#define COLORS 3
#define ITER 5
#define RA_SIZE 4194304

static int input_pixels[HALF_KNL][HALF_KNL][COLORS];
static int output_pixels[KNL][KNL][COLORS];
static int mse[HALF_KNL][HALF_KNL][COLORS];
static int t=1;
static int randpointer=0;
static signed char randarray[RA_SIZE];
static unsigned int imageSX;
static unsigned int imageSY;
static FILE *randomfd;

static inline long half_psnr_mse(void)
{
	int halved_pixels[HALF_KNL][HALF_KNL][COLORS];
	unsigned int x,y,ox,oy,c;
	long all=0;
	for( y=0 , oy=0 ; y < HALF_KNL ; y++ , oy+=2 )
	{
		for( x=0 , ox=0 ; x < HALF_KNL ; x++ , ox+=2 )
		{
			for ( c=0 ; c<COLORS ; c++)
			{
				int tmp =0;
				tmp += output_pixels[oy][ox][c] ;
				tmp += output_pixels[oy][ox+1][c];
				tmp += output_pixels[oy+1][ox][c];
				tmp += output_pixels[oy+1][ox+1][c];
				halved_pixels[y][x][c] = tmp / 4;
			}
		}
	}

	for( y = 0 ; y < HALF_KNL ; y++)
	{
		for( x=0 ; x < HALF_KNL ; x++ )
		{
			for ( c=0 ; c < COLORS ; c++ )
			{
				long a,b;
				a = (long)input_pixels[y][x][c];
				b = (long)halved_pixels[y][x][c];
				all  += (( a - b )*( a - b ));
				mse[y][x][c]= ( a - b ) /2 ;
				if( mse[y][x][c] < 0 )
					mse[y][x][c] = b - a ;
				if (mse[y][x][c] < 2 )
					mse[y][x][c] = 2;
			}
		}
	}
	return all;
}

static inline signed char getrand(void)
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
	long all;
	long counter;
	all = half_psnr_mse() ;
	for(counter = 0;;)
	{
		if ( all /( HALF_KNL * HALF_KNL * COLORS * MAX * t  ) == 0 )
			return 0;
		for( y=0 ; y < KNL ; y++)
		{
			for( x=0 ; x < KNL ; x++)
			{
				for ( c=0 ; c<COLORS ; c++)
				{
					signed char rand;
					rand = getrand();
					output_pixels[y][x][c] += (int)rand % ( mse[y/2][x/2][c]  ) ;
					counter++;
					if(output_pixels[y][x][c] >= MAX )
						output_pixels[y][x][c] = MAX-1 ;
					else if(output_pixels[y][x][c] < 0 )
						output_pixels[y][x][c] = 0 ;
					
				}
			}
		}
		/*CPU cycle counter detects stall.*/
		if ( counter >= INT_MAX / (HALF_KNL * 4 * 7 + KNL * 2 * 7 * COLORS))
		{
			t++;
			printf ("t=%d\n",t);
			counter = 0;
		}
		all = half_psnr_mse() ;
		
	}
}

static gdImagePtr twice(gdImagePtr input_image)
{
	gdImagePtr output_image  ;
	int input_imageSX , input_imageSY ;
	input_imageSX = gdImageSX(input_image);
	input_imageSY = gdImageSY(input_image);
	imageSX = input_imageSX * 2;
	imageSY = input_imageSY * 2;
	gdImageSetInterpolationMethod(input_image,GD_BELL);
	output_image = gdImageScale(input_image,  imageSX , imageSY );
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
			int read_x,read_y,i,j;
			for( read_y= y-(HALF_KNL/2) , j=0 ; j<HALF_KNL ; read_y++ , j++ )
			{
				for( read_x= x-(HALF_KNL/2) , i=0 ; i<HALF_KNL ; read_x++ ,i++ )
				{
					int tmp;
					tmp = gdImageGetTrueColorPixel( input_image , read_x , read_y );
					input_pixels[j][i][0] = gdTrueColorGetRed(tmp);
					input_pixels[j][i][1] = gdTrueColorGetGreen(tmp);
					input_pixels[j][i][2] = gdTrueColorGetBlue(tmp);
				}
			}
			for( read_y= y*2 -HALF_KNL , j = 0 ; j < KNL ; read_y++ , j++ )
			{
				for( read_x= x*2 -HALF_KNL , i = 0 ; i < KNL ; read_x++ ,i++ )
				{
					int tmp;
					tmp = gdImageGetTrueColorPixel( output_image , read_x , read_y );
					output_pixels[j][i][0] = gdTrueColorGetRed(tmp);
					output_pixels[j][i][1] = gdTrueColorGetGreen(tmp);
					output_pixels[j][i][2] = gdTrueColorGetBlue(tmp);
				}
			}
			quantum_art();
			for( read_y = y*2 -1 , j =  HALF_KNL -1 ; j <= HALF_KNL ; read_y++ , j++ )
			{
				for( read_x = x*2 -1 , i = HALF_KNL -1 ; i <= HALF_KNL ; read_x++ ,i++ )
				{
					int pixel;
					pixel = gdImageColorClosest(output_image,output_pixels[j][i][0],output_pixels[j][i][1],output_pixels[j][i][2]);
					gdImageSetPixel(output_image,read_x,read_y,pixel);
				}
			}
		}
	}
	fclose(randomfd);
	return output_image;
}
