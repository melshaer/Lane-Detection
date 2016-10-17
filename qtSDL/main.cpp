#include <qapplication.h>

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "main.h"
#include "mainwin.h"

int main( int argc, char **argv )
{
  int shmFD;

#ifndef BENCHTEST
  shmFD= shm_open(VEHSHM_MEMNAME, O_RDONLY, 0);
  if (shmFD == -1) {
     fprintf(stderr, "SHM Open failed: %s\n", strerror(errno));
     exit(1);
  }
  vehshm= (VEHSHM *) mmap(0, sizeof(VEHSHM), PROT_READ, MAP_SHARED,
       shmFD, 0);
  if (vehshm == (void *) -1) {
      fprintf(stderr, "mmap failed: %s\n", strerror(errno));
      exit(1);
  }
#else
  vehshm= (VEHSHM*) calloc(1, sizeof(VEHSHM));
#endif

    QApplication a(argc,argv);
    MainWindow window;
    window.show();
    return a.exec();
}
