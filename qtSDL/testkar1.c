 // testkar1.c
// simple test/example of matched filter brightness plus a very simple RANSAC

int testkarBrightRansac(unsigned char *camera, SDL_Surface *screen )  {
  int i,j,k,jj;
  int scanavg, scanstd, scanloc[6], scanval[6];
  int xpts[20000], ypts[20000], npts;
  int k1, k2, kc, best;
  double slope, offset, bslope, boffset;
  double dd, dd1, dd2, dd3, dd4;
  
  npts= 0;
  for (i=0; i<NUMSCANS; i++) {
     imagepoints(camera, FIRSTSCAN-i*SCANINT, 6, scanloc, scanval,
    	&scanavg, &scanstd);
    for (j=0; j<6; j++) {
      Rect(*(scanloc+j)-(8-j-2), FIRSTSCAN-i*SCANINT-2, 12-2*j+1, 5,
         0, 255, 0, screen);
    }
    
    for (j=0; j<6;j++) {
      xpts[npts]= *(scanloc+j);
      ypts[npts]= FIRSTSCAN-i*SCANINT;
      npts++;
    }
  }
  printf("\n");
  for (jj=0; jj<3; jj++) {
    best= -1;
    for (i=0; i<20000; i++) {
      k1= npts*(double)rand()/(double)RAND_MAX + 0.5;
      k2= npts*(double)rand()/(double)RAND_MAX + 0.5;
      if (k1==k2) continue;
      if (xpts[k1]<0) continue;
      if (xpts[k2]<0) continue;
      kc=0;
      slope= (ypts[k1]-ypts[k2])/(xpts[k1]-xpts[k2]+0.001);
      offset= ypts[k1]-slope*xpts[k1];
      for (j=0; j<npts; j++) {
	if (xpts[j]<0) continue;
	dd1= xpts[j]+slope*ypts[j]-slope*offset;
	dd2= slope*slope+1.0;
	dd=sqrt((dd1/dd2-xpts[j])*(dd1/dd2-xpts[j])+
		(slope*dd1/dd2+offset-ypts[j])*(slope*dd1/dd2+offset-ypts[j]));
	if (dd<4.0) kc++;
      }
      if (kc>best) {bslope=slope; boffset=offset;best=kc;}
      if (best>17) break;
    }
    printf("Slope %f Offset %f Score %d\n", bslope, boffset, best);
    for (j=0; j<npts; j++) {
      if (xpts[j]<0) continue;
      dd1= xpts[j]+bslope*ypts[j]-bslope*boffset;
      dd2= bslope*bslope+1.0;
      dd=sqrt((dd1/dd2-xpts[j])*(dd1/dd2-xpts[j])+
	      (bslope*dd1/dd2+boffset-ypts[j])*(bslope*dd1/dd2+boffset-ypts[j]));
      if (dd<4.0) {
	//printf("%f %d %d\n", dd, xpts[j], ypts[j]);
	//	Rect(xpts[j]-1, ypts[j]-1, 3, 3, 255, 0, 0, screen);
	xpts[j]= -99; ypts[j]= -99;
      }
    }
    if (best>1 && fabs(bslope)>0.46) {
      // Line((FIRSTSCAN-boffset)/bslope, FIRSTSCAN,
      // (100-boffset)/bslope, 100, 255,0,0,screen);
#if 1
      // draw line on ground plane in upper left corner of display
      double xpix, ypix,xinch1, yinch1, xinch2, yinch2;
      xpix= (FIRSTSCAN-boffset)/bslope-320.0; ypix=FIRSTSCAN;
      pixeltoinch(xpix, ypix, &xinch1, &yinch1);
      printf("%f %f INCH %f %f\n", xpix, ypix, xinch1, yinch1);
      xpix= (100-boffset)/bslope-320.0; ypix= 100.0;
      pixeltoinch(xpix, ypix, &xinch2, &yinch2);
      printf("%f %f INCH %f %f\n", xpix, ypix, xinch2, yinch2);
      Line(0.5*(xinch1+120.0), 0.5*(2000.0-yinch1)/10.0,
	   0.5*(xinch2+120.0), 0.5*(2000.0-yinch2)/10.0, 255,0,0,screen);
#endif
    }
  }
  return(0);
}


int testkarBrightRansac_ME(unsigned char *camera, SDL_Surface *screen)  {
  
  int *xPts = new int[300]();
  int *yPts = new int[300]();

  int l = hessian_ptdtr(0.5, 0.2, camera, xPts, yPts, screen);

  //Display Hessian points on screen
#if 0
  for(int i = 0; i < l; i++)
    {
      int x = *(xPts + i);
      int y = *(yPts + i);
      set_point(screen, x, y, 255, 0, 0);
    }
#endif

  float *lines2D = new float[4](); //lines2D has line parameters
  lines2D =  ransac_2dline(0.2, 0.2, 0.5, 0.5, xPts, yPts, l);

  return(0);
}




