/*  lanekar4.h
    Lane marker extraction and centerline estimation.
    Keith A. Redmill   17 February 2011  cleanup and generalize
    Keith A. Redmill   Smarteye versions 1997-July 1999
                       Windows/Matrox MIL version August 1999-February 2000
    Derived from earlier code, including 
        vision5.c    23 Oct 1996
        minidemo.c   5 Aug 1997
        winding.c    7 Oct 1997
        try_this.c   20 Aug 1998
        lanekar3.c   14 May 1999
        ln3-wrc.c    23 July 1999 
        vis2.c/l.c   30 Aug 1999/25 Feb 2000
*/


/* Parameters from various versions of code (smarteye):
	    try_this.c   minidemo.c    winding.c    trctape lanekar3  skidpad
LOOKAHEAD   8            12            10            12                10
QUALIFY1    3.2          3.0           2.3           2.3
QUALIFY2    3.0          2.8           2.3           2.3      2.0      2.1
QUALIFY3    12           10            10            10       8        9
BASEDIFF    292/330      300/330       300           355      310      320
XDIFFSCALE  12.6         11.8          11.7          10.0     12.3     12.4
NUMSCANS    19           19            17            26                17
BREADTH     9            9             x             x
LINEWIDTH   3            3             3             3
DEPTH       0            0             1             1
NUMPEAKS    1            1             6             6
FIRSTSCAN   235                                      330      235
MARKSCAN    270                                      345
MATCHEDFILT                            0             0
SPOTFILT                               1             1
QuadFit/LineFit
*/


#define DISTSCALE  (0.014905) //17 feet from front bumper //NOT CURRENTLY USED
#define DEGTORAD (3.1415926/180.0)
#define MARKSCAN  350    /* align this horiz mark to window washer jets */
#define MARKLEFT  50    /* align this vert mark to left washer jet */
#define MARKRIGHT 590    /* align this vert mark to right washer jet */

#define FRAMEWIDTH 640
#define FRAMEHEIGHT 480
#define FIRSTSCAN 280   
#define CENTER (FRAMEWIDTH/2)
#define LOOKAHEAD 10
#define NUMSCANS  30
#define SCANINT 6 
/* The following #defines are for the image extraction routines*/
#define BREADTH   9       /* +/- pixel width of windowed averaging filter */
#define LINEWIDTH 2       /* half the width of line to be detected */
#define DEPTH   1         /* +/- pixel height of windowed averaging filter */
#define NUMPEAKS  6       /* number of peaks per line to report */
#define MINSEP   20       /*number of pixels to ignore around a peak*/
#define XMIN  30          /* ignore the outside edges of the frame */
#define XMAX  (FRAMEWIDTH-XMIN)
#define SCANWIDTH 30
#define SPOTFILT   0          /* 1 to use spot brightness filter */
#define MATCHEDFILT 1         /* 1 to use matched filter */
/* The following #defines are for the qualifier routines*/
#define QUALIFY1 (1.9) //1.9  /*Line 1: number of standard deviations above average*/
#define QUALIFY2 (1.9) //1.9  /*Line n: number of standard deviations above average*/
#define QUALIFY3 (8)  //11  /* std dev < QUALIFY3 => nothing there */
#define DELTAXDIFF  (0.06)
/* The following #defines describe the expected geometry of the road 
    in the image plane and the parameters for finding points on the base
    scanline. */
#define BASEDIFF 530    // approx width of lane in pixels at firstscan
#define BASEDIFFRANGE 60
#define QLEFTC  (CENTER-BASEDIFF/2)
#define QLEFTW  100
#define QRIGHTC (CENTER+BASEDIFF/2)
#define QRIGHTW  100
#define XDIFFSCALE (16.5) //rate in pixels at which lane width shrinks as
                         // we move up the image

#define LEFTFILTER  1       /* assymetric filter dir flag: gains= -9 15 -3 */
#define RIGHTFILTER 2       /* assymetric filter dir flag: gains= -3 15 -9 */

#define LEFTONLY  0   /*use only left lane marker in centerline calc if 1 */
#define RIGHTONLY 0   /*use only right lane marker in centerline calc if 1 */ 


struct LaneIO {
  int center[NUMSCANS], width[NUMSCANS], left[NUMSCANS], right[NUMSCANS];
  double fitcoef1[3], fitcoef2[3], fitcoef3[3];
  float Qualify1, Qualify2;
  int Qualify3;
  float basediff;
  float xdiffscale;
  float xdiff[NUMSCANS];
  int quality;
};

int hessian_ptdtr(float sigma, float hthres, unsigned char *camera, int *xCoords, int *yCoords, SDL_Surface *screen);
float* ransac_2dline(float eta0rl, float eta0ll, float idrl, float idll, int xCoords[], int yCoords[], int l);
void drawmarkers(unsigned char *OVL);
void extractlane(unsigned char *FGBuf, struct LaneIO *);
void localpoints(unsigned char *FGBuf, int scanline, int center, int width, 
		 int leftright, 
		 int *scanloc, int *scanval, int *scanavg, int *scanstd);
void imagepoints(unsigned char *FGBuf, int scanline,int numpeaks,
		 int *scanloc,int *scanval,int *scanavg,int *scanstd);
void qualifypoints(int *point,int numpeaks,int *scanloc,int *scanval, 
		   int scanavg, int scanstd, float qual,int center,int width);
void drawpoint1(unsigned char *DISPBuf, int point, int scanline);
void drawpoint2(unsigned char *DISPBuf, int point, int scanline);
void drawline(unsigned char *DISPBuf, int x1, int y1, int x2, int y2);
void outputresults(unsigned char *DISPBuf, struct LaneIO, int seqnum);
void enhancecontrast(unsigned char *FGBuf, int firstpass);
void quadfit(int size, int *x, int *y, double *coef);
void linefit(int size, int *x, int *y, double *coef);
int adjustparams(struct LaneIO *, char);
void initLaneIO(struct LaneIO *);
