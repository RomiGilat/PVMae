#include "Functions.h"
//These variables need to be declared in this file, otherwise it does not run. They are related to the gui's scale
struct pp // Structure to estimate the RGB color palette.
{
  double x, r, g, b;
};
struct pp ppl0[] = {
  {-PI, 0, 0, 1}, //-Pi and Pi are the max possible phases in rads. 0,0,1 and 1,0,0 are the colors representing these max values
  {0.2, 0, 0, 0},//the middle 3 rows are related to the color transitions between them
  {0, 0, 0, 0},
  {-0.2, 0, 0, 0},
  {PI, 1, 0, 0}
};
static int npp0 = sizeof (ppl0) / sizeof (struct pp);
void *font = GLUT_BITMAP_HELVETICA_18;

//START OF FUNCTION DEFINITIONS

int **GrabImages(fc2Context context,char* message, int rows, int col){
    fc2Image rawImage;
    fc2Error error;
    int **photoMatrix;
    photoMatrix=malloc(sizeof(int*)*rows);
    for (int n=0; n<rows; n++){
        photoMatrix[n]=malloc(sizeof(int)*col);
    }
    error = fc2CreateImage(&rawImage);
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2CreateImage: %s\n", fc2ErrorToDescription(error));
        return -1;
    }
    communicate(message); //This is the message that is defined before every call of grab images. This includes the trigger delay requested
    error = fc2RetrieveBuffer(context, &rawImage);//This is the function that "takes the picture", meaning, it tells the camera to fill the buffer for the duration of the shutter time
                                                  //and then retrieves the electrical signal from each pixel
    message="07"; //we tell the camera that we don't want any more triggers
    communicate(message);

    if (error != FC2_ERROR_OK)
    {
        printf("Error in retrieveBuffer: %s\n", fc2ErrorToDescription(error));
        return -1;
    }
    int counter=0;
    for (int i=0; i<rows;i++){
        for (int j=0; j<col; j++){
            photoMatrix[i][j]= *(rawImage.pData+counter);
            //printf("%d\n", photoMatrix[i][j]);
            counter++;
            }
    }

    error = fc2DestroyImage(&rawImage);
    if (error != FC2_ERROR_OK)
    {
		printf("Error in fc2DestroyImage: %s\n", fc2ErrorToDescription(error));
        return -1;
    }
    return photoMatrix;
}

int InitializeConnection(){
    WSADATA wsa;

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");


    //printf("Socket created.\n");
    char *message="07"; //The meaning of the messages will be in the auxiliary document. But for example, 07 means that we are not requesting triggers
    communicate(message);
    return 0;
}

int communicate(char *message){ //Very important function, this is the one that actually sends info to the RP
    SOCKET s;
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("169.254.8.101"); //RP-f0949f default IP address (it changes after rebooting sometimes)
    server.sin_family = AF_INET;
    server.sin_port = htons(1001);

    //Connect to remote server
    if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    }
    if( send(s , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return 1;
    }
    return 0;
}


//We need to different functions because of the different data types between the intensity arrays and the phase array (int and double respectively)

