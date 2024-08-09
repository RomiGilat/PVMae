#include "Functions.h" //This header contains all necessary libraries

// I think the xcoords in the gui function may be causing too much memory to work
// trying defining them outside to see if they can be a global variable so it isnt reallocating them every frame



int main(int argc, char **argv){

    /*

    UNCOMMENT THIS IF YOU WANT TO TEST EACH TRIGGER

    // Josh testing
    printf("josh testing:\n");


    // 07

    printf("Sending a 07\n");
    communicate("07");
    printf("07 sent!\n");
    getchar();

    // 09
    printf("Sending a 09\n");
    communicate("09");
    printf("09 sent!\n");
    getchar();

    printf("Sending a 07\n");
    communicate("07");
    printf("07 sent!\n");
    getchar();

    // 10
    printf("Sending a 10\n");
    communicate("10");
    printf("10 sent!\n");
    getchar();

    printf("Sending a 07\n");
    communicate("07");
    printf("07 sent!\n");
    getchar();


    // 11
    printf("Sending a 11\n");
    communicate("11");
    printf("11 sent!\n");
    getchar();


    printf("Sending a 07\n");
    communicate("07");
    printf("07 sent!\n");
    getchar();


    // 12
    printf("Sending a 12\n");
    communicate("12");
    printf("12 sent!\n");
    getchar();


    printf("Sending a 07\n");
    communicate("07");
    printf("07 sent!\n");
    getchar();


    // 07
    printf("Sending a 07\n");
    communicate("07");
    printf("07 sent!\n");
    getchar();


    printf("josh testing over\n");
    // pressing enter will resume code as normal
    getchar();
    */


    // Start up procedure
    printf("-------------------------\n");
    printf("Initializing PVMae GUI.\n");
    printf("Ensure that the server is running on the Red Pitaya");
    printf("    $: gcc -o server RPserver.c    TO RECOMPILE\n");
    printf("    $: ./server                    TO RUN SERVER\n");
    printf("Press enter once you have verified the server is running.\n");
    getchar();

    const char * IPs[] = {
            "169.254.2.239", // RP f084ec main
            "169.254.52.185",    // RP f084ec alternate
            "169.254.8.101", // RP f0949f main
            "169.254.251.182"// RP f0949f alternate
        };


    int choice;

    // Prompt the user for input
    printf("Please pick from the following IP addresses for the Red Pitaya:\n");
    printf("The IP can be found in the RP dashboard under System>Network Manager\n");
    printf("    1. RP-f084ec    : 169.254.2.239\n");
    printf("    2. RP-f084ec alt: 169.254.52.185\n");
    printf("    3. RP-f0949f    : 169.254.8.101\n");
    printf("    4. RP-f0949f alt: 169.254.251.182\n");
    printf("Enter a number (1, 2, 3, or 4): ");


    // Read the user's input
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number.\n");
        return 1; // Exit with error code
    }

    // Check if the input is within the valid range
    if (choice >= 1 && choice <= 4) {
        printf("You entered: %d; ", choice);
        printf("%s\n", IPs[choice-1]);
    } else {
        printf("Invalid choice. Please enter a number between 1 and 4.\n");
    }

    IP = IPs[choice-1];

    getchar();
    printf("-------------------------\n");


    // Begin real system
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
    //pdata=fopen("RawResults.txt", "w"); //Creates variable for data saving
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
    //fclose(pdata);
    WSACleanup();

    printf("Press Enter to exit...\n");

    getchar();
    return 0;
}
