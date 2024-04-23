//need this to get proper includes
#define LINUX

#if defined(_WIN32) || defined(_WIN64)
#define _CRT_SECURE_NO_WARNINGS
#endif

//	Linux libraries
#include <stdio.h>
#include <GL/glut.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <sys/io.h>
#include <unistd.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

//	Camera libraries
//#include "win2lin.h"
//#include "camera_defaults.h"

//	Camera libraries (FlyCapture)
#include "FlyCapture2_C.h"
#include "stdafx.h"
#include <stdbool.h>

//	TCP/IP libraries
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "bpf_client.h"

//	TCP/IP Constants
#define START 0
#define STOP 1
#define SAMP_FREQ 100000
#define CLK_FREQ 125000000
#define PORT 1001
#define M 278
//#define M 0

//	Defined Constants for the program
#define PI     3.14159265358979323846264338327		// Definition of PI
#define TWOPI  6.28318530717958647692528676654		// Definition of 2 * PI
#define nx 964						// Number of pixels in x
#define ny 1296						// Number of pixels in y
#define nz 4   // Number of necessary frames to calculate a phase front. Selects the algorithm to be used: 4-Point,7-Point,9-Points,13-Points
#define ZEIT 1						// Integration time in microseconds
#define GAMMA 0.5
#define CONTRAST_LIMIT 50.0
#define PERIOD 6160					// Heterodyne period in 10^-7 seconds

void init_mem(void);					// Allocates the memory. File init_mem.c
//int init_camera(void);		 			// Initializes the camera. File init_camera.c
int init_camera(fc2Context context);		 			// Initializes the camera. File init_camera.c
//int CaptureFrames(void); 				// Captures the nz frames for one phase front calculation and calls the function phase_calc. File capture_frames.c
int CaptureFrames (fc2Context context);                   // Captures the nz frames for one phase front calculation and calls the function phase_calc. File capture_frames.c
//int phase_calc(void);					// Calculates a phase front, contrast, average, minimum, and maximum out of the nz frames. File phase.c
int phase_calc(fc2Context context);					// Calculates a phase front, contrast, average, minimum, and maximum out of the nz frames. File phase.c
//int close_camera(void); 				// Closes the camera. File close_camera.c
int close_camera(fc2Context context); 				// Closes the camera. File close_camera.c
void free_mem(void);					// Clears the memory. File free_mem.c
int sleeping(int delay);	 			// Delay. Stops the programm for 'delay' milliseconds. File init_camera.c
void output(int x, int y, char *string); 		// Prints a string on a desired window. File gui.c
void mouse(int button, int state, int x, int y);	// Function for the mouse option. File gui.c
void motion(int x, int y);	 			// Function for the mouse option. File gui.c
void recalcModelPos(void);	 			// Function for the mouse option. File gui.c
void _Timer(int value); 				// Executes the commands defined in this function every 'value' milliseconds. File gui.c
//void _Timer(int value, fc2Context context); 				// Executes the commands defined in this function every 'value' milliseconds. File gui.c
//void Key(unsigned char key, int x, int y);		// Executes the commands specified for the corresponding 'key' case (defined in ASCII code!). File gui.c
//void new_timer(int value, fc2Context context);
void Key(unsigned char key, int x, int y);		// Executes the commands specified for the corresponding 'key' case (defined in ASCII code!). File gui.c
void back_gnd ();                                       // Takes pictures of the background light. File background.c
//void back_gnd (fc2Context context);                                       // Takes pictures of the background light. File background.c
void pal (double x, double *r, double *g, double *b, int no); // Defines the color palette for each plot. File rgb.c
int scale(double minimal, double maximal, double factor, int choice); // Plots the scale for the corresponding display. File gui.c
void DrawPhase(void);							 // Plots the phase front. File gui.c
void DrawAverage(void);							 // Plots the intensity average. File gui.c
void DrawMinCalc(void);							 // Plots the calculate intensity minimum. File gui.c
void DrawMaxMess(void);							 // Plots the measured intensity maximum. File gui.c
void DrawContrast(void);					         // Plots the contrast. File gui.c
void pixel(double size_x, double size_y, double x, double y, double z);	 // Defines and plots a pixel. File gui.c

int save_data(void); 					// Saves the phase and weight data in text files. File save_data.c


