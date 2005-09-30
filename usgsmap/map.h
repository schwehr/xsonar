/* Created by Kurt Schwehr */
void ddtodegmin(double decdeg, int *deg, double *min);
void ddtodegminsec(double decdeg, int *deg, int *min, double *sec);
void degmintodd(double *decdeg, int deg, double min);
void djdtohm(double decjd, int *hour, int *min, double *sec);
void itoa (int n, char s[]);
void reverse(char s[]);
void mergeo (float *phi, float *lam, float clam, float cphi, float scale, float x, float y);

/* stardate */
double  stardate( short *ymdhms );
void invstardate( double sd, short ymdhms[] );
double sd_diff( short ymdhms [] );
short invsd_diff( double sd, short ymdhms[] );
double hms2sd( short ymdhms[] );
short sd2hms( double fracday, short ymdhms[] );
long julian ( short ymdhms[] );
short cdate(long jd, short ymdhms[]);

float geoazi (float phi1, float lam1, float phi2, float lam2);
void nec2dec(float *a,int n);

void geoutm (double phi, double lam, double clam, double *x, double *y, int ellipsoid);
void utmgeo (double *phi, double *lam, double clam, double x, double y, char hem, int ellipsoid);
double geodis (double phi1, double lam1, double phi2, double lam2, int ellipsoid);
     /* for geodis & geoutm ellipsoid */
#define WGS84        1
#define CLARKE1866   2

double hmtodjd (int jd, int hour, int min, double sec);
int spline (double *inputt, double *inputy, int inputn, double *outputt, double *outputy, int outputn);
void zspl3 (double *t, double *y, double *z, int n);
double spl3 (double x, double *t, double *y, double *z, int n);


