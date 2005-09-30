/*
**   segy sidescan sonar data format
*/

#define BYTE unsigned char	/* segy byte */
#define CHAR unsigned char

#define NBYTES_HEADER             32
#define NUMSAMPS                1768
#define NUM_BYTES_PER_SIDE       884
#define NBYTES_PER_RECORD       1800

typedef struct {

   BYTE NotUsed;
   BYTE seconds;		/* TIME:  	sec	0-60 */
   BYTE minute;			/*        	min	0-60 */
   BYTE hour;			/*        	hour	0-24 */
   BYTE year;			/* DATE:  	year	0-99 */
   BYTE month;			/*        	month	0-12 */
   BYTE day;			/*        	day	0-31 */
   BYTE fish_height_tenths;	/* FISH HEIGHT:	tenths	0-90 */
   BYTE fish_height_tens;	/* 		tens	0-6  */
   BYTE fix_inits;		/* FIX:		0-99 */
   BYTE fix_hundreds;		/*		0-99 */
   BYTE speed_tenths;		/* SPEED:	0-99 */
   BYTE speed_tens;		/*		0-1  */
   BYTE dsf_heading_degrees;	/* DSF HDG:	0-99 */
   BYTE dsf_heading_hundreds;	/*		0-3  */
   BYTE	flags;			/* FLAGS:  1B00 B00B */
   BYTE range_meters;		/* 260 RANGE:	0-99 */
   BYTE range_hundreds;		/* 		0-6  */
   CHAR lon_sign;		/* LONGITUDE: 	E/W  		     */
   BYTE lon1;			/* 		xxxxx.xBB	0-99 */
   BYTE lon2;			/* 		xxxxB.Bxx	0-99 */
   BYTE lon3;			/* 		xxBBx.xxx	0-99 */
   BYTE lon4;			/* 		BBxxx.xxx	0-99 */
   CHAR lat_sign;               /* LATITUDE:    E/W                  */
   BYTE lat1;                   /* 		xxxxx.xBB    	0-99 */
   BYTE lat2;                   /* 		xxxxB.Bxx    	0-99 */
   BYTE lat3;                   /* 		xxBBx.xxx    	0-99 */
   BYTE lat4;                   /* 		BBxxx.xxx    	0-99 */
   BYTE nav_heading_degrees;	/* NAV HDG:	0-99 */
   BYTE nav_heading_hundreds;	/*		0-3  */
   BYTE dsf_tsp_flags;		/* DSF TSP FLAGS     */
   BYTE data_block_id;		/* DATA BLOCK ID -- Bytes 18-27 */
				/*		81H = Nav	*/
				/*		82H = Mag	*/

} EggHeader;
