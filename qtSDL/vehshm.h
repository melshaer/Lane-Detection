// vehshm.h   Ford Fusion 2014  DURA
//    Shared memory structure
//    Keith Redmill  Sept 2014

#ifndef VEHSHM_H
#define VEHSHM_H

#define VEHSHM_MEMNAME "vehshm"

typedef unsigned char uchar;

// NOVATEL GPS
typedef struct {
  double timestamp;   //cpu timestamp of last packet received
  uchar idletime;
  uchar timestatus;
  uint week;
  double seconds;
  uint rcvrstatus;
//    BESTPOS 42 
  double postimestamp, posseconds;
// postype:  0=none, 16=single, 17=psrdiff, 18=waas, 20=omnistar, 
//           32-47=floatAmb, 48=L1int,49=WideInt,50=NarrowInt, 64=omnistarHP
// solnstatus: 0=computed, 1=ins obs, 4=cov_trace_large
  uchar solnstatus, postype;
  float diffage, solnage;
  uchar numobs, numgpsL1, numL1, numL2;
  double lat, lng, hgt;
  float undulation;
  float latstd, lngstd, hgtstd;
//    BESTVEL  99
  double veltimestamp, velseconds;
  uchar veltype;
  float latency;
  double horzspeed, trkgnd, vertspeed;
//    PSRDOP 174
  double doptimestamp, dopseconds;
  float gdop, pdop, hdop, htdop, tdop;
  float cutoff;
//    RXSTATUSEVENT 94	
  double eventtimestamp, eventseconds;
  uchar eventword, eventtype;
  uint eventbits;
//    TIME  101
  double utctimestamp, utcgpsseconds;
  uchar utcstatus;
  double utcoffset;
  uint utcyear;
  uchar utcmonth, utcday, utchour, utcminute;
  float utcseconds;
//    OMNISTAT 510
  double omnistatustimestamp, omnistatusseconds;
  float omnisnr, omnilocktime;
  ushort omnitracking, omnivbsstatus, omnihpstatus1, omnihpstatus2;
} NOVATEL_DATA;

typedef struct {
// Vehicle CAN Bus Data
   double cants;   // last update from CAN bus
   double yawrate;    // deg/s  
   double lataccel;   // m/ss   
   double accelpos; // 0-100?  0x42d:3
   double brakepressure;  //   0x7d:0-1
   double steeringwheel;  // deg?  0x76:0-1
   double speed1; // m/s  0x14b:2-3
   double enginerpm;  //   0x204:6-7
   double transrpm;  // 
   double odometer;  //  
   double flWheel, frWheel, rlWheel, rrWheel; // kph 0x217 
   double airtemp;  // 
   int headlights;   //   
   int brakepdl;   // 0x165:0,4   0x20=depressed
   int accelpdl;  // 
   int prndl;  // P=0, R=2, N=4, D5=8, D4=10, D3=20, D2=40, D1=80  0x230:1
   int gear1, gear2;
   int wiper;  //   
   int turnsignal;  //  0x83:0  0x10=left, 0x20=right, 0x40= stickfwd/back
   int door1, door2;  //
 
// high-prec external imu
   double imuyaw, imupitch, imuroll;
   double imuaccelX, imuaccelY, imuaccelZ;
   double imurollRate, imupitchRate, imuyawRate; //yaw around Z, roll around Y
   double imuts;

   NOVATEL_DATA gps1;
} VEHSHM;

#endif
