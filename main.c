#if defined(_WIN32) || defined(_WIN64)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "FlyCapture2_C.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "Functions.h"
//#include "header.h"
//#include "ConvertDimension.c"

void SetTimeStamping(fc2Context context, BOOL enableTimeStamp)
{
    fc2Error error;
    fc2EmbeddedImageInfo embeddedInfo;

    error = fc2GetEmbeddedImageInfo(context, &embeddedInfo);
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2GetEmbeddedImageInfo: %s\n", fc2ErrorToDescription(error));
    }

    if (embeddedInfo.timestamp.available != 0)
    {
        embeddedInfo.timestamp.onOff = enableTimeStamp;
    }

    error = fc2SetEmbeddedImageInfo(context, &embeddedInfo);
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2SetEmbeddedImageInfo: %s\n", fc2ErrorToDescription(error));
    }
}

int GrabImages(fc2Context context, int numImagesToGrab)
{
    fc2Error error;
    fc2Image rawImage;
    fc2Image convertedImage;
    fc2TimeStamp prevTimestamp = {0};
    fc2PGMOption *pgmoption = {0}; //This line is necessary to disable the "Save as binary file" option from the flycapture2 program.                      //Otherwise, the file is unreadable
    double **phase;
    char buf[20];

    error = fc2CreateImage(&rawImage);
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2CreateImage: %s\n", fc2ErrorToDescription(error));
        return -1;
    }

    error = fc2CreateImage(&convertedImage);
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2CreateImage: %s\n", fc2ErrorToDescription(error));
        return -1;
    }

    // If externally allocated memory is to be used for the converted image,
    // simply assigning the pData member of the fc2Image structure is
    // insufficient. fc2SetImageData() should be called in order to populate
    // the fc2Image structure correctly. This can be done at this point,
    // assuming that the memory has already been allocated.
    for (int i = 0; i < 10/*numPhases to Calculate*/; i++)
    {
        for (int j=0; j<4;j++){
        // Retrieve the image
        Sleep(0.154); //In theory this is the quarter period delay.
        //We still need to calculate an appropriate camera delay so that we measure on the
        //right points of the wave.
        error = fc2RetrieveBuffer(context, &rawImage);

        if (error != FC2_ERROR_OK)
        {
			printf("Error in retrieveBuffer: %s\n", fc2ErrorToDescription(error));
            return -1;
        }
        else
        {
            // Get and print out the time stamp
            fc2TimeStamp ts = fc2GetImageTimeStamp(&rawImage);
//            int diff = (ts.cycleSeconds - prevTimestamp.cycleSeconds) * 8000 +
//                       (ts.cycleCount - prevTimestamp.cycleCount);
//            prevTimestamp = ts;
//            printf("timestamp [%d %d] - %d\n",
//                   ts.cycleSeconds,
//                   ts.cycleCount,
//                   diff);
        }
        if (error == FC2_ERROR_OK)
        {
        // Save it to PGM

        printf("Saving the last image to image%d.pgm \n", j);
        snprintf(buf, 20, "image%d.pgm", j);
        error = fc2SaveImageWithOption(&rawImage, buf, FC2_PGM, &pgmoption);
        if (error != FC2_ERROR_OK)
            {
			printf("Error in fc2SaveImage: %s\n", fc2ErrorToDescription(error));
            printf("Please check write permissions.\n");
            return -1;
            }
        }
        }
    phase=ProcessImages();

    }

    error = fc2DestroyImage(&rawImage);
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2DestroyImage: %s\n", fc2ErrorToDescription(error));
        return -1;
    }

    error = fc2DestroyImage(&convertedImage);
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2DestroyImage: %s\n", fc2ErrorToDescription(error));
        return -1;
    }

    return 0;
}

