#ifndef _ALDEN_H_
#define _ALDEN_H_

/*	Defines for the Alden Thermal Recorder Structure	*/
/*								*/
/*	TFO							*/
/*	4 Febury 1995						*/
/*	USGS 							*/
/*	Quissett Campus						*/
/*	Woods Hole, MA. 02543					*/
/*	508.457.2310						*/

#include <netinet/in.h>

typedef struct
{
	char	Start_Of_Header;	/* First byte = ASCII SOH */
	short	Left_Margin;		/* Start position 0 -2047 */
	short	Line_Length;		/* Bytes/line 1 - 2048 */
	short	Image_Length;		/* scans/file 1 - 65535 */
	char	Margin_Color;		/* color of both margins 0-255 */
	char	Line_Repeat;		/* # of times to dup. 0 - 7 */
	char	Vertical_Compression;	/* see below */
	char	Pixel_Repeat;		/* # times pixel duped 0 - 7 */
	char	Translation_Select;	/* see below */
	char	Image_Type;		/* see below */
	char	Image_Number[4];	/* see below */
	char	Line_Spacing;		/* distance between scans */
	char	Line_Rate;		/* see below */
	char	Paper_Type;		/* see below */
	short	Feed_Length;		/* see below */
	char	Reserved[10];		/* not used */
	char	Printed_Header[96];	/* see below */
	char	Translation_Table[256]; /* see below */
} AldenHeader;

/*	From the Alden Thermal Printer manual

Start_Of_header	The first byte in the header is an ASCII SOH (01H) char

Left_Margin	Starting position (0 - 2047) of the left edge of the image.
		The most significant byte first; larger values are truncated.
		If an invalid margin parameter is given, the margin will be
		set to 0.

Line_Length	(1 - 2048) The number of transferred bytes in a line. In
		gray scale mode a byte represents one pixel of data. In
		monochrome mode a byte represents 8 pixels of data. The most
		significant byte first, larger values are truncated.

Image_Length	(0, 1 - 65535) The total number of lines in the image, the
		most significant byte is first. 0 specifies that the image
		length is ignored; -INPUT PRIME is required to terminate
		the image.

Margin_Color	(0 - 255) This byte identifies the color of the left and
		right margins. The value of the color selection is converted
		via the current translation table.

Line_Repeat	(0 - 7) The number of times the line is duplicated in the
		vertical direction

Vertical_Compression	(-1 to -128, +1 to +127 or 0) The output line count
		for duplicating or removing one line in the vertical
		direction thus providing fine adjustment of the image
		aspect ratio. If 0, no compression or expansion takes
		place. Negative numbers remove lines, positive numbers
		add lines.

Pixel_Repeat	(0 - 7) The number of times the pixel is duplicated in
		the horizontal direction.

Translation_Table (0, 1 or 255) This byte selects one of the internal
		translation tables, or indicates that the translation
		table supplied by the host is to be used. A value of 255
		designates a user defined translation table. Two standard
		table are 0 - linear 0 - 255 and 1 - inverted linear 255 - 0.

Image_Type	(0, 2 or 3) This byte selects the image type for this
		transfer: 0 = gray scale, 8 bits per pixel; 2 = monochrome
		1 bit per pixel and 8 pixels per byte, the most significant
		bit corresponds to the left most pixel; 3 = the high speed
		binary mode. In the high speed binary mode the line length
		must be 2048 pixels, image data is 256 bytes per line with
		each byte containing 8 pixels in reversed order. In this
		mode the printer may operate at up to 130 lines per second.

Image_Number	A four digit ASCII number used to identify the image. Any
		valid ASCII character, including spaces, can be used. The
		image number is printed at the end of the header.

Line_Spacing	(6 - 9) This byte selects the distance between printed
		lines. The value should be set to 8 for optimum printing
		density. This is only valid in monochrome mode.

Line_Rate	(LPS 15 - 30) The line rate parameter is used to match
		the printing speed to the host computer's transfer speed.
		If this parameter is above the transfer rate of the host 
		computer, dark lines may result in the image from the host.
		If set to low, the image will take longer to print. The
		current rate for gray scale is 15 - 30 LPS.

Paper_Type	(0, 1 - 5) 0 indicates paper tyoe is selected through the
		control panel. 1 - 5 select paper types: plastic (1,2,3)
		fiber (4), and film (5).

Feed_Length	(0, 1 - 8191) 0 indicates the default value (360) is used.
		A value of 1 - 8191 specifies the number of lines to feed
		paper after the end of an image. A value of FFFF specifies
		no paper feed.

Printed_Header	The printed header is a string of ASCII characters that are
		printed at the top of the paper before the top margin. Only
		printable ASCII characters in the range of 20 - 7F should be
		used. If the first byte of the field is 00H, neither the 
		header not the image number will be printed.

Translation_Table A 256 byte table which converts toneshades to the printer
		toneshades. If the table is not used, this field is ignored.


*/


#endif /*  _ALDEN_H_ */
