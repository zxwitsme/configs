#include <stdio.h>
#include <stdlib.h>

typedef struct			/* 24 bit BMP color */
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ColorBGR24BIT;

#define MAXROWTEXT  200


int
Tran_24bit_BMP (const char *BMPFile, const char *CxxFile)
{

  int width, height;

  short int bitcount;

  FILE *fpBMP, *fpCxx;

  ColorBGR24BIT *colorRead24;	// read the color data(24 bit)
  int i, j;

  int out_counter;

  unsigned short int Value_16;

  fpBMP = fopen (BMPFile, "rb");

  fpCxx = fopen (CxxFile, "wb");


  if ((NULL == fpBMP) || (NULL == fpCxx))
    {
      if (NULL == fpBMP)
	printf ("Read %s file failure. \n", BMPFile);
      else
	fclose (fpBMP);
      if (NULL == fpCxx)
	printf ("Create %s file failure. \n", CxxFile);
      else
	fclose (fpCxx);
      return 0;
    }

  fseek (fpBMP, 18, SEEK_SET);
  fread (&width, sizeof (width), 1, fpBMP);
  fread (&height, sizeof (height), 1, fpBMP);
  fseek (fpBMP, 28, SEEK_SET);
  fread (&bitcount, sizeof (bitcount), 1, fpBMP);


  printf ("\n Width=%d, Height=%d Bits=%d.\n", width, height, (int) bitcount);


  if (24 != bitcount)

    {

      fclose (fpBMP);

      fclose (fpCxx);

      printf ("Only support 24 bitmap file. \n");
      return 0;

    }



  // color * width memory
  colorRead24 = (ColorBGR24BIT *) malloc (width * sizeof (ColorBGR24BIT));

  // BPX==32
  out_counter = 0;
  fprintf (fpCxx,
	   "/* R(8)/G(8)/B(8) code for little endian system */\n");
  fprintf (fpCxx, "static unsigned char truecolor_logo_data[] =\n");
  fprintf (fpCxx, "{\n");

  fseek (fpBMP, 54, SEEK_SET);	// BMP file information head
  for (j = 0; j < height; j++)
    {
      fread (colorRead24, width * 3, 1, fpBMP);
      if (width % 4)
	fseek (fpBMP, (width % 4), SEEK_CUR);	// fix width and file pointer 24-bits

      for (i = 0; i < width; i++)
	{
	  if (0 == (out_counter % 6))
	    fprintf (fpCxx, "\n\t");
	  out_counter++;
	  // R/G/B code for little endian system, the code for big endian system is not test
	  fprintf (fpCxx, "0x%02x, 0x%02x, 0x%02x, 0x00, ", colorRead24[i].r,
		   colorRead24[i].g, colorRead24[i].b);
	}
    }
  fprintf (fpCxx, "\n");

  fprintf (fpCxx, "};\n");

  fprintf (fpCxx, "\n");


  free (colorRead24);

  fclose (fpBMP);

  fclose (fpCxx);


  return 1;

}


int
main (int argc, char *argv[])
{
  char CxxFileName[100];
  if (argc < 2)
    {
      printf ("Use: bmp2logo xxx.bmp (pic.c)\n");
      return;
    }

  if (argc > 2)
    strcpy (CxxFileName, argv[2]);
  else
    strcpy (CxxFileName, "./pic.c");

  if (!(Tran_24bit_BMP (argv[1], CxxFileName)))
    {
      printf ("Transform %s to pic.c failure \n", argv[1]);
      return -1;
    }

  printf ("Transform %s to pic.c success \n", argv[1]);
  return 0;
}
