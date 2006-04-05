
extern XImage *image;
extern XImage *subimage;
extern XImage *magnified_image;

extern struct long_pos_buf *scans[8192];
extern struct qmips_sonar *qmipsScans[4096];

extern int scansize;
extern int datasize;
extern int headsize;
extern int data_reducer;
extern int inbytes, fp1;
extern int swath_width;
extern int fileHeaderSize;

extern unsigned char *sonar_data;
extern unsigned char *subbottom_data;
extern unsigned char *trailer;

extern unsigned char *sub_sample_sonar;
extern unsigned char *preMagnifySonar;
extern unsigned char *display_data;
extern unsigned char *zoom_data;
extern unsigned short *sixteenBitData;
extern unsigned short *zoomSixteenBitData;


