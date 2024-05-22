#include "Functions.h" //This header contains all necessary libraries

int main(int argc, char **argv){
    t=clock();
    fc2Error error; //Errors in the code
    fc2PGRGuid guid;
    fc2TriggerMode triggermode= {1,1,0,0,0}; //This has the trigger settings (activate ext. trig., polarity, gpio pin, trig. mode, parameter)
    fc2Property propexposure={FC2_AUTO_EXPOSURE, 1, 1, 0, 1, 1, 0,0, 0};//This is setting the exposure to 0 EV ->These ARE NOT electron volts, they are "exposure value"
    //{property, present, abscontrol, onepush, onoff, automode, A,B, absValue}
    fc2Property propshutter={FC2_SHUTTER, 1, 1, 0,1,0,0,0,0.02};//This are the the shutter value settings to set it to 0.020 ms
    fc2Property propgain={FC2_GAIN, 1, 1, 0,1,0,0,0,0}; //This will be setting the gain to 0 dB
    int rows=960; //Dimensions of the data matrix
    int col=1280;
    pdata=fopen("RawResults.txt", "w"); //Creates variable for data saving
    globalcounter=0;
    length=100; //Size of square in the middle of the screen where we actually measure data
    phase=malloc(sizeof(double*)*960);

    for (int i=0; i<960; i++){
        phase[i]=malloc(sizeof(double)*1280);
        }
    error = fc2CreateContext(&context);//We initialize the camera information
    if (error != FC2_ERROR_OK) //Error checking
    {
		printf("Error in fc2CreateContext: %s\n", fc2ErrorToDescription(error));
        printf("Press Enter to exit...\n");
        getchar();
        return -1;
    }
    printf("Context created\n");
    ////Get the 0th camera
    error = fc2GetCameraFromIndex(context, 0, &guid);
    if (error != FC2_ERROR_OK)
    {
        fc2DestroyContext(context);

		printf("Error in fc2GetCameraFromIndex: %s\n", fc2ErrorToDescription(error));
        printf("Press Enter to exit...\n");
        getchar();
        return -1;
    }
    error = fc2Connect(context, &guid);
    if (error != FC2_ERROR_OK)
    {
        fc2DestroyContext(context);

		printf("Error in fc2Connect: %s\n", fc2ErrorToDescription(error));
        printf("Press Enter to exit...\n");
        getchar();
        return -1;
    }

    //These actually set the values that we described at the beginning of main()
    error = fc2SetProperty(context, &propexposure);
    error = fc2SetProperty(context, &propshutter);
    error = fc2SetProperty(context, &propgain);
    error = fc2SetVideoModeAndFrameRate(context, FC2_VIDEOMODE_1280x960Y8, FC2_FRAMERATE_15); //Sets the videomode capable of 15 fps

    error = fc2SetTriggerMode(context, &triggermode); //This sets trigger mode settings stated above
     //This means which gpio pin in specific. So gpio pin 1 = overall pin 4 on the connector

    if (error != FC2_ERROR_OK)
    {
        fc2DestroyContext(context);

		printf("Error in setting the triggermode: %s\n", fc2ErrorToDescription(error));
        printf("Press Enter to exit...\n");
        getchar();
        return -1;
    }
    InitializeConnection();
    communicate("07");
    error = fc2StartCapture(context);
    if (error != FC2_ERROR_OK)
    {
        fc2DestroyContext(context);

		printf("Error in fc2StartCapture: %s\n", fc2ErrorToDescription(error));
        printf("Press Enter to exit...\n");
        getchar();
        return -1;
    }

    printf("Going to initialize gui\n");

    printf("Press enter to start the gui, turn on laser first\n");
    getchar();


    initializeGUI(argc, argv); //The loop for taking pictures is the gui's timer function


    error = fc2StopCapture(context);
    if (error != FC2_ERROR_OK)
    {
        fc2DestroyContext(context);

		printf("Error in fc2StopCapture: %s\n", fc2ErrorToDescription(error));
        printf("Press Enter to exit...\n");
        getchar();
        return -1;
    }

    error = fc2DestroyContext(context); //here we close the camera information
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2DestroyContext: %s\n", fc2ErrorToDescription(error));
        printf("Press Enter to exit...\n");
        getchar();
        return -1;
    }

    closesocket(s);
    fclose(pdata);
    WSACleanup();

    printf("Press Enter to exit...\n");
    getchar();
    return 0;
}
