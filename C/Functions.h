#include "FlyCapture2_C.h"
#include <winsock2.h>
#include <complex.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#if defined(_WIN32) || defined(_WIN64)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <signal.h>
#include "FlyCapture2_C.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#define PI     3.14159265358979323846264338327


/*GLOBAL VARIABLES*/
fc2Context context; //Contains the information for the camera we are using. Most flycapture2 API functions need it
int globalcounter; //Used to specify the amount of phase calculations we want
int length; //Used to specify the area of the square in the middle of the results matrix that we take into account. This way we avoid noise at the edge of the camera's sensor area
FILE *pdata; //Pointer to the file were the mean and standard deviation are being saved
double **phase; //Matrix with the phase data
double maxPhase; //Self explanatory
SOCKET s; //Socket variable necessary to send information to the Red Pitaya.
clock_t t;  //Optional variable, I used this to calculate the Frames per second of the program. Without data saving, program runs at about 2.3 fps

/*TAKE A PICTURE FUNCTION*/
int **GrabImages(fc2Context context,char* message, int rows, int col);

/*CLIENT SERVER RELATIONSHIP FUNCTIONS*/
int InitializeConnection();
int communicate(char *message);

/*DATA SAVING FUNCTIONS*/
//Commenting out the lines of code in function.c related to saving data speeds up the program
int saveImagePhase(double **photoMatrix, int rows, int col, char *name);
int saveImageInt(int **photoMatrix, int rows, int col, char *name);


/*DATA PROCESSING*/
double **CalculatePhase(int **arr0, int **arr1, int **arr2, int **arr3, int rows, int col);
double stdDeviation(double **matrix, double mean, int rows, int col);
double findMaximum(double **mat);

/*GUI FUNCTIONS*/
float* linspace(float init, float fin, int N);
void timer( int value);
void renderScene(void);
void initializeGUI(int argc, char **argv);

/*GUI'S SCALE FUNCTIONS - TAKEN FROM FELIPE GUZMANS' AND ALBERTO ZAMORAS' CODE*/
void output(int x, int y, char *string);
void pal (double x, double *r, double *g, double *b, int no);
int scale (double minimal, double maximal, double factor, int choice);



/*
LEGACY DECLARATIONS. THESE FUNCTIONS ARE NOT LONGER IN USE BUT THEIR CODE
REMAINS AT THE BOTTOM OF FUNCTIONS.C
int **ConvertDimension(char *filename, float value);
double **ProcessImages();
*/