//int main(){
//char ip_addr[32] = "11111111111111111111111111111111";
//  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//  {
//    perror("[-] Socket error");
//    exit(1);
//  }
//  else
//  {
//    printf("[+] TCP server socket created\n");
//  }
//  strcpy(ip_addr,"169.254.8.101");
//
//  memset(&addr, 0, sizeof(addr));
//  addr.sin_family = AF_INET;
//  addr.sin_port = htons(PORT);
//  addr.sin_addr.s_addr = inet_addr(ip_addr);
//
//    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
//  {
//    perror("connect");
//    exit(1);
//  }
//  else
//  {
//    printf("[+] Connected\n");
//  }
//
//
//return 0;
//}

int main(){
ProcessImages();
}


//int main(int argc, char **argv){
//
//    const unsigned int k_numImages = 1;
//
//    ////These variables will contain information about
//    fc2Error error; //Errors in the code
//    fc2Context context; //This is pretty much the camera we are working with itself
//    fc2PGRGuid guid;
//    fc2TriggerMode triggermode= {1,1,0,0,0}; //This has the trigger settings (activate ext. trig., polarity, gpio pin, trig. mode, paramter)
//    fc2Property *prop={FC2_AUTO_EXPOSURE, 1, 1, 1,1,0,-3}; //This is setting the exposure to -3 eV
//    error = fc2CreateContext(&context);//We initialize the camera information
//    if (error != FC2_ERROR_OK) //Error checking
//    {
//		printf("Error in fc2CreateContext: %s\n", fc2ErrorToDescription(error));
//        printf("Press Enter to exit...\n");
//        getchar();
//        return -1;
//    }
//
//    ////Get the 0th camera
//    error = fc2GetCameraFromIndex(context, 0, &guid);
//    if (error != FC2_ERROR_OK)
//    {
//        fc2DestroyContext(context);
//
//		printf("Error in fc2GetCameraFromIndex: %s\n", fc2ErrorToDescription(error));
//        printf("Press Enter to exit...\n");
//        getchar();
//        return -1;
//    }
//
//    error = fc2Connect(context, &guid);
//    if (error != FC2_ERROR_OK)
//    {
//        fc2DestroyContext(context);
//
//		printf("Error in fc2Connect: %s\n", fc2ErrorToDescription(error));
//        printf("Press Enter to exit...\n");
//        getchar();
//        return -1;
//    }
//    SetTimeStamping(context, TRUE);
//    error = fc2SetPropertyBroadcast(context, &prop); //Sets the desired exposure value
//    error = fc2SetVideoModeAndFrameRate(context, FC2_VIDEOMODE_640x480Y8, FC2_FRAMERATE_30); //Sets the videomode capable of 30 fps
//    error = fc2SetTriggerMode(context, &triggermode); //This sets trigger mode settings stated above
//    if (error != FC2_ERROR_OK)
//    {
//        fc2DestroyContext(context);
//
//		printf("Error in setting the triggermode: %s\n", fc2ErrorToDescription(error));
//        printf("Press Enter to exit...\n");
//        getchar();
//        return -1;
//    }
//
//    error = fc2StartCapture(context);
//    if (error != FC2_ERROR_OK)
//    {
//        fc2DestroyContext(context);
//
//		printf("Error in fc2StartCapture: %s\n", fc2ErrorToDescription(error));
//        printf("Press Enter to exit...\n");
//        getchar();
//        return -1;
//    }
//
//    if (GrabImages(context, k_numImages) != 0)
//    {
//        fc2DestroyContext(context);
//
//        printf("Press Enter to exit...\n");
//        getchar();
//        return -1;
//    }
//
//
//    error = fc2StopCapture(context);
//    if (error != FC2_ERROR_OK)
//    {
//        fc2DestroyContext(context);
//
//		printf("Error in fc2StopCapture: %s\n", fc2ErrorToDescription(error));
//        printf("Press Enter to exit...\n");
//        getchar();
//        return -1;
//    }
//    error = fc2DestroyContext(context); //And here we close the camera information
//    if (error != FC2_ERROR_OK)
//    {
//		printf("Error in fc2DestroyContext: %s\n", fc2ErrorToDescription(error));
//        printf("Press Enter to exit...\n");
//        getchar();
//        return -1;
//    }
//
//    printf("Press Enter to exit...\n");
//    getchar();
//    return 0;
//}
