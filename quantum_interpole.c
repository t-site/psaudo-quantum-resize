/*
Copyright (c) 2018 "tropposite" as on https://github.com/t-site

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

static int input_pixels[COLORS][KNL][KNL];
static int output_pixels[COLORS][KNL][KNL];
static int mse[COLORS][HALF_KNL][HALF_KNL];
static int t=1;
static int randpointer=0;
static signed char randarray[RA_SIZE];
static unsigned int imageSX;
static unsigned int imageSY;
static FILE *randomfd;

static long half_psnr_mse(void)
{
	int halved_pixels[COLORS][HALF_KNL][HALF_KNL];
	unsigned int x,y,ox,oy,c;
	long all=0;
	for( y=0 , oy=0 ; y < HALF_KNL ; y++ , oy+=2 )
	{
		for( x=0 , ox=0 ; x < HALF_KNL ; x++ , ox+=2 )
		{
			for ( c=0 ; c<COLORS ; c++)
			{
				int tmp =0;
				tmp += output_pixels[c][oy][ox] ;
				tmp += output_pixels[c][oy][ox+1];
				tmp += output_pixels[c][oy+1][ox];
				tmp += output_pixels[c][oy+1][ox+1];
				halved_pixels[c][y][x] = tmp / 4;
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
				a = (long)input_pixels[c][y][x];
				b = (long)halved_pixels[c][y][x];
				all  += (( a - b )*( a - b ));
				mse[c][y][x]= a - b ;
				if( mse[c][y][x] < 0 )
					mse[c][y][x] = b - a ;
				if (mse[c][y][x] < 2 )
					mse[c][y][x] = 2;
			}
		}
	}
	return all;
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
	long all;
	half_psnr_mse() ;
	for(int counter = 0;;)
	{
		for( y=0 ; y < KNL ; y++)
		{
			for( x=0 ; x < KNL ; x++)
			{
				for ( c=0 ; c<COLORS ; c++)
				{
					signed char rand;
					rand = getrand();
					output_pixels[c][y][x] += (int)rand % ( mse[c][y/2][x/2] /2 ) ;
					counter++;
					if(output_pixels[c][y][x] >= MAX )
						output_pixels[c][y][x] = MAX-1 ;
					else if(output_pixels[c][y][x] < 0 )
						output_pixels[c][y][x] = 0 ;
					
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
		if ( all /( HALF_KNL * HALF_KNL * COLORS * COLORS * (MAX-1) ) < t )
			return 0;
		
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
			for( read_y= y-((HALF_KNL -1)/2) , j=0 ; j<HALF_KNL ; read_y++ , j++ )
			{
				for( read_x= x-((HALF_KNL-1)/2) , i=0 ; i<HALF_KNL ; read_x++ ,i++ )
				{
					int tmp;
					tmp = gdImageGetTrueColorPixel( input_image , read_x , read_y );
					input_pixels[0][j][i] = gdTrueColorGetRed(tmp);
					input_pixels[1][j][i] = gdTrueColorGetGreen(tmp);
					input_pixels[2][j][i] = gdTrueColorGetBlue(tmp);
				}
			}
			for( read_y= y*2 -HALF_KNL+1 , j = 0 ; j < KNL ; read_y++ , j++ )
			{
				for( read_x= x*2 -HALF_KNL+1 , i = 0 ; i < KNL ; read_x++ ,i++ )
				{
					int tmp;
					tmp = gdImageGetTrueColorPixel( output_image , read_x , read_y );
					output_pixels[0][j][i] = gdTrueColorGetRed(tmp);
					output_pixels[1][j][i] = gdTrueColorGetGreen(tmp);
					output_pixels[2][j][i] = gdTrueColorGetBlue(tmp);
				}
			}
			quantum_art();
			for( read_y = y*2 , j =  HALF_KNL -1 ; j <= HALF_KNL ; read_y++ , j++ )
			{
				for( read_x = x*2 , i = HALF_KNL -1 ; i <= HALF_KNL ; read_x++ ,i++ )
				{
					int pixel;
					pixel = gdImageColorClosest(output_image,output_pixels[0][j][i],output_pixels[1][j][i],output_pixels[2][j][i]);
					gdImageSetPixel(output_image,read_x,read_y,pixel);
				}
			}
		}
		printf("%u lines\n",y);
	}
	fclose(randomfd);
	return output_image;
}
