#include <unistd.h>
#include <assert.h>

#include "showimage2.h"
#include "sonar.h"
#include "externals.h"

#define EIGHT_BITS       8
#define FILEHEADERSIZE   1024

int getHeaderInfo(struct draw *drawarea)
/*struct draw *drawarea;*/
{
    void get_swath();
    int get_scansize();

    unsigned short get_short();
    short sampleRate;
    short numSonarChannels;

    struct long_pos_buf usgs;

    int bytesPerPixel;
    int status;
    int newQmips;

    long position;

    unsigned char inbuf[1024];
    /*unsigned char header[256];*/

    ImageHeader rasterHeader;

    assert(drawarea);

    if(drawarea->fileType == USGS)
        {
        position = lseek(fp1, 0L, SEEK_SET);
        inbytes = read(fp1, &usgs, 256);
        position = lseek(fp1, -256L, SEEK_CUR);

        if(usgs.fileType != XSONAR)
            {
            message_display(drawarea, NOTXSONAR);
            position = lseek(fp1, 0L, SEEK_SET);
            status = 0;
            return(status);
            }

        fileHeaderSize = 0;

        scansize = usgs.sdatasize;

    /*
     *    Need to reset what get_scansize() returns.  Should be the
     *    number of bits/pixel, or 0 if the cancel button was hit.
     */

        if(scansize == (int) NULL || (scansize - 256) % 512)
            status = get_scansize(drawarea, 1);
        else
            {
            headsize = 256;
            datasize = scansize - headsize; /* - headsize */
            status = 8; /* bits per pixel */
            }



       if(!status)
           {
           message_display(drawarea, NOSCAN);
           return(status);
           }

        status = 8; /* bits per pixel */

        swath_width = usgs.swath_width;

        if(swath_width == (int) NULL || swath_width == 0 ||
                   swath_width < 0 || swath_width > 50000)
            get_swath(drawarea);


        }

    if(drawarea->fileType == QMIPS)
        {
        newQmips = 0;

        inbytes = read(fp1, inbuf, FILEHEADERSIZE);

        if(inbuf[0] != 50)
            {
            inbytes = read(fp1, inbuf, FILEHEADERSIZE);
            newQmips++;
            fileHeaderSize = 2048;
            }
        else
            fileHeaderSize = 1024;

        if(inbuf[0] != 50)
            {
            message_display(drawarea, NOTQMIPS);
            status = 0;
            position = lseek(fp1, 0L, SEEK_SET);
            return(status);
            }

        drawarea->fileType = QMIPS;

/*
        if(inbuf[1] == 84)
            drawarea->fileType = QMIPS_DSP;
*/

        sampleRate = get_short(inbuf, 26);
        drawarea->numImageryChannels = get_short(inbuf, 30);
        drawarea->bitsPerPixel = get_short(inbuf, 32);
        drawarea->pixelsPerChannel = get_short(inbuf, 34);
        numSonarChannels = get_short(inbuf, 46);
        bytesPerPixel = drawarea->bitsPerPixel / EIGHT_BITS;


        scansize = drawarea->pixelsPerChannel * drawarea->numImageryChannels * 
                                                  bytesPerPixel + 256;
        headsize = 0;
        datasize = drawarea->pixelsPerChannel * 2;   /* sonar channels only */

        if(newQmips)
            position = lseek(fp1, 2048, SEEK_SET);
        else
            position = lseek(fp1, 1024, SEEK_SET);

        status = drawarea->bitsPerPixel;


fprintf(stdout, "fileFormat = %d\n", inbuf[0]);
fprintf(stdout, "systemType = %d\n", inbuf[1]);
fprintf(stdout, "software rev = %.6s\n", &inbuf[2]);
fprintf(stdout, "sampleRate = %d\n", get_short(inbuf, 26));
fprintf(stdout, "sample rate = %d\n", sampleRate);
fprintf(stdout, "num channels = %d\n", drawarea->numImageryChannels);
fprintf(stdout, "num Sonar channels = %d\n", numSonarChannels);
fprintf(stdout, "bits per pixel = %d\n", drawarea->bitsPerPixel);
fprintf(stdout, "pixels per channel = %d\n", drawarea->pixelsPerChannel);
fprintf(stdout, "bytesPerPixel = %d\n", bytesPerPixel);
fprintf(stdout, "datasize = %d, scansize = %d\n", datasize, scansize);
fprintf(stdout, "sonar range = %f\n", get_float(inbuf, 92));

        }


    if(drawarea->fileType == RASTER)
        {
        inbytes = read(fp1, &rasterHeader, sizeof(rasterHeader));

        if(rasterHeader.type != RASTER)
            {
            message_display(drawarea, NOTRASTER);
            status = 0;
            position = lseek(fp1, 0L, SEEK_SET);
            return(status);
            }


        fileHeaderSize = sizeof(rasterHeader);

        scansize = datasize = rasterHeader.width;
        headsize = 0;

        status = 8;
        }
        

    /*
     *    Need to reset what get_scansize() returns.  Should be the
     *    number of bits/pixel, or 0 if the cancel button was hit.
     */

    if(drawarea->fileType == UNDEFINED)
        {
        status = get_scansize(drawarea, 0);
        if(status)
            status = 8;

        if(fileHeaderSize)  /* for UNDEFINED file types */
            position = lseek(fp1, (off_t) fileHeaderSize, SEEK_SET);

        }


    return(status);

}
