/*
 * Filename   : bmp2logo.c
 
 * Program    : Transform the 24 bits's BMP to 16, 24, 32 bits logo tools

 
 * Copyright  : Copyright (C) 2009~2020 XXX Co., LTD.
 * Author     : behard
 * Created at : 2009.01.09

 */
#include <stdio.h>
#include <stdlib.h>

typedef struct			/* 24 bit BMP color */
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ColorBGR24BIT;

#define MAXROWTEXT  200

char strHead[][MAXROWTEXT] = {
  "/*\r\n",
  " * Filename   : linux/drivers/video/truecolor_logo.c\r\n",

  " * Program    : Show the logo with 16, 24, 32 bits color not 224 colors\r\n",

  " * Copyright  : Copyright (C) 2009~2020 XXX Co., LTD.\r\n",

  " * Author     : behard\r\n",

  " * Created at : 2009.01.09\r\n",

  " */\r\n",

  "#include <linux/module.h>\r\n",

  "#include <linux/kernel.h>\r\n",

  "#include <linux/errno.h>\r\n",

  "#include <linux/string.h>\r\n",

  "#include <linux/mm.h>\r\n",

  "#include <linux/tty.h>\r\n",

  "#include <linux/slab.h>\r\n",

  "#include <linux/vmalloc.h>\r\n",

  "#include <linux/delay.h>\r\n",

  "#include <linux/interrupt.h>\r\n",

  "#include <linux/dma-mapping.h>\r\n",

  "#include <linux/platform_device.h>\r\n",

  "\r\n",

  "#include <asm/uaccess.h>\r\n",

  "#include <linux/fb.h>\r\n",

  "#include <linux/init.h>\r\n",

  "\r\n",

  "#include \"truecolor_logo.h\"\r\n",

  "\r\n",

  "// #define    DEBUG_TRUECOLOR_LOGO    /* Debug Output Switch */\r\n",

  "\r\n",

  "#ifdef CONFIG_FB_TRUECOLOR_LOGO\r\n",

  "static struct TrueColor\r\n",

  "{\r\n",

  "    int Width, Height;\r\n",

  "    int ColorBytes;\r\n",

  "    const char *BitData;\r\n",

  "} _true_color_logo;\r\n",
  "\r\n",
};


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


  for (i = 0; i < sizeof (strHead) / MAXROWTEXT; i++)

    fprintf (fpCxx, strHead[i]);


  // color * width memory
  colorRead24 = (ColorBGR24BIT *) malloc (width * sizeof (ColorBGR24BIT));

  // BPX==32
  out_counter = 0;
  fprintf (fpCxx, "#if (BPX==32)\r\n");
  fprintf (fpCxx, "\r\n");
  fprintf (fpCxx,
	   "/* R(8)/G(8)/B(8)/0(8) code for little endian system */\r\n");
  fprintf (fpCxx, "static unsigned char truecolor_logo_data[] =\r\n");
  fprintf (fpCxx, "{\r\n");

  fseek (fpBMP, 54, SEEK_SET);	// BMP file information head
  for (j = height - 1; j >= 0; j--)
    {
      fread (colorRead24, width * 3, 1, fpBMP);
      if (width % 4)
	fseek (fpBMP, (width % 4), SEEK_CUR);	// fix width and file pointer 24-bits

      for (i = 0; i < width; i++)
	{
	  if (0 == (out_counter % 6))
	    fprintf (fpCxx, "\r\n\t");
	  out_counter++;
	  // R/G/B/0 code for little endian system, the code for big endian system is not test
	  fprintf (fpCxx, "0x%02x, 0x%02x, 0x%02x, 0x00, ", colorRead24[i].r,
		   colorRead24[i].g, colorRead24[i].b);
	}
    }
  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "};\r\n");

  fprintf (fpCxx, "\r\n");



  // BPX==24

  out_counter = 0;

  fprintf (fpCxx, "#elif (BPX==24)\r\n");
  fprintf (fpCxx, "\r\n");
  fprintf (fpCxx, "/* R(8)/G(8)/B(8) code for little endian system */\r\n");
  fprintf (fpCxx, "static unsigned char truecolor_logo_data[] =\r\n");
  fprintf (fpCxx, "{\r\n");

  fseek (fpBMP, 54, SEEK_SET);	// BMP file information head
  for (j = height - 1; j >= 0; j--)
    {
      fread (colorRead24, width * 3, 1, fpBMP);
      if (width % 4)
	fseek (fpBMP, (width % 4), SEEK_CUR);	// fix width and file pointer 24-bits

      for (i = 0; i < width; i++)
	{
	  if (0 == (out_counter % 8))
	    fprintf (fpCxx, "\r\n\t");
	  out_counter++;
	  // R/G/B: code for little endian system, the code for big endian system is not test
	  fprintf (fpCxx, "0x%02x, 0x%02x, 0x%02x, ", colorRead24[i].r,
		   colorRead24[i].g, colorRead24[i].b);
	}
    }
  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "};\r\n");

  fprintf (fpCxx, "\r\n");



  // BPX==16

  out_counter = 0;

  fprintf (fpCxx, "#elif (BPX==16)\r\n");
  fprintf (fpCxx, "\r\n");
  fprintf (fpCxx, "/* R(5)/G(6)/B(5) code for little endian system */\r\n");
  fprintf (fpCxx, "static unsigned char truecolor_logo_data[] =\r\n");
  fprintf (fpCxx, "{\r\n");

  fseek (fpBMP, 54, SEEK_SET);	// BMP file information head
  for (j = height - 1; j >= 0; j--)
    {
      fread (colorRead24, width * 3, 1, fpBMP);
      if (width % 4)
	fseek (fpBMP, (width % 4), SEEK_CUR);	// fix width and file pointer 24-bits

      for (i = 0; i < width; i++)
	{
	  if (0 == (out_counter % 12))
	    fprintf (fpCxx, "\r\n\t");
	  out_counter++;
	  // R(5)/G(6)/B(5): code for little endian system
	  Value_16 =
	    ((colorRead24[i].r >> 3) << 11) | ((colorRead24[i].
						r >> 2) << 5) |
	    (colorRead24[i].b >> 3);
	  fprintf (fpCxx, "0x%02x, 0x%02x, ", (Value_16 >> 8),
		   (Value_16 & 0xFF));
	}
    }
  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "};\r\n");

  fprintf (fpCxx, "\r\n");



  // BPX < 16 (Don't support)

  fprintf (fpCxx, "#else\r\n");

  fprintf (fpCxx, "\r\n");
  fprintf (fpCxx, "static unsigned char truecolor_logo_data[] =\r\n");
  fprintf (fpCxx, "{ 0, }; \r\n");
  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "#endif // BPX \r\n");

  fprintf (fpCxx, "\r\n");



  // Set the logo data information and the fb_prepare_truecolor_logo function

  fprintf (fpCxx, "int fb_prepare_truecolor_logo(void)\r\n");

  fprintf (fpCxx, "{\r\n");
  fprintf (fpCxx, "\t_true_color_logo.ColorBytes = BPX/8;\r\n");
  fprintf (fpCxx, "\t_true_color_logo.Width = %d;\r\n", width);
  fprintf (fpCxx, "\t_true_color_logo.Height = %d;\r\n", height);

  fprintf (fpCxx, "\t_true_color_logo.BitData = truecolor_logo_data;\r\n");

  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "#ifdef DEBUG_TRUECOLOR_LOGO\r\n");

  fprintf (fpCxx, "\tprintk(\"fb_prepare_truecolor_logo ok \\n\");\r\n");

  fprintf (fpCxx, "#endif\r\n");

  fprintf (fpCxx,
	   "\treturn (_true_color_logo.ColorBytes > 1); /* Only support 16 Bits or more color*/\r\n");

  fprintf (fpCxx, "}\r\n");

  fprintf (fpCxx, "\r\n");



  // Set the fb_show_truecolor_logo function

  fprintf (fpCxx, "void fb_show_truecolor_logo(struct fb_info *info)\r\n");

  fprintf (fpCxx, "{\r\n");

  fprintf (fpCxx, "\tchar *framebase = info->screen_base;\r\n");

  fprintf (fpCxx, "\tint m, n;\r\n");

  fprintf (fpCxx, "\tint startx, starty;\r\n");

  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "\tstartx = (XRES- _true_color_logo.Width)/ 2;\r\n");

  fprintf (fpCxx, "\tif(startx < 0) startx = 0;\r\n");

  fprintf (fpCxx, "\tstarty = (YRES- _true_color_logo.Height)/ 2;\r\n");

  fprintf (fpCxx, "\tif(starty < 0) starty = 0;\r\n");

  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "#ifdef DEBUG_TRUECOLOR_LOGO\r\n");

  fprintf (fpCxx,
	   "\tprintk(\"fb_show_truecolor_logo:: %%d %%d %%d %%d \\n\", XRES, YRES, startx, starty);\r\n");

  fprintf (fpCxx, "#endif\r\n");

  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "\tfor(m=0; m<YRES; m++)\r\n");

  fprintf (fpCxx, "\t{\r\n");

  fprintf (fpCxx,
	   "\t\tif((m < starty) || (m >= _true_color_logo.Height+starty)) // Set the background color to black\r\n");

  fprintf (fpCxx, "\t\t{\r\n");

  fprintf (fpCxx,
	   "\t\t\tmemset(framebase+ (m* XRES* _true_color_logo.ColorBytes), 0, XRES* _true_color_logo.ColorBytes);\r\n");

  fprintf (fpCxx, "\t\t}\r\n");

  fprintf (fpCxx, "\t\telse\r\n");

  fprintf (fpCxx, "\t\t{\r\n");

  fprintf (fpCxx, "\t\t\t// Set the left to black color\r\n");

  fprintf (fpCxx,
	   "\t\t\tmemset(framebase+ ((m* XRES)* _true_color_logo.ColorBytes), 0, _true_color_logo.ColorBytes* startx);\r\n");

  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "\t\t\t// Set logo data, the vertical must upend\r\n");

  fprintf (fpCxx,
	   "\t\t\tmemcpy(framebase+ ((m* XRES+ startx)* _true_color_logo.ColorBytes), \r\n");

  fprintf (fpCxx,
	   "\t\t\t\t// (_true_color_logo.BitData+ ((m-starty)* _true_color_logo.Width)* _true_color_logo.ColorBytes), \r\n");

  fprintf (fpCxx,
	   "\t\t\t\t(_true_color_logo.BitData+ ((_true_color_logo.Height-(m-starty)-1)* _true_color_logo.Width)* _true_color_logo.ColorBytes), \r\n");

  fprintf (fpCxx,
	   "\t\t\t\t_true_color_logo.ColorBytes* _true_color_logo.Width);\r\n");

  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "\t\t\t// Set the right to black color\r\n");

  fprintf (fpCxx,
	   "\t\t\tmemset(framebase+ ((m* XRES+ startx+ _true_color_logo.Width)* _true_color_logo.ColorBytes), 0, \r\n");

  fprintf (fpCxx,
	   "\t\t\t\t_true_color_logo.ColorBytes*(XRES- startx- _true_color_logo.Width));\r\n");


  fprintf (fpCxx, "\t\t}\r\n");

  fprintf (fpCxx, "\t}\r\n");

  fprintf (fpCxx, "}\r\n");

  fprintf (fpCxx, "\r\n");


  fprintf (fpCxx, "#else // CONFIG_FB_TRUECOLOR_LOGO\r\n");

  fprintf (fpCxx, "\r\n");

  fprintf (fpCxx, "int fb_prepare_truecolor_logo(void) { return 0; } \r\n");

  fprintf (fpCxx,
	   "void fb_show_truecolor_logo(struct fb_info *info) { return; } \r\n");

  fprintf (fpCxx, "\r\n");


  fprintf (fpCxx, "#endif // CONFIG_FB_TRUECOLOR_LOGO\r\n");

  fprintf (fpCxx, "\r\n");


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
      printf ("Use: bmp2logo xxx.bmp (truecolor_logo.c)\n");
#ifdef  __WIN32__
      system ("Pause");
#endif
      return;
    }

  if (argc > 2)
    strcpy (CxxFileName, argv[2]);
#ifdef  __WIN32__
  else
    strcpy (CxxFileName, ".\\truecolor_logo.c");
#else
  else
    strcpy (CxxFileName, "./truecolor_logo.c");
#endif

  if (!(Tran_24bit_BMP (argv[1], CxxFileName)))
    {
      printf ("Transform %s to truecolor_logo.c failure \n", argv[1]);
#ifdef  __WIN32__
      system ("Pause");
#endif
      return -1;
    }

  printf ("Transform %s to truecolor_logo.c success \n", argv[1]);
#ifdef  __WIN32__
  system ("Pause");
#endif
  return 0;
}