int saveImagePhase(double **photoMatrix, int rows, int col, char *name){
    int i, j;
    FILE *fp;
    fp=fopen(name, "w");
    for(i = 0; i < rows; i++){
        for(j = 0; j < col; j++){
                fprintf(fp,"%.3f ", photoMatrix[i][j]); //You can change the decimal amount by modifying the %.3f format string
            }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return 0;
}

int saveImageInt(int **photoMatrix, int rows, int col, char *name){
    int i, j;
    FILE *fp;
    fp=fopen(name, "w");
    for(i = 0; i < rows; i++){
        for(j = 0; j < col; j++){
                fprintf(fp,"%d ", photoMatrix[i][j]);
            }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return 0;
}

double **CalculatePhase(int **arr0, int **arr1, int **arr2, int **arr3, int rows, int col){//arr3 is the newest one
    int **arrA;
    int **arrB;
    int **arrD;
    double **result;
    double complex z =0.0+I*0.0; //Felipe's recommendation was to use complex numbers since it makes calculating
                                 //the rest of the quantities outside of phase (contrast, dark fringe, etc) easier
    double sum=0.0;
    double mean=0.0;
    int totData=0;
    double deviation=0.0;
    arrA = malloc(sizeof(int*)*rows);
    arrB = malloc(sizeof(int*)*rows);
    arrD = malloc(sizeof(int*)*rows);
    result = malloc(sizeof(double*)*rows);
    for (int i=0;i<rows;i++){
        arrA[i]=malloc(sizeof(int)*col);
        arrB[i]=malloc(sizeof(int)*col);
        arrD[i]=malloc(sizeof(int)*col);
        result[i]=malloc(sizeof(double)* col);
    }
    for (int i=0; i<rows;i++){
        for (int j=0; j<col; j++){
            arrA[i][j]=(arr0[i][j])-(arr2[i][j]);
            arrB[i][j]=(arr3[i][j])-(arr1[i][j]);
            arrD[i][j]=arr0[i][j]+arr1[i][j]+arr2[i][j]+arr3[i][j];
            z = (double)arrA[i][j]+(double)arrB[i][j]*I;
            result[i][j] = carg(z);
            //printf("%f ",result[i][j]); //This is useful for troubleshooting
            if ((j>640-length/2)&(j<640+length/2)&(i>480-length/2)&(i<480+length/2)){ //Only data in the square of side length
                                                                                      //in the middle of the screen is taken into account
                sum+=result[i][j];
                totData+=1;
            }
        }
    }

        for (int i=0;i<rows;i++){
        free(arrA[i]);
        free(arrB[i]);
        free(arrD[i]);
    }
    mean=sum/totData;
    deviation=stdDeviation(result, mean, rows, col);
    printf("mean: %.6f rads\ndeviation: %.6f rads\n", mean, deviation);
    fprintf(pdata,"%.6f;%.6f\n", mean, deviation);
    free(arrA);
    free(arrB);
    free(arrD);
    return result;
}


double stdDeviation(double **matrix, double mean, int rows, int col){
    double deviation;
    double sum=0.0;
    int totData=0;
    for (int i=0; i<rows; i++){
        for (int j=0; j<col; j++){
            if ((j>640-length/2)&(j<640+length/2)&(i>480-length/2)&(i<480+length/2)){ //Same here for deviation
            sum+=pow((matrix[i][j]-mean),2);
            totData+=1;
            }
        }
    }
    deviation=sqrt(sum/(totData-1));
return deviation;
}

double findMaximum(double **mat){
    // Initializing max element as INT_MIN
    double maxElement = 0.0;

    // checking each element of matrix
    // if it is greater than maxElement,
    // update maxElement
    int row=960;
    int col=1280;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (mat[i][j] > maxElement) {
                maxElement = mat[i][j];
                row = i;
                col = j;
            }
        }
    }

    return maxElement;
}

float* linspace(float init, float fin, int N) {
     float *x;
     int i = 0;
     float step = (fin - init) / (float)N;
     x = (float*)calloc(N, sizeof(float));

     x[i] = init; /*<=== Line 14*/

     for (i = 1; i < N; i++) {
         x[i] = x[i - 1] + step;
     }
     x[N - 1] = fin;
     return x;
}

//Timer function is basically the main loop in the program, this is re run by the amount of times specified in globalcounter
void timer( int value){
    int **arr0;
    int **arr1;
    int **arr2;
    int **arr3;
    char *message;
    int rows=960;
    int col=1280;
    for (int k=0; k<4;k++){
        //To save data, just uncomment the 3 lines in each conditional
        if (k==0){
        message = "08";
        arr0 = GrabImages(context,message, rows, col);
        //char buf0[16];
        //snprintf(buf0, 16, "image0_%d_.txt", globalcounter);
        //saveImageInt(arr0, rows, col, buf0);
        }
        else if (k==1){
        message = "09";
        arr1 = GrabImages(context,message, rows, col);
        //char buf1[16];
        //snprintf(buf1, 16, "image1_%d_.txt", globalcounter);
        //saveImageInt(arr0, rows, col, buf1);
        }
        else if (k==2){
        message = "10";
        arr2 = GrabImages(context,message, rows, col);
        //char buf2[16];
        //snprintf(buf2, 16, "image2_%d_.txt", globalcounter);
        //saveImageInt(arr0, rows, col, buf2);
        }
        else if (k==3){
        message = "11";
        arr3 = GrabImages(context,message, rows, col);
        //char buf3[16];
        //snprintf(buf3, 16, "image3_%d.txt", globalcounter);
        //saveImageInt(arr0, rows, col, buf3);
        }
    }

    phase=CalculatePhase(arr0, arr1, arr2, arr3, rows, col);
    //char bufphase[16];
    //snprintf(bufphase, 16, "phase%d.txt", globalcounter);
    //saveImagePhase(phase, rows, col, bufphase);
    for (int i=0;i<rows;i++){
        free(arr0[i]);
        free(arr1[i]);
        free(arr2[i]);
        free(arr3[i]);
    }
    free(arr0);
    free(arr1);
    free(arr2);
    free(arr3);

    globalcounter=globalcounter+1;
    if (globalcounter>100){ //this ends measurements
    t=clock()-t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Average frames per second: %f fps", ((double)globalcounter)/time_taken);
    return;}

    // Glut functions
    glutPostRedisplay();
    glutTimerFunc( 16, timer, 0 );
}

