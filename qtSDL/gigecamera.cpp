#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "main.h"

#include "FlyCapture2.h"

using namespace FlyCapture2;

GigECamera cam;

double getmidnight() {
  double time1;
  struct tm *tmidnight;
  time_t ttmidnight;

  ttmidnight= time(NULL);
  tmidnight= localtime(&ttmidnight);
  tmidnight->tm_sec=0; tmidnight->tm_min=0; tmidnight->tm_hour=0;
  return(mktime(tmidnight));
}

void PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf( 
        version, 
        "FlyCapture2 library version: %d.%d.%d.%d\n", 
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    printf( version );

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    printf( timeStamp );
}

void PrintCameraInfo( CameraInfo* pCamInfo )
{
    char macAddress[64];
    sprintf( 
        macAddress, 
        "%02X:%02X:%02X:%02X:%02X:%02X", 
        pCamInfo->macAddress.octets[0],
        pCamInfo->macAddress.octets[1],
        pCamInfo->macAddress.octets[2],
        pCamInfo->macAddress.octets[3],
        pCamInfo->macAddress.octets[4],
        pCamInfo->macAddress.octets[5]);

    char ipAddress[32];
    sprintf( 
        ipAddress, 
        "%u.%u.%u.%u", 
        pCamInfo->ipAddress.octets[0],
        pCamInfo->ipAddress.octets[1],
        pCamInfo->ipAddress.octets[2],
        pCamInfo->ipAddress.octets[3]);

    char subnetMask[32];
    sprintf( 
        subnetMask, 
        "%u.%u.%u.%u", 
        pCamInfo->subnetMask.octets[0],
        pCamInfo->subnetMask.octets[1],
        pCamInfo->subnetMask.octets[2],
        pCamInfo->subnetMask.octets[3]);

    char defaultGateway[32];
    sprintf( 
        defaultGateway, 
        "%u.%u.%u.%u", 
        pCamInfo->defaultGateway.octets[0],
        pCamInfo->defaultGateway.octets[1],
        pCamInfo->defaultGateway.octets[2],
        pCamInfo->defaultGateway.octets[3]);

    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n"
        "GigE version - %u.%u\n"
        "User defined name - %s\n"
        "XML URL 1 - %s\n"
        "XML URL 2 - %s\n"
        "MAC address - %s\n"
        "IP address - %s\n"
        "Subnet mask - %s\n"
        "Default gateway - %s\n\n",
        pCamInfo->serialNumber,
        pCamInfo->modelName,
        pCamInfo->vendorName,
        pCamInfo->sensorInfo,
        pCamInfo->sensorResolution,
        pCamInfo->firmwareVersion,
        pCamInfo->firmwareBuildTime,
        pCamInfo->gigEMajorVersion,
        pCamInfo->gigEMinorVersion,
        pCamInfo->userDefinedName,
        pCamInfo->xmlURL1,
        pCamInfo->xmlURL2,
        macAddress,
        ipAddress,
        subnetMask,
        defaultGateway );
}

void PrintStreamChannelInfo( GigEStreamChannel* pStreamChannel )
{
    char ipAddress[32];
    sprintf( 
        ipAddress, 
        "%u.%u.%u.%u", 
        pStreamChannel->destinationIpAddress.octets[0],
        pStreamChannel->destinationIpAddress.octets[1],
        pStreamChannel->destinationIpAddress.octets[2],
        pStreamChannel->destinationIpAddress.octets[3]);

    printf(
        "Network interface: %u\n"
        "Host post: %u\n"
        "Do not fragment bit: %s\n"
        "Packet size: %u\n"
        "Inter packet delay: %u\n"
        "Destination IP address: %s\n"
        "Source port (on camera): %u\n\n",
        pStreamChannel->networkInterfaceIndex,
        pStreamChannel->hostPost,
        pStreamChannel->doNotFragment == true ? "Enabled" : "Disabled",
        pStreamChannel->packetSize,
        pStreamChannel->interPacketDelay,
        ipAddress,
        pStreamChannel->sourcePort );
}

void PrintError( Error error )
{
    error.PrintErrorTrace();
}

