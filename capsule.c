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
#include<gd.h>

static int rgb;

void set_rgb(int input)
{
	rgb = input;
}

int getpixel(gdImagePtr image , int x , int y )
{
	int tmp = gdImageGetTrueColorPixel( image , x , y );
	switch ( rgb )
	{
		case 'R':
			return gdTrueColorGetRed(tmp);
		case 'G':
			return gdTrueColorGetGreen(tmp);
		case 'B':
			return gdTrueColorGetBlue(tmp);
	}
	return 0;
}

void setpixel(gdImagePtr image , int value , int x , int y )
{
	int r,g,b;
	int output;
	int before;
	before = gdImageGetTrueColorPixel( image , x , y );
	switch ( rgb )
	{
		case 'R':
			b = gdTrueColorGetBlue(before);
			g = gdTrueColorGetGreen(before);
			r = value;
		break;
		case 'G':
			b = gdTrueColorGetBlue(before);			
			r = gdTrueColorGetRed(before);
			g = value;
		break;
		case 'B':
			g = gdTrueColorGetGreen(before);
			r = gdTrueColorGetRed(before);
			b = value;
	}
	output = gdImageColorClosest( image , r , g , b );
	gdImageSetPixel(image,x,y,output);

}