void renderScene(void) {

    // load pgm

    // clear screen and get the context
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();//load identity matrix
    glDisable(GL_DEPTH_TEST);

    glPointSize(2.0f);//set point size to 1 pixels

    glBegin(GL_POINTS); //starts drawing of points

    // this gives us the normalized device coordinates of the window so we can plot the square in the center
    float *xcoords;
    float *ycoords;
    xcoords=malloc(sizeof(float*)*1280);
    ycoords=malloc(sizeof(float*)*960);
    xcoords = linspace(-0.75, 0.75, 1280);
    ycoords = linspace(-0.75, 0.75, 960);

    float z; // this will store
    maxPhase=findMaximum(phase);
    for (int row = 0; row < 960; row++)
    {
        //glVertex3f(xcoords[i], 0, 0);
        //printf("%lf\n", xcoords[i]);
        for (int col=0; col < 1280 ; col++)
        {
            z = phase[row][col] / maxPhase; // this will give us color out 1.0 so we can set color properly

            glColor3f(0.0, 0.0, 0.0); // white

            if (phase[row][col] > 0.0)
            {
                // Make positive phase red
                glColor3f(fabs(z), 0, 0);
            }
            else if (phase[row][col]==0.0){
                glColor3f(0, 0, 0);
            }
            else {
                // Make negative phase blue
                glColor3f(0,0,fabs(z));
            }
//            if ((col>640-length/2)&(col<640+length/2)&(row>480-length/2)&(row<480+length/2)){
//                glColor3f(0, 0, 0);
//            }
            // draw pixel at coordinate
            glVertex3f(xcoords[col], ycoords[row], 0);
        }
    }

    glEnd();//end drawing of points
    scale (ppl0[0].x, ppl0[4].x, 1, 0);
    glutSwapBuffers();


}
void initializeGUI(int argc, char **argv) {
    glutInit (&argc, argv); // this starts the glut api
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // sets the settings for the display
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1300,1000);
	glViewport(0, 0, 1280, 960); // this is where we define the normalized device coordinates "working area"
	glutCreateWindow("PVMae");

    glutDisplayFunc(renderScene); // this display function will be called every frame

    glutTimerFunc(0, timer, 0); // timer is what is called every frame, INSIDE should be the frame to capture phase values

    glutMainLoop();

}
void output (int x, int y, char *string){
  int len, i;

  glRasterPos2f ((float) x / 100.0, (float) y / 100.0);
  len = (int) strlen (string);
  for (i = 0; i < len; i++)
    {
      glutBitmapCharacter (font, string[i]);//This prints the labels for the scale
    }
}



void pal (double x, double *r, double *g, double *b, int no){
  int i = 0;
  double x1, x2, z1, z2;

  struct pp *ppt;
  int npp;

  switch (no)
    {
    case 0:
      ppt = (ppl0); //All of the uncommented lines are used for the different possible colors for all other windows of measured data in the program
      npp = npp0;
      break;
//    case 1:
//      ppt = (ppl1);
//      npp = npp1;
//      break;
//    case 2:
//      ppt = (ppl2);
//      npp = npp2;
//      break;
//    case 3:
//      ppt = (ppl3);
//      npp = npp3;
//      break;
//    case 4:
//      ppt = (ppl4);
//      npp = npp4;
//      break;
    default:
      assert (1 == 0);
    }

  while (x > (ppt + i)->x)
    ++i;
  if (i >= npp)
    {
      //printf ("x=%g no=%d\n", x, no);
    }

  x1 = (ppt + i - 1)->x;
  x2 = (ppt + i)->x;

  z1 = (ppt + i - 1)->r;
  z2 = (ppt + i)->r;
  *r = (x - x1) / (x2 - x1) * (z2 - z1) + z1;

  z1 = (ppt + i - 1)->g;
  z2 = (ppt + i)->g;
  *g = (x - x1) / (x2 - x1) * (z2 - z1) + z1;

  z1 = (ppt + i - 1)->b;
  z2 = (ppt + i)->b;
  *b = (x - x1) / (x2 - x1) * (z2 - z1) + z1;
}


