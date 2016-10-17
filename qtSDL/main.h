// main.h
//

#include "vehshm.h"
#define IMAGESHM_NAME "imageshm"
#define IMAGEBUFSIZE (640*480)
typedef struct {
  int newframe;
  unsigned char image[IMAGEBUFSIZE];
  int framecnt;
  double frametime;
  int camthreadstop;
} IMAGESHM;


#ifdef THISISMAIN
int framecount;
double framerate;
int sdl_thread_die;
double Gtimemidnight;
int Goffsetpixels;
int Gacqframe;
VEHSHM *vehshm;

#else
extern int framecount;
extern double framerate;
extern int sdl_thread_die;
extern double Gtimemidnight;
extern int Goffsetpixels;
extern VEHSHM *vehshm;
extern int Gacqframe;
#endif

