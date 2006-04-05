/* 
   Q-MIPS data format as used by Isis
   written as C-language structures.
   Current 13 December 1994
*/


/* how will the nav data be saved? (default) */
#define METERS  0
#define FEET    1
#define YARDS   2
#define DEGREES 3
#define NAV_UNITS METERS

/* possible downsample schemes */
#define MEAN  1L
#define VMAX  2L
#define VMIN  3L
#define RMS   4L
#define NONE  5L

#define       FORMAT1   1    /* old format (smips) */
#define       FORMAT2   50   /* new format (qmips) */

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

typedef struct header_format {
/****************************************************************************/

/* header record */
/*****************/

   BYTE     fileFormat;       /* equal to 50 for Q-MIPS files */
   BYTE     systemType;       /* equal to 202 for Isis-recorded files */
   BYTE     softwareRev[6];   /* String "Isis" or ASCII software rev  */
   BYTE     spare1[18];       /* not used. */
   
   /* Data storage parameters */
   /*-------------------------*/
   float    sampleRate;              /* Not valid for Isis.  set to 20. */
   WORD     numImageryChannels;      /* 0..4 for Isis-recorded files */
   WORD     bitsPerPixel;            /* 8 or 16 bits per pixel */
   WORD     pixelsPerChannelPerPing; /* 0..65535 samples per chan saved */
   float    speedOfSoundInWater;     /* Ususlly 750 m/s, 2-way travel time */
   float    noLongerUsed;            /* Not used in Isis. */
   WORD     asyncChannelNumber;      /* Not used in Isis. */
   WORD     numSonarChannels;        /* 0..4 Number of recorded channels */
   WORD     ch1_processingAvailable; /* Set to 1 if Chan 1 logged */
   WORD     ch2_processingAvailable; /* Set to 1 if Chan 2 logged */
   WORD     ch3_processingAvailable; /* Set to 1 if Chan 3 logged */
   WORD     ch4_processingAvailable; /* Set to 1 if Chan 4 logged */
   
   /* nav system parameters */
   /*-----------------------*/
   WORD     timeDelay;                /* Not used */
   BYTE     navSystemName[100];       /* Not used */
   BYTE     projectionType[12];       /* Not used */
   BYTE     spheriodType[12];         /* Not used */
   WORD     zone;                     /* Not used */
   float    originLat;                /* Not used */
   float    originLong;               /* Not used */
   float    offsetLat;                /* Not used */
   float    offsetLong;               /* Not used */
   WORD     navUnits;  /* 0 = meters, 1 = feet, 2 = yards, 3 = degrees */
   
   
   /* site parameters */
   /*-----------------*/
   WORD     diveNumber;               /* Not used */
   WORD     blockNumber;              /* Not used */
   WORD     trackNumber;              /* Not used */
   WORD     runNumber;                /* Not used */
   BYTE     spare4[100];              /* Not used */
   
   /* annotation */
   /*------------*/
   BYTE     operatorAnnotation[100];  /* Not used */
   BYTE     sonarName[40];            /* "Isis Analog Server" */
   
   
   /* sonar parameters */
   /*------------------*/
   WORD     triggerDirection;         /* Not used in Isis */
   DWORD    triggerMagnitude;         /* Not used in Isis */
   float    triggerWidth;             /* Not used in Isis */
   
   WORD     ch1_frequency;            /* Defaults to 100 */
   WORD     ch2_frequency;            /* Defaults to 100 */
   WORD     ch3_frequency;            /* Defaults to 100 */
   WORD     ch4_frequency;            /* Defaults to 100 */
   
   float    ch1_horizBeamAngle;       /* Not yet used in Isis */
   float    ch2_horizBeamAngle;       /* Not yet used in Isis */
   float    ch3_horizBeamAngle;       /* Not yet used in Isis */
   float    ch4_horizBeamAngle;       /* Not yet used in Isis */
   
   /* name of this file */
   /*-------------------*/
   char     thisFileName[45];         /* Original name of logged file */
   char     reserved2;                /* reserved */
   
   /* which channels are half-wave rectified */
   /*----------------------------------------*/
   WORD     ch1_halfWaveRectify;      /* Set to 0 */
   WORD     ch2_halfWaveRectify;      /* Set to 0 */
   WORD     ch3_halfWaveRectify;      /* Set to 0 */
   WORD     ch4_halfWaveRectify;      /* Set to 0 */


   /* Sonar Transducer Tilt Angles (off horizontal) */
   /*-----------------------------------------------*/
   float    ch1_tiltAngle;            /* defaults to 30 */
   float    ch2_tiltAngle;            /* defaults to 30 */
   float    ch3_tiltAngle;            /* defaults to 30 */
   float    ch4_tiltAngle;            /* defaults to 30 */

   /* Sonar Transducer beamWidth at 3dB points */
   /*------------------------------------------*/
   float    ch1_beamWidth_3dB;        /* defaults to 50 */
   float    ch2_beamWidth_3dB;        /* defaults to 50 */
   float    ch3_beamWidth_3dB;        /* defaults to 50 */
   float    ch4_beamWidth_3dB;        /* defaults to 50 */

   float    ch1_realSampleRate;       /* Unused in Isis */
   float    ch2_realSampleRate;       /* Unused in Isis */
   float    ch3_realSampleRate;       /* Unused in Isis */
   float    ch4_realSampleRate;       /* Unused in Isis */

   /* left over */
   /*-----------*/
   BYTE     spare5[438];              /* reserved */
                                          
   
} QMIPSHEADER;