int setupcamera(  ) {
  Error error;
  BusManager busMgr;
  CameraInfo camInfo[10];
  unsigned int numCamInfo= 10, numCameras;
  IPAddress ipaddrcam;
  PGRGuid guid;
  InterfaceType interfaceType;
//  GigECamera cam;
  unsigned int numStreamChannels= 0;
  GigEStreamChannel streamChannel;
  GigEImageSettingsInfo imageSettingsInfo;
  GigEImageSettings imageSettings;

  PrintBuildInfo();
  error= BusManager::DiscoverGigECameras( camInfo, &numCamInfo );
  if (error != PGRERROR_OK) {
     PrintError(error);
     return(-1);
  }
  printf( "Number of cameras discovered: %u\n", numCamInfo );
  //PrintCameraInfo(&camInfo[0]);
  error = busMgr.GetNumOfCameras(&numCameras);
  if (error != PGRERROR_OK) {
     PrintError(error);
     return(-1);
  }
  printf( "Number of cameras enumerated: %u\n", numCameras );

  ipaddrcam.octets[0]= 192; ipaddrcam.octets[1]= 168; 
  ipaddrcam.octets[2]=1; ipaddrcam.octets[3]=81;
  error= busMgr.GetCameraFromIPAddress(ipaddrcam, &guid);
  if (error != PGRERROR_OK) {
     PrintError(error);
     return(-1);
  }
  error = busMgr.GetInterfaceTypeFromGuid( &guid, &interfaceType );
  if (error != PGRERROR_OK) {
     PrintError(error);
     return(-1);
  }
  if (interfaceType != INTERFACE_GIGE) {
    printf("Oops- not a GiGE camera...\n");
    return(-1);
  }

  printf( "Connecting to camera...\n" );

  error = cam.Connect(&guid);
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }
  error = cam.GetCameraInfo(&camInfo[0]);
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }
  PrintCameraInfo(&camInfo[0]);        

  error = cam.GetNumStreamChannels( &numStreamChannels );
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }
  for (unsigned int i=0; i < numStreamChannels; i++) {
      error = cam.GetGigEStreamChannelInfo( i, &streamChannel );
      if (error != PGRERROR_OK) {
            PrintError( error );
            return -1;
      }
      printf( "\nPrinting stream channel information for channel %u:\n", i );
      PrintStreamChannelInfo( &streamChannel );
  }    

  printf( "Querying GigE image setting information...\n" );
  error = cam.GetGigEImageSettingsInfo( &imageSettingsInfo );
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }

  error= cam.SetGigEImageBinningSettings(2,2);
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }

  imageSettings.offsetX = 0;
  imageSettings.offsetY = 0;
//  imageSettings.height = imageSettingsInfo.maxHeight;
//  imageSettings.width = imageSettingsInfo.maxWidth;
  imageSettings.width=640;
  imageSettings.height=480;
  imageSettings.pixelFormat = PIXEL_FORMAT_MONO8;
  printf( "Setting GigE image settings...\n" );
  error = cam.SetGigEImageSettings( &imageSettings );
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }
#if 0
  error=cam.SetVideoModeAndFrameRate(VIDEOMODE_640x480Y8, FRAMERATE_30);
  if (error!=PGRERROR_OK) {
     PrintError(error);
     return -1;
  }
#endif
  return(0);
}

int main() {
  int i,j,k, shmFD;
  unsigned int ui;
  Error error;
  Image rawImage; 
  unsigned char *pData;
  IMAGESHM *imgshm;
  double timemidnight, time1, time2;
  struct timespec timep;

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

  k= setupcamera();
  if (k!=0) {
     printf("Couldn't initialize camera.\n");
     return -1;
  }

  printf( "Starting image capture...\n" );
  error = cam.StartCapture();
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }
  do {
    time2=time1;
    clock_gettime(CLOCK_REALTIME, &timep);
    time1= (timep.tv_sec-timemidnight) +
                (double) timep.tv_nsec/(double)1000000000;
    imgshm->frametime= time1-time2;
//printf("%f\n", time1-time2);
#if 0
error=cam.ReadRegister(0x73c, &ui);
if (error != PGRERROR_OK) {
  PrintError(error);
}
printf("FrameRate 73C: %x %d\n", ui,ui);
error=cam.WriteRegister(0x73c, 0x3c012c);
if (error != PGRERROR_OK) {
  PrintError(error);
}

error=cam.ReadRegister(0x83c, &ui);
if (error != PGRERROR_OK) {
  PrintError(error);
}
printf("FrameRate 83C: %x %d\n", ui,ui);
error=cam.ReadRegister(0x600, &ui);
if (error != PGRERROR_OK) {
  PrintError(error);
}
printf("Current_FrameRate 600: %x %d\n", ui,ui);
#endif
    error = cam.RetrieveBuffer( &rawImage );
    if (error != PGRERROR_OK) {
        PrintError( error );
        continue;
    }
    pData= rawImage.GetData();
    unsigned int rows, cols, stride, pDataSize;
    PixelFormat pixFormat;
    pDataSize= rawImage.GetDataSize();
#if 0
    rawImage.GetDimensions( &rows, &cols, &stride, &pixFormat );
    printf( "Grabbed image %d: %d %d %d %d 0x%x\n", imgshm->framecnt,pDataSize,
              cols, rows, stride, pixFormat );
#endif
    for (i=0; i<pDataSize; i++)
       imgshm->image[i]= pData[i];  
    imgshm->framecnt++;
    imgshm->newframe= 1;
  } while (!imgshm->camthreadstop);

  printf( "Stopping capture...\n" );
  error = cam.StopCapture();
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }      
  error = cam.Disconnect();
  if (error != PGRERROR_OK) {
        PrintError( error );
        return -1;
  }  

}
