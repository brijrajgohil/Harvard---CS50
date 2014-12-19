/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */
#include <stdio.h>
#include <stdint.h>

// define unsigned int BYTE, 1 byte in size
typedef uint8_t BYTE;

// function to check the 4 first bytes of buffer
int checkJPG(BYTE* buffer);

int main(int argc, char* argv[])
{
	// open raw card file
    FILE* card = fopen("card.raw", "r");
    if (card == NULL)
    {
    	printf("Could not open card!\n");
    	return 1;
    }
    // declare buffer for card
    BYTE buffer[512];
    // declare buffer for names of images + counter
    char imgTitle[10];
	int i = 0;

	// while 512 bytes is being read from raw file
    while (fread(&buffer, 512, 1, card) == 1)
    {
    	// while the block starts with the signature bytes for jpg
    	while (checkJPG(buffer))
    	{
    		// print name of img to imgTitle, starting with 000.jpg
    		// %0*d means print i, and always take up 3 spaces. Use 0 as padding.
			sprintf(imgTitle, "%0*d.jpg", 3, i);

			// open img in write
			FILE *img = fopen(imgTitle, "w");

			// write the first block of data to img
			fwrite(&buffer, 512, 1, img);

			// while a block is being read, and the block does
			// not start with signature jpg bytes (if it does, it signals
			// the end of the jpg, and the beginning of a new)
			while (fread(&buffer, 512, 1, card) == 1 && !checkJPG(buffer))
			{
				// write the block of data to img
				fwrite(&buffer, 512, 1, img);
			}

			// close the img file when encountering signature bytes
			fclose(img);

			// increment i by 1, so as to name the next image +1
			i++;
    	}
    }
	fclose(card);
}

int checkJPG(BYTE* buffer)
{
	// if the buffer starts with signature bytes, return true
	if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] == 0xe0 || buffer[3] == 0xe1))
	{
		return 1;
	}
	// else return false
	else
	{
		return 0;
	}
}