/********************************************************************
   Note: in the comments below, the bracketed characters represent
   the Navigation Template characters that can be used to fill the 
   given fields.  
*********************************************************************/

typedef struct sonar_struct2 {
/*****************************************************************/
/*  Every ping ends with this 256-byte structure.  Note that not 
    all  fields  will  be used during every  run.   If  not  all 
    channels are being used, the remaining channels will contain 
    0's for all fields. 
*/

   /* time and date stamp */
   /*---------------------*/
   BYTE     day;                 
   BYTE     month;         /* will be time and date of nav system */
   BYTE     year;          /* user specifies 'AB' in nav template */
   BYTE     hour;
   BYTE     minute;
   BYTE     seconds;
   WORD     thousandsSeconds; /* 1000th of seconds */
   
   float    magX;          /* [e] X-axis magnetometer data */
   float    magY;          /* [w] Y-axis magnetometer data */
   float    magZ;          /* [z] Z-axis magnetometer data */

   DWORD    pingNumber;    /* counts from 0 at start of file */

   WORD     asyncByteOffset; /* not used in Isis */

   WORD     shipSpeed;       /* [v] speed of ship (knots * 100) */

   float    auxVal1;         /* [1] Aux value displayed in Sensors box */
   float    auxVal2;         /* [2] Aux value displayed in Sensors box */
   float    auxVal3;         /* [3] Aux value displayed in Sensors box */
   float    auxVal5;         /* [5] Aux value displayed in Sensors box */

   /* processing parameters */
   /*-----------------------*/
   float    auxAltitude;     /* [a] auxiliary altitude */

   WORD     triggerChannel;  /* not used in Isis */
   WORD     altSource;       /* not used in Isis */
   DWORD    waterColumn;     /* not used in Isis */
   DWORD    triggerPeriod;   /* not used in Isis */
   WORD     ch1_signalDivisor; /* set to 1 */
   
   /* telemetry from towfish */
   /*------------------------*/
   float    telemFishDepth;   /* [0] depth (sea surface to sonar) in meters */
   float    telemFishHeading; /* [h] heading in degrees */
   float    telemFishPitch;   /* [8] pitch in degrees (positive=nose up) */
   float    telemFishRoll;    /* [9] roll in degrees (positive=roll to stbd) */
   float    telemFishAlt;     /* [7] Altitude tracked by Isis - in meters */
   float    telemSbotAlt;     /* unused in Isis (reserved) */
   float    telemSpeedLog;    /* [s] speed log sensor on towfish - knots */
   WORD     soundVelocity;    /* [{v}] sound velocity in m/s, can also be computed, */
                              /* stored times 30 */
   
   /* sonar parameters */
   /*------------------*/
   float    ch1_floatRawRange; /* Chan 1 slant range in meters */
   float    ch1_delayRange;    /* Not used in Isis - reserved */
   WORD     ch1_bandWidth;     /* Not used in Isis */
   WORD     ch1_sampleScheme;  /* Set to 2 */
   WORD     ch1_rawRange;      /* integer slant range Chan 1 */
   BYTE     ch1_initialGain;   /* [q] Chan 1 initial gain code */
   BYTE     ch1_gain;          /* [g] Chan 1 gain code */
   WORD     reservedForFloatSampleRate; /* Not used in Isis */
   WORD     sampleRate;        /* Not used in Isis - reserved */
   WORD     ch1_correctedRange;/* Not used in Isis */
                               /* compute ground range as 
                                  sqrt(ch1_floatRawRange^2 - telemFishAlt^2)
                               */
   float    ch2_floatRawRange; /* Chan 2 slant range in meters */
   float    waterTemperature;  /* [{w}] Water Temperature in C, can be computed from [b] */
   WORD     ch2_bandWidth;     /* Not used in Isis */
   WORD     ch2_sampleScheme;  /* Set to 2 */
   WORD     ch2_rawRange;      /* integer slant range Chan 2 */
   BYTE     ch2_initialGain;   /* [r] Chan 2 initial gain code */
   BYTE     ch2_gain;          /* [j] Chan 2 gain code */
   WORD     oceanTide;         /* [{t}] Ocean tide in meters times 100 */
   WORD     ch2_signalDivisor; /* Set to 1 */
   WORD     ch2_correctedRange;/* Not used in Isis */
                               /* compute ground range as 
                                  sqrt(ch2_floatRawRange^2 - telemFishAlt^2)
                               */
   float    ch3_floatRawRange; /* Chan 3 slant range in meters */
   float    pressure;          /* [{p}] water pressure in psia, can be computed from [0] */
   WORD     ch3_bandWidth;     /* Not used in Isis */
   WORD     ch3_sampleScheme;  /* Set to 2 */
   WORD     ch3_rawRange;      /* integer slant range Chan 3 */
   BYTE     ch3_initialGain;   /* [t] Chan 3 initial gain code */
   BYTE     ch3_gain;          /* [k] Chan 3 gain code */
   WORD     range_to_fish;     /* [?] slant range to fish times 10 */
   WORD     ch3_signalDivisor; /* Set to 1 */
   WORD     ch3_correctedRange;/* Not used in Isis */
                               /* compute ground range as 
                                  sqrt(ch3_floatRawRange^2 - telemFishAlt^2)
                               */
   float    ch4_floatRawRange; /* Chan 4 slant range in meters */
   float    ch4_delayRange;    /* Not used in Isis - reserved */
   WORD     ch4_bandWidth;     /* Not used in Isis */
   WORD     ch4_sampleScheme;  /* Set to 2 */
   WORD     ch4_rawRange;      /* integer slant range Chan 4 */
   BYTE     ch4_initialGain;   /* [u] Chan 4 initial gain code */
   BYTE     ch4_gain;          /* [n] Chan 4 gain code */
   WORD     bearing_to_fish;   /* [>] bearing to fish in degrees * 100 */
   WORD     ch4_signalDivisor; /* set to 1 */
   WORD     ch4_correctedRange;/* Not used in Isis */
                               /* compute ground range as 
                                  sqrt(ch4_floatRawRange^2 - telemFishAlt^2)
                               */
   
   /* nav system parameters */
   /*-----------------------*/
   WORD     turbidity;             /* [|] turbidity sensor (0 to +5 volts) stored times 10000 */
   int      currentLineIdentifier; /* [i] Current line ID from serial nav */
   int      eventNumber;           /* [O] last unique event number */

   float    auxVal6;           /* [6] Aux value displayed in Sensors box */
 
   double   shipLatitude;      /* [y] Ship latitude */
   double   shipLongitude;     /* [x] Ship longitude */
   
   float    navEasting;        /* [E] fish easting */
   float    navNorthing;       /* [N] fish northing */

                             /* When navUnits in the header equals 3, the
                                fish nav is logged in degrees.  In this
                                case, the 8-bytes occupied by navEasting
                                and navNorthing are recycled and used as
                                a single double-float (8-byte) value called
                                navLatitude which is the fish latitude.
                                To make make navUnits 3, put a '$' in the
                                nav template.  This forces the navigation
                                to be accepted as lat/lon as opposed to
                                meters.
                                [N] with '$' in nav template
                              */
   
   float    cableTension;     /* [P] cable tension from serial port */
   
   float    conductivity;     /* [{c}] Conductivity in S/m can be computed from [Q] */
   float    navFishHeading;   /* Unused by Isis - see telemFishHeading */
   float    navFishSpeed;     /* [V] Speed of towfish in knots */
   float    navShipGyro;      /* [G] Gyro of ship */
   
   float    auxVal4;          /* [4] Aux value displayed in Sensors box */
   double   navLongitude;     /* [E] with '$' in nav lemplate; longitude 
                                 of fish (header.navUnits==3) */

   WORD     fishLayback;      /* [l] horizontal distance from ship to fish */
   BYTE     navFixHour;       /* [H] Hour of last nav update */
   BYTE     navFixMinute;     /* [I] Minute of last nav update */
   BYTE     navFixSeconds;    /* [S] Seconds of last nav update */
   BYTE     onLineFlag;       /* unused in Isis */
   WORD     julianDay;        /* Number of days since January 1 */
   WORD     cableOut;         /* [o] amount of cable payed out in meters */
   
} QMIPSFOOTER;