int scale (double minimal, double maximal, double factor, int choice){
  int i;
  char string[100];
  int step = 21;
  double x, r, g, b;

  font = GLUT_BITMAP_TIMES_ROMAN_10;

  for (i = 0; i < step; i++)
    {
      glBegin (GL_QUADS);	// start drawing a polygon (4 sided)
      x = ((double) i + 0.5) / (double) step *(maximal - minimal) + minimal;
      pal (x / factor, &r, &g, &b, choice);
      glColor3f (r, g, b);

      glVertex3f (0.50, 0.63 - (float) (step - i) / (float) step, 0.0);	// Top Left
      glVertex3f (0.60, 0.63 - (float) (step - i) / (float) step, 0.0);	// Top Right
      glVertex3f (0.60, 0.53 - (float) (step - i) / (float) step, 0.0);	// Bottom Right
      glVertex3f (0.50, 0.53 - (float) (step - i) / (float) step, 0.0);	// Bottom Left
      glEnd ();			// done with the polygon

//      if (i % 2 == 0 && choice!=0)
//	{
//	  sprintf (string, "%.4f", x);
//	  glColor3f (1.0, 1.0, 1.0);
//	  output (65,59 - ((int) ((float) (step - i) * 1.02 / (float) step * 100)),string);
//	}

      if (choice == 0)
	{
	  sprintf (string, "%.4f  rad", x);
	  glColor3f (1.0, 1.0, 1.0);
	  output (60,59 - ((int) (100*((float) (step - i) * 1.08  / (float) step))), string);
	}
    }
    //Legacy code from this function, these printed additional scale divisions representing Out of scale values
//    if (choice == 0)
//    {
//      glBegin (GL_QUADS);	// start drawing a polygon (4 sided)
//      glColor3f (0, 1, 0);
//      glVertex3f (0.50, 0.64, 0.0);  // Top Left
//      glVertex3f (0.60, 0.64, 0.0); // Top Right
//      glVertex3f (0.60, 0.58, 0.0); // Bottom Right
//      glVertex3f (0.50, 0.58, 0.0); // Bottom Left
//      glColor3f (1, 1, 0);
//      glVertex3f (0.50,-0.47, 0.0);  // Top Left
//      glVertex3f (0.60, -0.47, 0.0); // Top Right
//      glVertex3f (0.60,-0.53, 0.0); // Bottom Right
//      glVertex3f (0.50,-0.53, 0.0); // Bottom Left
//      glEnd ();			// done with the polygon
//
//      sprintf (string, "Over scale's maximum");
//      glColor3f (1.0, 1.0, 1.0);
//      output (65,60, string);
//
//
//      sprintf (string, "Under scale's minimum");
//      glColor3f (1.0, 1.0, 1.0);
//      output (65,-53, string);
//    }

  return 0;
}





/*Following are "legacy functions", they are not useful in the current version
of the program, but they may be useful for reference in the future
*/



//int **ConvertDimension(char *filename, float value){ //this value parameter is for testing purposes only
//    FILE *fp;
//    char readout[64];
//    int col=0;
//    int row=0;
//*
//These next lines are called dynamic memory allocation. It was necessary
//to build the arrays this way since the normal one (array[480][640]) was creating
//issues (I think memory related, I'm not too sure).
//
//*/
//    int **matrix;
//
//    matrix=malloc(sizeof(int*)*480);
//
//    for (int i=0; i<480; i++)
//    matrix[i]=malloc(sizeof(int)*640);
//
//    fp=fopen(filename, "r");
//    getc(fp);//All of these lines make the pointer on the file start on the
//    getc(fp);//actual data and ignore the header information
//    getc(fp);
//    getc(fp);
//    while(getc(fp) != '\n'); //skip to end of line
//    while (getc(fp)=='#'){      //skip comment lines
//        while (getc(fp)!='\n');//skip to end of comment line
//    }
//    fseek(fp, -1, SEEK_CUR);
//    fscanf(fp, "%s", &readout);
//
//    for (int i=0; i<307200; i++){
//        fscanf(fp, "%s", &readout);
//        matrix[row][col]=atoi(readout);
//        matrix[row][col]=value;
//        col=col+1;
//        if(col>639){
//            col=0;
//            row=row+1;
//        }
//
//    }
//    fclose(fp);
//    return matrix;
//
//
//}

//double **ProcessImages(){
//    int rows=480;
//    int col= 640;
//    int **arr0;
//    int **arr1;
//    int **arr2;
//    int **arr3;
//    double **result;
//    arr0=ConvertDimension("image0.pgm", 0.5);
//    arr1=ConvertDimension("image1.pgm",0);
//    arr2=ConvertDimension("image2.pgm",0.5);
//    arr3=ConvertDimension("image3.pgm",1);
//    result=CalculatePhase(arr0, arr1, arr2, arr3, rows, col);
//    //printf("Phase calculated\n");
//
////        for(int x = 0; x < rows; x++) { //Debugging
////        for(int y = 0; y < col; y++) {
////            printf("%f ", phase[x][y]);
////        }
////    printf("\n");
////    }
//    /*
//    After the arrays are used, we should free them from memory
//    */
//
//    for (int i=0;i<rows;i++){
//        free(arr0[i]);
//        free(arr1[i]);
//        free(arr2[i]);
//        free(arr3[i]);
//    }
//    free(arr0);
//    free(arr1);
//    free(arr2);
//    free(arr3);
//    return result;
//}
