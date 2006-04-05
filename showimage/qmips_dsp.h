#define       FORMAT0   0     /* pic-file format    */
#define       FORMAT1   1     /* old format (smips) */
#define       FORMAT2   50    /* new format (qmips) */


/* how will the nav data be saved? (default) */
#define METERS  0
#define FEET    1
#define YARDS   2
#define DEGREES 3
#define NAV_UNITS METERS


/*
 *    These are defined in qmips.h for regular (as opposed to dsp) qmips files.
 */

#if !defined(BYTE)
#define BYTE unsigned char
#endif

#if !defined(WORD)
#define WORD unsigned short 
#endif

#if !defined(DWORD) 
#define DWORD unsigned long
#endif


#define	UCHAR	unsigned char

struct dsp_header {
/****************************************************************************/
   /* header record */
   /*****************/                                            /* offset */
   BYTE     fileFormat;  /* will be 50 */                          /* 0 */
   BYTE     systemType;  /* will be 84 for Q-MIPS DSP */           /* 1 */
   BYTE     softwareRev[6];                                        /* 2 */
   BYTE     spare1[18];                                            /* 8 */
   
   /* digitizer parameters */
   /*----------------------*/
   DWORD     sampleRate;              /* in samples/sec */          /* 26 */
   WORD     numImageryChannels;      /* 1-8 */                     /* 30 */
   WORD     bitsPerPixel;            /* 8 or 16 */                 /* 32 */
   WORD     pixelsPerChannelPerPing; /* (WORD COUNT) */            /* 34 */
   float    speedOfSoundInWater;     /* two way travel t: m/sec */ /* 36 */
   float    noLongerUsed;            /* used to be initial */      /* 40 */
                                     /* sonarSignalDivisor */
                                     /* before v5.19 */
   WORD     asyncChannelNumber;      /* if 0, no async chan.*/     /* 44 */
                                     /* Otherwise, which */
                                     /* channel is asynchronous */
   WORD     numSonarChannels;        /* 1-4 */                     /* 46 */
   WORD     ch1_processingAvailable;                               /* 48 */
   WORD     ch2_processingAvailable;                               /* 50 */
   WORD     ch3_processingAvailable;                               /* 52 */
   WORD     ch4_processingAvailable;                               /* 54 */
   
   /* nav system parameters */
   /*-----------------------*/
   WORD     timeDelay;                /* tenths of seconds */      /* 56 */
   BYTE     navSystemName[100];                                    /* 58 */
   BYTE     projectionType[12];                                    /* 158 */
   BYTE     spheriodType[12];                                      /* 170 */
   WORD     zone;                                                  /* 182 */
   float    originLat;                                             /* 184 */
   float    originLong;                                            /* 188 */
   float    offsetLat;                                             /* 192 */
   float    offsetLong;                                            /* 196 */
   WORD     navUnits;/* 0 = meters, 1 = feet, */                   /* 200 */
                     /* 2 = yards, 3 = degrees */
   
   
   /* site parameters */
   /*-----------------*/
   WORD     diveNumber;                                            /* 202 */
   WORD     blockNumber;                                           /* 204 */
   WORD     trackNumber;                                           /* 206 */
   WORD     runNumber;                                             /* 208 */
   BYTE     spare4[100];                                           /* 210 */
   
   /* annotation */
   /*------------*/
   BYTE     operatorAnnotation[100];                               /* 310 */
   BYTE     sonarName[40];                                         /* 410 */
   
   
   /* sonar parameters */
   /*------------------*/
   WORD     triggerDirection;  /* -1 is neg, 0 is equal, 1 is pos */ /* 450 */
   DWORD    triggerMagnitude;                                      /* 452 */
   float    triggerWidth;                                          /* 456 */
   
   WORD     ch1_frequency;     /* in kHz */                        /* 460 */
   WORD     ch2_frequency;                                         /* 462 */
   WORD     ch3_frequency;                                         /* 464 */
   WORD     ch4_frequency;                                         /* 466 */
   
   float    ch1_horizBeamAngle; /*degrees, read from DSP.DAT file*//* 468 */
   float    ch2_horizBeamAngle;                                    /* 472 */
   float    ch3_horizBeamAngle;                                    /* 476 */
   float    ch4_horizBeamAngle;                                    /* 480 */
   
   /* name of this file */
   /*-------------------*/
   char     thisFileName[45];                                      /* 484 */
   char     reserved2;                                             /* 529 */
   
   /* which channels are half-wave rectified */
   /*----------------------------------------*/
   WORD     ch1_halfWaveRectify;                                   /* 530 */
   WORD     ch2_halfWaveRectify;                                   /* 532 */
   WORD     ch3_halfWaveRectify;                                   /* 534 */
   WORD     ch4_halfWaveRectify;                                   /* 536 */


   /* Sonar Transducer Tilt Angles (off horizontal) */
   /*-----------------------------------------------*/
   float    ch1_tiltAngle; /* in degrees, read from DSP.DAT file */ /* 538 */
   float    ch2_tiltAngle;                                          /* 542 */
   float    ch3_tiltAngle;                                          /* 546 */
   float    ch4_tiltAngle;                                          /* 550 */

   /* Sonar Transducer beamWidth at 3dB points */
   /*------------------------------------------*/
   float    ch1_beamWidth_3dB; /*in degrees, read from DSP.DAT file*/ /* 554 */
   float    ch2_beamWidth_3dB;                                        /* 558 */
   float    ch3_beamWidth_3dB;                                        /* 562 */
   float    ch4_beamWidth_3dB;                                        /* 566 */

