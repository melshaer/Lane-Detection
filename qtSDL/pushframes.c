#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "main.h"

int getpgm(unsigned char *imagebuf, char *prefix, int framenum) {
  int i;
  FILE *fp;
  char line[100], filename[100];

  sprintf(filename, "%s%05d.pgm", prefix, framenum);
  printf("%s\n", filename);
  if ((fp=fopen(filename,"r"))==NULL) {
    printf("Can't open %s\n", filename);
    return(1);
  }
  for (i=0; i<4; i++) {
    fgets(line, 100, fp);
  }
  fread(imagebuf, 640*480, 1, fp);
  fclose(fp);
  return(0);
}

double getmidnight() {
  double time1;
  struct tm *tmidnight;
  time_t ttmidnight;

  ttmidnight= time(NULL);
  tmidnight= localtime(&ttmidnight);
  tmidnight->tm_sec=0; tmidnight->tm_min=0; tmidnight->tm_hour=0;
  return(mktime(tmidnight));
}

int main(int argc, char **argv) {
  int i,j,k, shmFD;
  int start, end, delayus;
  IMAGESHM *imgshm;
  double timemidnight, time1, time2;
  struct timespec timep;

  if (argc!=5) {
    printf("pushframes suffix startindex endindex timedelay(uSec)\n");
    return(1);
  }

  start= atoi(*(argv+2));
  end= atoi(*(argv+3));
  delayus= atoi(*(argv+4));

  timemidnight= getmidnight();

  shmFD= shm_open(IMAGESHM_NAME, O_RDWR|O_CREAT, 000777);
  if (shmFD == -1) {
     fprintf(stderr, "SHM Open failed: %s\n", strerror(errno));
     exit(1);
  }
  if (ftruncate(shmFD, sizeof(IMAGESHM)) == -1) {
     fprintf(stderr, "ftruncate: %s\n", strerror(errno));
     exit(1);
  }
  imgshm= (IMAGESHM *) mmap(0, sizeof(IMAGESHM), PROT_READ|PROT_WRITE,
                         MAP_SHARED, shmFD,0);
  if (imgshm == (void *) -1) {
      fprintf(stderr, "mmap failed: %s\n", strerror(errno));
      exit(1);
  }
  imgshm->camthreadstop= 0;
  imgshm->newframe=0;
  imgshm->framecnt=0;
  imgshm->frametime= 0.0;

  for (i=start; i<=end; i++) {
     getpgm((unsigned char *)&(imgshm->image), *(argv+1), i);
     imgshm->newframe=1;
     imgshm->framecnt++;
     usleep(delayus);
  }

#if 0
  do {
    time2=time1;
    clock_gettime(CLOCK_REALTIME, &timep);
    time1= (timep.tv_sec-timemidnight) +
                (double) timep.tv_nsec/(double)1000000000;
    imgshm->frametime= time1-time2;
  }  
#endif

}
