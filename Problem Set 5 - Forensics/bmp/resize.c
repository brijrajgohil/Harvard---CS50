/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Resizes a BMP by a factor of n
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[]) {
	// ensure proper usage
	if (argc != 4)
	{
		printf("Usage: resize n infile outfile\n");
		return 1;
	}

	// remember filenames
	char *infile = argv[2];
	char *outfile = argv[3];

	//get the factor
	int resizeFactor = atoi(argv[1]);
	if (resizeFactor < 1 || resizeFactor > 100)
	{
		printf("Usage: resize n infile outfile\n");
		return 1;
	}

	// open input file
	FILE *inptr = fopen(infile, "r");
	if (inptr == NULL)
	{
		printf("Could not open %s.\n", infile);
		return 2;
	}

	// open output file
	FILE *outptr = fopen(outfile, "w");
	if (outptr == NULL)
	{
		fclose(inptr);
		fprintf(stderr, "Could not create %s.\n", outfile);
		return 3;
	}

	// read infile's BITMAPFILEHEADER
	BITMAPFILEHEADER bf;
	fread(&bf, sizeof (BITMAPFILEHEADER), 1, inptr);

	// read infile's BITMAPINFOHEADER
	BITMAPINFOHEADER bi;
	fread(&bi, sizeof (BITMAPINFOHEADER), 1, inptr);

	// ensure infile is (likely) a 24-bit uncompressed BMP 4.0
	if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
					bi.biBitCount != 24 || bi.biCompression != 0)
	{
		fclose(outptr);
		fclose(inptr);
		fprintf(stderr, "Unsupported file format.\n");
		return 4;
	}

	/*
	 * how:
	 * change the header file for the new file.
	 * calculate the padding for the new file.
	 * make a buffer to hold the entire line of pixels for the outfile.
	 */

	int oldWidth = bi.biWidth; //save old width
	bi.biWidth = bi.biWidth * resizeFactor; // the new width

	int oldHeight = bi.biHeight; // save old height
	bi.biHeight = bi.biHeight * resizeFactor; // the new height

	// determine padding for scanlines
	int newpadding = (4 - (bi.biWidth * sizeof (RGBTRIPLE)) % 4) % 4;
	int oldpadding = (4 - (oldWidth * sizeof (RGBTRIPLE)) % 4) % 4;

	bi.biSizeImage = abs(bi.biHeight) * ((bi.biWidth * sizeof (RGBTRIPLE)) + newpadding); // determine the new size of the image
	bf.bfSize = bi.biSizeImage + sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER); // determine the new file size.

	RGBTRIPLE scanline[bi.biWidth];

	//printf("%d: bi.biWidth=%d, bi.biHeight=%d, bi.biSizeImage=%d, bf.bfSize=%d\n",
	//				__LINE__, bi.biWidth, bi.biHeight, bi.biSizeImage, bf.bfSize);

	// write outfile's BITMAPFILEHEADER
	fwrite(&bf, sizeof (BITMAPFILEHEADER), 1, outptr);


	// write outfile's BITMAPINFOHEADER
	fwrite(&bi, sizeof (BITMAPINFOHEADER), 1, outptr);


	// iterate over infile's scanlines
	for (int i = 0, biHeight = abs(oldHeight); i < biHeight; i++)
	{
		int cursor = 0;
		// iterate over pixels in scanline
		for (int j = 0; j < oldWidth; j++)
		{
			// temporary storage
			RGBTRIPLE triple;

			// read RGB triple from infile
			fread(&triple, sizeof (RGBTRIPLE), 1, inptr);

			for (int c = 0; c < resizeFactor; c++)
			{
				if (cursor > bi.biWidth)
				{
					printf("%d:Error cursor > bi.biWidth (%d > %d)\n", __LINE__, cursor, bi.biWidth);
					return (1);
				}
				scanline[cursor].rgbtBlue = triple.rgbtBlue;
				scanline[cursor].rgbtGreen = triple.rgbtGreen;
				scanline[cursor].rgbtRed = triple.rgbtRed;
				cursor++;
			}
		} // now I have read the entire line

		for (int h = 0; h < resizeFactor; h++)
		{
			//write out the new resized scanline
			for (cursor = 0; cursor < bi.biWidth; cursor++)
			{
				// write RGB triple to outfile
				fwrite(&scanline[cursor], sizeof (RGBTRIPLE), 1, outptr);
			}

			// write padding to outfile
			for (int k = 0; k < newpadding; k++)
				fputc(0x00, outptr);
		}

		// skip over padding, if any
		fseek(inptr, oldpadding, SEEK_CUR);
	}

	// close infile
	fclose(inptr);

	// close outfile
	fclose(outptr);

	// that's all folks
	return (EXIT_SUCCESS);
}
