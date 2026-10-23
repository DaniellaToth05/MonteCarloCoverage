/*
    ~serial version
    daniella toth
    1261398
    cis3090 a1
*/ 

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    double x;
    double y;
    double r;
} node;

// globals
double fWidth;
double fHeight;
long numNodes;
node *nodes;

// prototypes
int loadFile(char *fileName);
int getNum(char *str, long *result);
int covered(double px, double py);
double getTime(struct timespec start);

int main (int argc, char*argv[]){


}