   float    ch1_realSampleRate;                                       /* 570 */
   float    ch2_realSampleRate;                                       /* 574 */
   float    ch3_realSampleRate;                                       /* 578 */
   float    ch4_realSampleRate;                                       /* 582 */

   /* left over */
   /*-----------*/
   BYTE     spare5[438];                                              /* 586 */

   
};


struct dsp_sonar {
/*****************************************************************/
/*  every ping ends with this 256-byte structure.  Note that not 
     all  fields  will  be used during every  run.   If  not  all 
     channels are being used, the remaining channels will contain 
     0's for all fields. 
*/

   /* time and date stamp */
   /*---------------------*/
   BYTE     day;
   BYTE     month;
   BYTE     year;
   BYTE     hour;
   BYTE     minute;
   BYTE     seconds;
   WORD     tenthsSeconds;
   
   /* misc. info */
   /*------------*/
   float    magX;
   float    magY;
   float    magZ;

   DWORD    pingNumber;

   WORD     asyncByteOffset;  /* when non-zero, indicates 
                                 a unique subbottom ping */

   WORD     shipSpeed;        /* speed of ship (knots * 100) */

   float    auxVal1;
   float    auxVal2;
   float    auxVal3;


   /* digitizer parameters */
   /*----------------------*/
   float    auxVal5;          /* Auxilary value from serial input */
   float    auxAltitude;      /* auxiliary altitude from subbottom sensor */

   WORD     triggerChannel;  
   WORD     altSource;        /* which altitude to use: 0 = from 
                                   telemetry, 
                                 1-4 = channel 1-4 water column,
                                 5=manual entry */
   DWORD    waterColumn;      /* # samples in water column */
   DWORD    triggerPeriod;    /* # samples in trigger period */
   WORD     ch1_signalDivisor;/* chan 1 signal divisor multiplied 
                                   by 100 */
   
   /* telemetry from towfish */
   /*------------------------*/
   float    telemFishDepth;   /* meters */ 
   float    telemFishHeading; /* degrees */
   float    telemFishPitch;   /* degrees */
   float    telemFishRoll;    /* degrees */
   float    telemFishAlt;     /* meters - from Q-MIPS altitude tracker */
   float    telemSbotAlt;     /* meters - from subbottom sensor */
   float    telemSpeedLog;    /* from EM speed log */
   WORD     soundVelocity;    /* compute speed of sound * 30 (one-way) */
   
   /* sonar parameters */
   /*------------------*/
   float    ch1_floatRawRange;
   float    ch1_delayRange;
   WORD     ch1_bandWidth;    /* in kHz */
   WORD     ch1_sampleScheme;
   WORD     ch1_rawRange;     /* (m) range of uncorrected channel */
   BYTE     ch1_initialGain;
   BYTE     ch1_gain;
   float    sampleRate;
   WORD     ch1_correctedRange; /* (m) range of corrected 
                                   channel */
   float    ch2_floatRawRange;
   float    ch2_delayRange;
   WORD     ch2_bandWidth;    /* in kHz */
   WORD     ch2_sampleScheme;
   WORD     ch2_rawRange;     /* (m) range of uncorrected channel */
   BYTE     ch2_initialGain;
   BYTE     ch2_gain;
   WORD     ch2_unused;
   WORD     ch2_signalDivisor;  /* signal divisor * 100 */
   WORD     ch2_correctedRange; /* (m) range of corrected 
                                   channel */
                                /* On any async channel, 
                                   correctedRange holds the async
                                   delay in meters * 100 */
   float    ch3_floatRawRange;
   float    ch3_delayRange;
   WORD     ch3_bandWidth;    /* in kHz */
   WORD     ch3_sampleScheme;
   WORD     ch3_rawRange;     /* (m) range of uncorrected channel */
   BYTE     ch3_initialGain;
   BYTE     ch3_gain;
   WORD     range_to_fish;      /* stored * 10 */
   WORD     ch3_signalDivisor;
   WORD     ch3_correctedRange; /* (m) range of corrected 
                                   channel */
   float    ch4_floatRawRange;
   float    ch4_delayRange;
   WORD     ch4_bandWidth;    /* in kHz */
   WORD     ch4_sampleScheme;
   WORD     ch4_rawRange;     /* (m) range of uncorrected channel */
   BYTE     ch4_initialGain;
   BYTE     ch4_gain;
   WORD     bearing_to_fish; /* stored as degrees * 100 */
   WORD     ch4_signalDivisor;/* signal divisor * 100 */
   WORD     ch4_correctedRange; /* (m) range of corrected 
                                   channel */
   
   /* nav system parameters */
   /*-----------------------*/
   WORD     waterTemperature;   /* water temp * 100 */
   int      currentLineIdentifier;
   int      eventNumber;

   float    auxVal6;
 
   double   shipLatitude;
   double   shipLongitude;
   
   /* #define nav_telemetry.latitude *((double *)
                                   &nav_telemetry.navEasting) */
   float    navEasting;    /* (x) in meters */
   float    navNorthing;   /* (y) in meters */
   
   float    cableTension;     
   
   float    conductivity;     /* water conductivity */
   float    navFishHeading;   /* degrees */
   float    navFishSpeed;     /* knots */
   float    navShipGyro;      /* degrees */
   
   float    auxVal4;
   double   navLongitude;     /* longitude of fish */

   WORD     fishLayback;
   BYTE     navFixHour;
   BYTE     navFixMinute;
   BYTE     navFixSeconds;
   BYTE     onLineFlag;
   WORD     julianDay;        /* when supplied by nav system */
   WORD     cableOut;         /* entered in NAV menu */
   
};
