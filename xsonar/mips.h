
/*	Structure for MIPS data record     */


typedef struct {
        short passnum;
        short linenum;
        short course;
        short prp;
        short day;
        short month;
        short year;
        short hour;
        float minutes;
        float latitude;
        float longitude;
        short cor_alt;
        short uncor_alt;
} MipsTrailer;
