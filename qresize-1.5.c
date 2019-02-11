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

#include<gd.h>
#include<unistd.h>
#include"quantum_interpole.h"
int main(int argc,char **argv)
{
	gdImagePtr input_image , qint_image , output_image;
	char *input_file_name , *output_file_name , *err;
	int parameters = 0;
	int threshold = 8;
	int opt;
	int outputSX , outputSY;
	while ((opt = getopt(argc, argv, "i:o:t:")) != -1) 
	{
		switch(opt)
		{
			case 'i':
				input_file_name=optarg;
				parameters += 1;
			break;
			case 'o':
				output_file_name=optarg;
				parameters += 2;
			break;
			case 't':
				threshold = atoi(optarg);
				if( threshold < 0 )
				{
					fputs("Error: threshold number less than zero\n",stderr);
					return 4;
				}
			break;
			default:
				fputs("psaudo quantum computing 1.5x image resizer\n",stderr);
				fprintf(stderr,"%s -i input_file_name -o output_file_name [-f process threshold]\n",argv[0]);
				fputs("Default process threshold as 8. less to more shapen , but cause particle artifact.\n",stderr);
				fputs("more to eliminate particles.\n",stderr);
		}
				
	}
	switch( parameters )
	{
		case 2:
			fputs("Error: Input file not specified.\n",stderr);
			return 8;
		case 1:
			fputs("Error: Output file not specified.\n",stderr);
			return 7;
		case 3:
			break;
		default:
			fputs("Error: No parameters specified.\n",stderr);
			return 6;
	}
		
	if( GD_FALSE == gdSupportsFileType( output_file_name ,1 ) )
	{
		fputs("Error: Output file Format not supported\n",stderr);
		return 3;
	}
	input_image=gdImageCreateFromFile( input_file_name );
	if( input_image == NULL )
	{
		fputs("Error: Can not open the input file\n",stderr);
		return 2;
	}
	qint_image = quantum_interpole(input_image , threshold);
	gdImageSetInterpolationMethod( qint_image , GD_LINEAR );
	outputSX = gdImageSX( input_image ) + gdImageSX( input_image ) / 2 ;
	outputSY = gdImageSY( input_image ) + gdImageSY( input_image ) / 2 ;
	output_image = gdImageScale( qint_image , outputSX , outputSY );
	if( GD_FALSE == gdImageFile(output_image,output_file_name ))
	{
		fputs("Error: Can not save Output image file\n",stderr);

		gdImageDestroy(input_image);
		gdImageDestroy(qint_image);
		gdImageDestroy(output_image);
		return 5;
	}
	gdImageDestroy(input_image);
	gdImageDestroy(qint_image);
	gdImageDestroy(output_image);
	return 0;
}
