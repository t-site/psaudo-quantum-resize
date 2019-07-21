/* qresize image divider. not nessesary for main quantum_interpole.c */
/* this mean is image data chunked for image data size < CPU cache   */
#include<gd.h>
#include<stdio.h>
#include "quantum_interpole.h"

#define COLORS 3
#define L1_CACHE_SZ 65536
/* mod2 size to less pixel margin loss */
#define L2_CACHE_SZ 838656
#define MARGIN 4
gdImagePtr chunked_qresize2( gdImagePtr input_img , int threshold )
{
	gdImagePtr output_img , chunk_img , output_chunk_img ;
	int input_SX , input_SY ;
	long input_pixels ;
	int chunk_SY , chunk_SX ;
	int divide , i , xcursor ;
	input_SX = gdImageSX( input_img );
	input_SY = gdImageSY( input_img );

	input_pixels = input_SX * input_SY * COLORS ;

	if( input_pixels < L1_CACHE_SZ )
	{
		return quantum_interpole( input_img , threshold );
	}

	chunk_SY = input_SY;

	chunk_SX = input_SX / ( input_pixels / L1_CACHE_SZ );
	if ( chunk_SX < ( MARGIN * 4 ) )
	{
		fputs("image chunk too tall\n",stderr);
		return quantum_interpole( input_img , threshold );
	}

	divide = input_SX / chunk_SX  ;
	output_img = gdImageCreateTrueColor( input_SX * 2 , input_SY * 2 );

	for ( i = 0 , xcursor = 0 ; i < divide ; i++ , xcursor += chunk_SX )
	{
		int left_margin , right_margin ;
		
		if ( i == 0 )
			left_margin = 0;
		else
			left_margin = MARGIN;
		if ( i < ( divide - 1) )
			right_margin = MARGIN ;
		else
			right_margin = 0;

		chunk_SX = ( input_SX + i ) / divide ;
		chunk_img = gdImageCreateTrueColor( chunk_SX + left_margin + right_margin  , chunk_SY );
		gdImageCopy( chunk_img , input_img , 0 , 0 , xcursor - left_margin , 0 , chunk_SX + left_margin + right_margin , chunk_SY );
		output_chunk_img = quantum_interpole( chunk_img , threshold );
		gdImageCopy( output_img , output_chunk_img , xcursor * 2 , 0 , left_margin * 2 , 0 , chunk_SX * 2 , chunk_SY * 2 );
		gdImageDestroy( chunk_img );
		gdImageDestroy( output_chunk_img );
	}
	
	return output_img;
}

gdImagePtr striped_qresize2( gdImagePtr input_img , int threshold )
{
	gdImagePtr output_img , output_stripe_img , stripe_img ;
	int input_SX , input_SY;
	int stripe_SX , stripe_SY;
	int divide , i , ycursor;
	long input_pixels;
	input_SX = gdImageSX( input_img );
	input_SY = gdImageSY( input_img );

	input_pixels = input_SX * input_SY * COLORS ;

	if( input_pixels < L2_CACHE_SZ )
	{
		return quantum_interpole( input_img , threshold );
	}

	stripe_SX = input_SX;
 	stripe_SY = input_SY  / ( input_pixels / L2_CACHE_SZ ) ;

	if( stripe_SY < ( MARGIN * 4 ) )
	{
		/*TODO: rectangle copy instead stripe for super large image.*/
		fputs("image stripe chunk for CPU Cache size failed.",stderr);
		fputs("image width is too large, operation slow.",stderr);
		return quantum_interpole( input_img , threshold );
	}
	divide = input_SY / stripe_SY  + 1 ;
	output_img = gdImageCreateTrueColor( input_SX * 2 , input_SY * 2 );
	for ( i = 0 , ycursor = 0 ; i < divide ; i++ , ycursor += stripe_SY )
	{
		int up_margin , down_margin ;
		
		if ( i == 0 )
			up_margin = 0;
		else
			up_margin = MARGIN;
		if ( i < ( divide - 1) )
			down_margin = MARGIN ;
		else
			down_margin = 0;

		stripe_SY = ( input_SY + i ) / divide ;
		stripe_img = gdImageCreateTrueColor( stripe_SX , stripe_SY + up_margin + down_margin );
		gdImageCopy( stripe_img , input_img , 0 , 0 , 0 , ycursor - up_margin , input_SX , stripe_SY + up_margin + down_margin );
		output_stripe_img = quantum_interpole( stripe_img , threshold );
		gdImageCopy( output_img , output_stripe_img , 0 , ycursor * 2 , 0 , up_margin * 2 , input_SX * 2 , stripe_SY * 2 );
		gdImageDestroy( output_stripe_img );
		gdImageDestroy( stripe_img );
	}
	return output_img ;
}

