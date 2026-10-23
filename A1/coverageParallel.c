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


int main (int argc, char*argv[]){
    pthread_t *threads;
    threadArgs *tArgs;
    long N;
    long bSeed;
    long hits;
    long i;
    int err;
    struct timespec start;
    double elapsed;
    double fraction;

    if ((argc != 5) || (getNum(argv[2], &N) != 0 || N <= 0) || (getNum(argv[3], &numThreads) != 0 || numThreads <= 0) || (getNum(argv[4], &bSeed) != 0)){
        invalidArg();
    }

    if(loadFile(argv[1]) != 0){
        exit(1);
    }

    threads = malloc(numThreads * sizeof(pthread_t));
    partHits = malloc(numThreads * sizeof(long));

    // timing starts at thread creation
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < numThreads; i++){
        tArgs = malloc(sizeof(threadArgs));

        // every thread gets N / numThreads samples
        // the first N % numThreads threads get one extra!
        tArgs->numSamples = N / numThreads;
        if (i < N % numThreads){
            tArgs->numSamples = tArgs->numSamples + 1;
        }
        tArgs->seed = bSeed + i;
        tArgs->tid = i;

        err = pthread_create(&threads[i], NULL, threadFunction, tArgs);
        if (err != 0){
            fprintf(stderr, "error! cannot create thread, error: %d\n", err);
            exit(1);
        }
    }

    // wait for every thread to finish
    for (i = 0; i < numThreads; i++){
        pthread_join(threads[i], NULL);
    }

    // add up each thread's hits
    hits = 0;
    for (i = 0; i < numThreads; i++){
        hits = hits + partHits[i];
    }

    elapsed = getTime(start);

    fraction = 1.0 * hits / N;
    printf("SAMPLES: %ld\n", N);
    printf("HITS: %ld\n", hits);
    printf("COVERAGE: %.1f %.6f\n", fraction * fWidth * fHeight, fraction * 100.0);
    fprintf(stderr, "TIME: %.6f\n", elapsed);

    free(threads);
    free(partHits);
    free(nodes);
    return 0;
}

// each thread runs
void *threadFunction(void *arg){
    threadArgs *tArgs = arg;
    unsigned int seed = tArgs->seed;
    long localHits = 0;
    double px;
    double py;

    for (long i = 0; i < tArgs->numSamples; i++){
        px = rand_r(&seed) / (RAND_MAX + 1.0) * fWidth;
        py = rand_r(&seed) / (RAND_MAX + 1.0) * fHeight;
        if(covered(px, py)){
            localHits++;
        }
    }

    partHits[tArgs->tid] = localHits;

    free(tArgs);
    return NULL;
}

void invalidArg(void){
    fprintf(stderr, "error! invalid arguments\n");
    fprintf(stderr, "usage: ./coverageParallel <inputFile> <N> <p> <baseSeed>\n");
    fprintf(stderr, "N and p must be positive integers and baseSeed must be an integer!\n");
    exit(1);
}

// reads input file 
int loadFile(char *fileName){
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL){
        fprintf(stderr, "error! cannot open file %s\n", fileName);
        return 1;
    }

    if (fscanf(fp, "%lf %lf", &fWidth, &fHeight) != 2){
        fprintf(stderr, "error! malformed file (field size)\n");
        fclose(fp);
        return 1;
    }

    if (fscanf(fp, "%ld", &numNodes) != 1 || numNodes < 0){
        fprintf(stderr, "error! malformed file (node count)\n");
        fclose(fp);
        return 1;
    }

    nodes = malloc(numNodes * sizeof(node));
    for (long i = 0; i < numNodes; i++){
        if (fscanf(fp, "%lf %lf %lf", &nodes[i].x, &nodes[i].y, &nodes[i].r) != 3){
            fprintf(stderr, "error! malformed file (node %ld)\n", i);
            free(nodes);
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// converts str to long
int getNum(char *str, long *result){
    int i = 0;

    if (str[0] == '\0'){
        return 1;
    }

    while(str[i] != '\0'){
        if(str[i] < '0' || str[i] > '9'){
            return 1;
        }
        i++;
    }

    *result = atol(str);
    return 0;
}

int covered(double px, double py){
    double dx;
    double dy;
    for(long j = 0; j < numNodes; j++){
        dx = px - nodes[j].x;
        dy = py - nodes[j].y;
        if(dx * dx + dy * dy < nodes[j].r * nodes[j].r){
            return 1;
        }
    }
    return 0;
}

// this function is from class examples
double getTime(struct timespec start){
    struct timespec end;

    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}