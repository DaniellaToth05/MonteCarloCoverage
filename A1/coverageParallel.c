/*
    ~parallel version
    daniella toth
    1261398
    cis3090 a1
*/ 

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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
long numThreads;
long *partHits;

typedef struct{
    long numSamples;
    unsigned int seed;
    long tid;
} threadArgs;

// prototypes
void *threadFunction(void *arg);
int loadFile(char *fileName);
int getNum(char *str, long *result);
int covered(double px, double py);
double getTime(struct timespec start);
void invalidArg(void);
