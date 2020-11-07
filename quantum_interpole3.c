/*
Copyright (c) 2020 "tropposite" as on https://github.com/t-site

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


#define IN_PX 4
#define OUT_PX 12
#define HALF_IN_KNL_SZ 2
#define MAX 255
#define BRIGHT 256
#define COLORS 3
#define RA_SIZE 4194304
#define PAD 2
#define OPAD ((PAD)*(OUT_PX)/((IN_PX)))
#define OUT_KNL_SIZE ((OUT_PX)+(OPAD)*2)
#define IN_KNL_SIZE ((IN_PX)+(PAD)*2)
static gdImagePtr input_kernel;
static gdImagePtr cand_kernel;
static gdImagePtr output_kernel;
static int mse[IN_KNL_SIZE][IN_KNL_SIZE][COLORS];
static float fil[3][3] =
	{
		{0,-2.24,0},
		{-2.24,10,-2.24},
		{0,-2.24,0}
	};
static unsigned int imageSX;
static unsigned int imageSY;
static FILE *randomfd;
static int t = 2;
static signed char randarray[RA_SIZE];
static int randpointer = 0;

static long half_psnr_mse( void )
{
	gdImagePtr tmp_kernel;
	int halved_pixels[ IN_KNL_SIZE ][ IN_KNL_SIZE ][COLORS];
	int input_pixels [ IN_KNL_SIZE ][ IN_KNL_SIZE ][COLORS];
	unsigned int x,y,c;
	long mse_all = 0;

	for( y=0 ; y < IN_KNL_SIZE ; y++  )
	{
		for( x=0 ; x < IN_KNL_SIZE ; x++  )
		{
			int color;
			color = gdImageGetTrueColorPixel( input_kernel , x , y );

			input_pixels[y][x][0] = gdTrueColorGetRed(color);
			input_pixels[y][x][1] = gdTrueColorGetGreen(color);
			input_pixels[y][x][2] =  gdTrueColorGetBlue(color);
		}
	}
	tmp_kernel = gdImageScale ( output_kernel, HALF_IN_KNL_SZ , HALF_IN_KNL_SZ);
	cand_kernel = gdImageScale ( tmp_kernel , IN_KNL_SIZE , IN_KNL_SIZE );
	gdImageDestroy( tmp_kernel );
	for( y=0 ; y < IN_KNL_SIZE ; y++  )
	{
		for( x=0 ; x < IN_KNL_SIZE ; x++  )
		{
			int color;
			color = gdImageGetTrueColorPixel( cand_kernel , x , y );

			halved_pixels[y][x][0] = gdTrueColorGetRed(color);
			halved_pixels[y][x][1] = gdTrueColorGetGreen(color);
			halved_pixels[y][x][2] =  gdTrueColorGetBlue(color);
		}
	}

	for( y = 0 ; y < IN_KNL_SIZE ; y++)
	{
		for( x=0 ; x < IN_KNL_SIZE ; x++ )
		{
			for ( c=0 ; c < COLORS ; c++ )
			{
				long a,b;
				a = (long)input_pixels[y][x][c];
				b = (long)halved_pixels[y][x][c];
				mse_all  += (( a - b )*( a - b ));
				mse[y][x][c] = ( a - b ) / 4 ;
				if( mse[y][x][c] < 0 )
					mse[y][x][c] = ( b - a )  ;
				if (mse[y][x][c] < 2 )
					mse[y][x][c] = 2;
			}
		}
	}
	gdImageDestroy( cand_kernel );
	return mse_all;
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
	int output_pixels [OUT_KNL_SIZE][OUT_KNL_SIZE][COLORS] ;
	int counter;
	long mse_all;

	for(counter = 0 ;;)
	{
		mse_all = half_psnr_mse() ;
		if (  mse_all / ( IN_KNL_SIZE * IN_KNL_SIZE * COLORS * MAX * t ) == 0 )
		{
			break;
		}
		for( y=0 ; y < OUT_KNL_SIZE ; y++ )
		{
			for( x=0 ; x < OUT_KNL_SIZE ; x++  )
			{
				int color;
				color = gdImageGetTrueColorPixel( output_kernel , x , y );
				output_pixels[y][x][0] = gdTrueColorGetRed(color);
				output_pixels[y][x][1] = gdTrueColorGetGreen(color);
				output_pixels[y][x][2] =  gdTrueColorGetBlue(color);
			}
		}
		for( y=0 ; y < OUT_KNL_SIZE ; y++)
		{
			int iny = y * IN_KNL_SIZE / OUT_KNL_SIZE;

			for( x=0 ; x < OUT_KNL_SIZE ; x++)
			{
				int inx = x * IN_KNL_SIZE / OUT_KNL_SIZE;
				for ( c=0 ; c<COLORS ; c++)
				{
	
					signed char rand;
					counter++;
					rand = getrand();
					output_pixels[y][x][c] += (int)rand % ( mse[iny][inx][c] ) ;
					if(output_pixels[y][x][c] >= BRIGHT )
						output_pixels[y][x][c] = BRIGHT-1 ;
					else if(output_pixels[y][x][c] < 0 )
						output_pixels[y][x][c] = 0 ;
				}
			}
		}
		/*CPU cycle counter detects stall.*/
		if ( counter >= INT_MAX / (IN_KNL_SIZE * 6 * 7 + OUT_KNL_SIZE * 2 * 7 * COLORS ))
		{
			t++;
			printf ("t=%d\n",t);
			counter = 0;
		}
		for( y=0 ; y < OUT_KNL_SIZE ; y++ )
		{
			for( x=0 ; x < OUT_KNL_SIZE ; x++  )
			{
				int pixel;
				pixel = gdImageColorClosest(output_kernel,output_pixels[y][x][0],output_pixels[y][x][1],output_pixels[y][x][2]);
				gdImageSetPixel(output_kernel,x,y,pixel);
			}
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
	imageSX = input_imageSX * OUT_KNL_SIZE / IN_KNL_SIZE ;
	imageSY = input_imageSY * OUT_KNL_SIZE / IN_KNL_SIZE ;
	gdImageSetInterpolationMethod(input_image,GD_BELL);
	output_image = gdImageScale(input_image,  imageSX , imageSY );
/*	gdImageConvolution(output_image,fil,1,0);*/
	input_kernel = gdImageCreateTrueColor( IN_KNL_SIZE + PAD * 2 , IN_KNL_SIZE + PAD * 2 );
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
	gdImageSetInterpolationMethod(input_image,GD_BELL);
	for( y=0  ; y < input_SY ; y += IN_PX )
	{
		for( x=0 ; x < input_SX ; x += IN_PX )
		{
			int out_x , out_y ;
			out_x = x * OUT_PX / IN_PX ;
			out_y = y * OUT_PX / IN_PX ;
			gdImageCopy(input_kernel , input_image ,0,0, x-PAD  , y-PAD  , IN_KNL_SIZE , IN_KNL_SIZE );
			gdImageCopy(output_kernel , output_image , 0,0, out_x-OPAD , out_y-OPAD  , OUT_KNL_SIZE , OUT_KNL_SIZE );
			quantum_art();
			gdImageCopy(output_image,output_kernel,out_x , out_y  ,OPAD , OPAD , OUT_PX ,OUT_PX );
		}
	}
	fclose(randomfd);
	gdImageDestroy(input_kernel);	
	gdImageDestroy(output_kernel);
	return output_image;
}