double ***foto,**phase,**contrast,**average,**maximum_mess,**minimum_calc,**background, *total_foto; // Global matrices. Created in file init_mem.c
double complex **z;                                                                     // Global complex matrix. Created in file init_mem.c

double max,min;			// Absolute intensity maximum and minimum for each nz frames series. Measured in file capture_frame.c
int win_phase,win_contrast,win_maxmess,win_mincalc,win_average, win_intensity; // Display's ID's. Defined in file main.c
double total_phase, total_contrast, total_background,period;	// Total phase, total contrast, and total background. Calculated in file phase.c
double maxph,minph; 					// Phase maximum and minimum for each measurment. Calculated in file phase.c

struct pp                                               // Structure to estimate the RGB color palette. File rgb.c
{
  double x, r, g, b;
};

extern struct pp ppl0[];                               // Palette for the phase front. File rgb.c
extern struct pp ppl1[];                              // Palette for the average. File rgb.c
extern struct pp ppl2[];                              // Palette for the contrast. File rgb.c
extern struct pp ppl3[];                              // Palette for the measured maximum. File rgb.c
extern struct pp ppl4[];                              // Palette for the calculated minimum. File rgb.c

double plotgamma;	// Variable plotgamma used for the brightness of the display. Files main.c gui.c
int counter;// Variable to control the trigger mode, when changing between software and external triggering. Files main.c capture_frame.c background.c

/*
int fpga_signal (void); 				// Sends a pulse to the FPGA through the EPP port. File port.c
int fpga2 (int delay, int trigger, int count);          // Sends a pulse to the FPGA through the EPP port. File parport.c
int counter_busy (void);                                // File parport.c
int delay_busy (void);                                  // File parport.c
void epp_write_data (unsigned char data);               // File parport.c
void epp_write_addr (unsigned char data);		// File parport.c
unsigned char epp_read_data (void);			// File parport.c
unsigned char epp_read_addr (void);			// File parport.c
unsigned char epp_read_status (void);			// File parport.c
void epp_reset (void);					// Resets the parallel port. File parport.c
void utoa16 (char *s, unsigned u);			// 16-bit counter. File parport.c
int rounding (double x);				// Function to round a variable to its integer part. File parport.c
void set_trigger (unsigned short trigger);		// Sets the parameters for the trigger pulse. File parport.c
void take_picture (unsigned short delay);		// Sends a pulse to the FPGA to take a pictur. File parport.c
double percounter (unsigned count);			// Measures the heterodyne period. File parport.c
*/

void PrintBuildInfo();                                            // File init_camera.c
void PrintCameraInfo(fc2Context context);                         // File init_camera.c
void SetTimeStamping(fc2Context context, BOOL enableTimeStamp);   // File init_camera.c
char *int_to_bin(int k);                                          // File init_camera.c

bool CheckSoftwareTriggerPresence(fc2Context context);	// File parport.c
bool PollForTriggerReady(fc2Context context);			// File parport.c
bool FireSoftwareTrigger(fc2Context context);			// File parport.c
int *CCD_ExternalCapture (fc2Context context, int *widebuffer, unsigned short timeout, const int k_numImages, const int trigg_source);		// File parport.c
int CCD_TriggerInit(fc2Context context, unsigned short timeout, const int trigg_source);							// File parport.c
int init_tcp_ip();				// File parport.c
int fpga_send(double delay);
int fpga_receive();
int delay_to_clockcycles (double delay);
double clockcycles_to_period (int period);

int create_TCP_socket();
void set_addr_properties(struct sockaddr_in address, int p, char* ip);
char* strRev(char *s);
char* zero_padding(int pad_bits);
char* dec2Bin(int n);
void integer_handle(int signal_number);

/*
char ui_string[32]; // User input string
char ip_addr[32];
char msg1[32], zpad[32], inbus[32];
volatile sig_atomic_t stp; // Stop signal for while loop
int sock;
struct sockaddr_in addr;
socklen_t addr_size;
char buffer[1024];
int conn;
int param, nbit;
*/

int widebuffer[nx * ny];		//Buffer used throughout the program
fc2Context context;

void DrawIntensity(void);                                          // File gui.c
double red (double x);                                          // File gui.c
double green (double x);                                          // File gui.c
double blue (double x);                                          // File gui.c
