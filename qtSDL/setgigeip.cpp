//  October 22, 2014- this left the camera in a bad state
//  Don't use until fixed  KAR
//

//#include "stdafx.h"
#include "stdio.h"

#include "FlyCapture2.h"

using namespace FlyCapture2;

void PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf( 
        version, 
        "FlyCapture2 library version: %d.%d.%d.%d\n", 
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    printf( "%s", version );

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    printf( "%s", timeStamp );
}

void PrintCameraInfo( CameraInfo* pCamInfo )
{
    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        pCamInfo->serialNumber,
        pCamInfo->modelName,
        pCamInfo->vendorName,
        pCamInfo->sensorInfo,
        pCamInfo->sensorResolution,
        pCamInfo->firmwareVersion,
        pCamInfo->firmwareBuildTime );
}

void PrintError( Error error )
{
    error.PrintErrorTrace();
}

int main(int /*argc*/, char** /*argv*/)
{
    PrintBuildInfo();

    Error error;
    BusManager busMgr;
    unsigned int numCameras;
    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    printf( "Number of cameras detected: %u\n", numCameras );

    if ( numCameras < 1 )
    {
        printf( "Insufficient number of cameras... exiting\n" );
        return -1;
    }

    PGRGuid guid;
    error = busMgr.GetCameraFromIndex(0, &guid);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    Camera cam;

    // Connect to a camera
    error = cam.Connect(&guid);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

GigECamera *gecam;
gecam= new GigECamera;
error= gecam->Connect(&guid);
if (error != PGRERROR_OK) {
  PrintError(error);
  return(-1);
}
unsigned int ipMac1, ipMac2; 
error= gecam->ReadGVCPRegister (0x0008, &ipMac1);
if (error != PGRERROR_OK) {
  PrintError(error);
}
error= gecam->ReadGVCPRegister (0x000c, &ipMac2);
if (error != PGRERROR_OK) {
  PrintError(error);
}
printf("MAC= %8.8x %8.8x\n", ipMac1, ipMac2);
error= gecam->ReadGVCPRegister (0x0010, &ipMac2);
if (error != PGRERROR_OK) {
  PrintError(error);
}
// Bit 0 persistent, Bit 1 dhcp, Bit 2 LLA
printf("Supported IP Configuration= %8.8x\n", ipMac2);
error= gecam->ReadGVCPRegister (0x0014, &ipMac2);
if (error != PGRERROR_OK) {
  PrintError(error);
}
printf("Current IP Configuration= %8.8x\n", ipMac2);
error= gecam->ReadGVCPRegister (0x0024, &ipMac2);
if (error != PGRERROR_OK) {
  PrintError(error);
}   
printf("Current IP Address= %8.8x\n", ipMac2);
error= gecam->ReadGVCPRegister (0x064c, &ipMac2);
if (error != PGRERROR_OK) {
  PrintError(error);
}
printf("Persistent IP Address= %8.8x\n", ipMac2);

#if 1
ipMac2= 0xc0a80152;  
error=gecam->WriteGVCPRegister(0x064c, ipMac2, 0);
if (error != PGRERROR_OK) {
  PrintError(error);
}
ipMac2= 0x0005;
error=gecam->WriteGVCPRegister(0x0014, ipMac2, 0);
if (error != PGRERROR_OK) {
  PrintError(error);
}

#endif

    // Get the camera information
    CameraInfo camInfo;
    error = cam.GetCameraInfo(&camInfo);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    PrintCameraInfo(&camInfo);        

    // Disconnect the camera
    error = cam.Disconnect();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

	return 0;
}
