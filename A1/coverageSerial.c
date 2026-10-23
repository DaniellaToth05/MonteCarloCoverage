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
void invalidArg(void);


int main (int argc, char*argv[]){
    long N;
    long bSeed;
    unsigned int seed;
    long hits;
    long i;
    double px;
    double py;
    struct timespec start;
    double elapsed;
    double fraction;

    if ((argc !=4) || (getNum(argv[2], &N) != 0 || N <= 0) || (getNum(argv[3], &bSeed) != 0)){
        invalidArg();
    }

    seed = bSeed;

    if(loadFile(argv[1]) != 0){
        exit(1);
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    hits = 0;
    for (i = 0; i < N; i++){
        px = rand_r(&seed) / (RAND_MAX + 1.0) * fWidth;
        py = rand_r(&seed) / (RAND_MAX + 1.0) * fHeight;
        if(covered(px , py)){
            hits++;
        }
    }

    elapsed = getTime(start);

    fraction = 1.0 * hits / N;
    printf("SAMPLES: %ld\n", N);
    printf("HITS: %ld\n", hits);
    printf("COVERAGE: %.1f %.6f\n", fraction * fWidth * fHeight, fraction * 100.0);
    fprintf(stderr, "TIME: %.6f\n", elapsed);

    free(nodes);
    return 0;
}

void invalidArg(void){
    fprintf(stderr, "error! invalid arguments\n");
    fprintf(stderr, "usage: ./coverageSerial <inputFile> <N> <baseSeed>\n");
    fprintf(stderr, "N must be positive integer and baseSeed must be an integer!\n");
    exit(1);
}