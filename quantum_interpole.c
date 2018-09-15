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
#include"capsule.h"

#define KNL 6
#define HALF_KNL 3
#define MAX 256
#define COLORS 3
#define ITER 5

static int input_pixels[KNL][KNL];
static int output_pixels[KNL][KNL];
static int mse[HALF_KNL][HALF_KNL];
static unsigned int imageSX;
static unsigned int imageSY;
static FILE *randomfd;

static long half_psnr_mse(void)
{
	int halved_pixels[HALF_KNL][HALF_KNL];
	unsigned int x,y,ox,oy;
	long all=0;
	for( y=0 , oy=0 ; y < HALF_KNL ; y++ , oy+=2 )
	{
		for( x=0 , ox=0 ; x < HALF_KNL ; x++ , ox+=2 )
		{
			int tmp =0;
			tmp += output_pixels[oy][ox] ;
			tmp += output_pixels[oy][ox+1];
			tmp += output_pixels[oy+1][ox];
			tmp += output_pixels[oy+1][ox+1];
			halved_pixels[y][x] = tmp / 4;
		}
	}

	for( y = 0 ; y < HALF_KNL ; y++)
	{
		for( x=0 ; x < HALF_KNL ; x++ )
		{
			long a,b,tmp;
			a = (long)input_pixels[y][x];
			b = (long)halved_pixels[y][x];
			tmp = (( a - b )*( a - b ));
			mse[y][x]=tmp;
			all+=tmp;
		}
	}
	return all;
}

static int quantum_art( int threshold )
{
	unsigned int x,y;
	int factor[KNL][KNL];
	long all;
	for(;;)
	{
		for( y=0 ; y < KNL ; y++)
		{
			for( x=0 ; x < KNL ; x++)
			{
				signed char tmp;
				tmp = 0xFF & fgetc(randomfd);
				output_pixels[y][x] += (int)tmp % ( all / (HALF_KNL * HALF_KNL *(MAX-1) ) /*(mse[y/2][x/2] / (MAX-1) +1*/ +1 )  ;
				if(output_pixels[y][x] >= MAX )
					output_pixels[y][x] = MAX-1 ;
				else if(output_pixels[y][x] < 0 )
					output_pixels[y][x] = 0 ;
			}
		}
		all = half_psnr_mse() ;
		if ( all /( HALF_KNL * HALF_KNL * (MAX-1) ) < threshold )
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
			int c;
			for( c=0 ; c < COLORS ; c++)
			{
				int read_y , read_x , i , j;
				switch(c)
				{
					case 0:
						set_rgb('R');
						break;
					case 1:
						set_rgb('G');
						break;
					case 2:
						set_rgb('B');
						break;
				}
				for( read_y= y-1 , j=0 ; j<HALF_KNL ; read_y++ , j++ )
				{
					for( read_x= x-1 , i=0 ; i<HALF_KNL ; read_x++ ,i++ )
					{
						input_pixels[j][i] = getpixel(input_image , read_x , read_y);
					}
				}
				for( read_y= y*2-2 , j=0 ; j<KNL ; read_y++ , j++ )
				{
					for( read_x= x*2-2 , i=0 ; i<KNL ; read_x++ ,i++ )
					{
						output_pixels[j][i] = getpixel(output_image , read_x , read_y);
					}
				}
				
				quantum_art(threshold);
				for( read_y=y*2 , j=2 ; j<=3 ; read_y++ , j++ )
				{
					for( read_x=x*2 , i=2 ; i<=3 ; read_x++ ,i++ )
					{
						setpixel(output_image,output_pixels[j][i],read_x,read_y);
					}
				}
			}
		}
		printf("%u lines\n",y);
	}
	fclose(randomfd);
	return output_image;
}
