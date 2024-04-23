#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>

#include "Functions.h"
int **ConvertDimension(char *filename, float value){ //this value parameter is for testing purposes only
    FILE *fp;
    char readout[64];
    int col=0;
    int row=0;
/*
This next lines are called dynamic memory allocation. It was necessary
to build the arrays this way since the normal one (array[480][640]) was creating
issues (I think memory related, I'm not too sure).

*/
    int **matrix;

    matrix=malloc(sizeof(int*)*480);

    for (int i=0; i<480; i++)
    matrix[i]=malloc(sizeof(int)*640);

    fp=fopen(filename, "r");
    getc(fp);//All of these lines make the pointer on the file start on the
    getc(fp);//actual data and ignore the header information
    getc(fp);
    getc(fp);
    while(getc(fp) != '\n'); //skip to end of line
    while (getc(fp)=='#'){      //skip comment lines
        while (getc(fp)!='\n');//skip to end of comment line
    }
    fseek(fp, -1, SEEK_CUR);
    fscanf(fp, "%s", &readout);

    for (int i=0; i<307200; i++){
        fscanf(fp, "%s", &readout);
        matrix[row][col]=atoi(readout);
        //matrix[row][col]=value;
        col=col+1;
        if(col>639){
            col=0;
            row=row+1;
        }

    }
    fclose(fp);
    return matrix;


}

double **CalculatePhase(int **arr0, int **arr1, int **arr2, int **arr3, int rows, int col){//arr3 is the newest one
    int **arrA;
    int **arrB;
    int **arrD;
    double **phase;
    arrA = malloc(sizeof(int*)*480);
    arrB = malloc(sizeof(int*)*480);
    arrD = malloc(sizeof(int*)*480);
    phase = malloc(sizeof(double*)*480);

    for (int i=0;i<rows;i++){
        arrA[i]=malloc(sizeof(int)*640);
        arrB[i]=malloc(sizeof(int)*640);
        arrD[i]=malloc(sizeof(int)*640);
        phase[i]=malloc(sizeof(double)*640);
    }
    for (int i=0; i<rows;i++){
        for (int j=0; j<col; j++){
            arrA[i][j]=arr0[i][j]-arr2[i][j];
            arrB[i][j]=arr3[i][j]-arr1[i][j];
            arrD[i][j]=arr0[i][j]+arr1[i][j]+arr2[i][j]+arr3[i][j];
            if (arrA[i][j]==0){
            phase[i][j]=1.570796;
            }
            else{
            phase[i][j]=atan((arrB[i][j])/(arrA[i][j]));
            }
        }
    }

        for (int i=0;i<rows;i++){
        free(arrA[i]);
        free(arrB[i]);
        free(arrD[i]);
    }
    free(arrA);
    free(arrB);
    free(arrD);
    return phase;
}

double **ProcessImages(){
    int rows=480;
    int col= 640;
    int **arr0;
    int **arr1;
    int **arr2;
    int **arr3;
    double **phase;
    arr0=ConvertDimension("image0.pgm", 0.5);
    arr1=ConvertDimension("image1.pgm",0);
    arr2=ConvertDimension("image2.pgm",0.5);
    arr3=ConvertDimension("image3.pgm",1);

    phase=CalculatePhase(arr0, arr1, arr2, arr3, rows, col);
    printf("Phase calculated\n");

        for(int x = 0; x < rows; x++) { //Debugging
        for(int y = 0; y < col; y++) {
            printf("%f ", phase[x][y]);
        }
    printf("\n");
    }
    /*
    After the arrays are used, we should free them from memory
    */

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
    return phase;
}

//This could be used to exit out of the program
//bool sig_caught=false;
//
//void signal_handler(int sig)
//{
//  if (sig == SIGINT) {
//    sig_caught=true;
//  }
//}
//
//int main()
//{
//  if (signal(SIGINT, signal_handler) == SIG_ERR) {
//    fprintf(stderr, "signal func registration failed\n");
//    return 1;
//  }
//
//  for (;;) {
//    if (sig_caught) {
//      printf("Requested an exit to the loop \n");
//      return 0;
//    }
//  }
//
//  return 0;
//}
