/* 
 *To run the code use: gcc grid.c main.c -g -Wall -lm -lpthread
 */

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>
#include "grid.h"
#include <time.h>
#include <unistd.h>

//Struct with the input of the thread function call
struct threadInput
{
    int idx;
    int task;
};
enum
{
    NS_PER_SECOND = 1000000000
};

struct t_point_cloud points; /* 3 arrays com os as structs das coordenadas */
pthread_mutex_t lock;        /* Semaphore to lock global point cloud */
int myIdx = 0;               /* Variable that stores the idx of the file read */
int c = 0;
int t1s = 0;
long int t1ns = 0;
int t2s = 0;
long int t2ns = 0;

int getNPoints(char *filename);
void resetPointers(int j);
void loadFile(char *filename);
void readStruct();
double *getMinX();
double *getMinY();
double *getMinZ();
double *getMaxX();
double *getMaxY();
double *getMaxZ();
double getAvgX();
double getAvgY();
double getAvgZ();
double getDevX(double avg);
double getDevY(double avg);
double getDevZ(double avg);
void freePointCloud(struct grid *g);
void task1(int n);
void task2(int n);
void task3(int n);
void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td);
void *performWork(void *input);
void resetPointCloud();
void deleteIdx(int idx);

pthread_t thread[3];

int main(int argc, char const *argv[])
{
    struct timespec act1, act2, delta1;
    struct threadInput input[3];

    for (int i = 0; i < 3; i++)
    {
        input[i].task = i;
    }


    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init has failed\n");
        return 1;
    }

    //for (int j = 0; j < 3; j++)
    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            if (pthread_create(&(thread[i]), NULL, performWork, &(input[i])) != 0)
            {
                perror("pthread_create");
            }

            if (i == 0 && myIdx == 0)
            {
                clock_gettime(CLOCK_REALTIME, &act1);
            }

            if (i == 0 && myIdx == 1)
            {
                clock_gettime(CLOCK_REALTIME, &act2);
                sub_timespec(act1, act2, &delta1);
                printf("Time Activation1: %d.%.9ld\n", (int)delta1.tv_sec, delta1.tv_nsec);
            }
        }

        for (int i = 0; i < 3; i++)
        {
            if (pthread_join(thread[i], NULL) != 0)
            {
                perror("thread join");
            }
        }

        resetPointCloud();
        myIdx++;
        if (myIdx == 3)
            myIdx = 0;
    }

    pthread_mutex_destroy(&lock);
    return 0;
}

/*
 * Function:  getNPoints 
 * --------------------
 *  get the number of points in the file, used to load points.npoints
 * 
 *  filename: name of the file containing the points
 * 
 *  returns: the number of points.
 */
int getNPoints(char *filename)
{
    FILE *file = fopen(filename, "r");
    char c;
    int n = 0;

    if (file == NULL)
    {
        perror("getNPoints->Opening file...");
        exit(EXIT_FAILURE);
    }

    for (c = getc(file); c != EOF; c = getc(file))
        if (c == '\n')
            n = n + 1;

    fclose(file);

    return n;
}

/*
 * Function:  loadFile 
 * --------------------
 *  load the global point struct with the points in the corresponding file 
 * 
 *  i: the index of the points struct
 *  filename: name of the file containing the points
 * 
 *  returns: void
 */
void loadFile(char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("readFile->Opening file...");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%lf%lf%lf", points.x, points.y, points.z) != EOF)
    {
        points.x++;
        points.y++;
        points.z++;
    }

    /* Resets the pointers to the initial location */
    points.x = points.x - points.npoints;
    points.y = points.y - points.npoints;
    points.z = points.z - points.npoints;

    fclose(file);
}

/* Use this for debug */
void readStruct()
{
    for (int i = 0; i < points.npoints; i++)
    {
        printf("%lf %lf %lf\n", *(points.x), *(points.y), *(points.z));
        points.x++;
        points.y++;
        points.z++;
    }

    /* Resets the pointers to the initial location */
    points.x = points.x - points.npoints;
    points.y = points.y - points.npoints;
    points.z = points.z - points.npoints;
}

/*
 * Function:  getMinX 
 * --------------------
 *  gets the lowest value of x
 * 
 *  n: index of points struct
 * 
 *  returns: a pointer pointing to the lowest value of x
 */
double *getMinX()
{
    double min_value = DBL_MAX;   /* High value so it gets replaced */
    double *min_value_ptr = NULL; /* Pointer pointing to the lowerst x pointer */
    double *cur = points.x;       /* Double pointer to search the x  points */

    for (int i = 0; i < points.npoints; i++)
    {
        if (*(cur) < min_value)
        {
            min_value = *(cur);
            min_value_ptr = cur;
        }
        cur = cur + 1;
    }
    return min_value_ptr;
}

/*
 * Function:  getMinY 
 * --------------------
 *  gets the lowest value of y
 * 
 *  n: index of points struct
 * 
 *  returns: a pointer pointing to the lowest value of y
 */
double *getMinY()
{
    double min_value = DBL_MAX;
    double *min_value_ptr = NULL;
    double *cur = points.y; /* Double pointer to search the x  points */

    for (int i = 0; i < points.npoints; i++)
    {
        if (*(cur) < min_value)
        {
            min_value = *(cur);
            min_value_ptr = cur;
        }
        cur = cur + 1;
    }

    return min_value_ptr;
}

/*
 * Function:  getMinZ 
 * --------------------
 *  gets the lowest value of Z
 * 
 *  n: index of points struct
 * 
 *  returns: a pointer pointing to the lowest value of Z
 */
double *getMinZ()
{
    double min_value = DBL_MAX;
    double *min_value_ptr = NULL;
    double *cur = points.z; /* Double pointer to search the x  points */

    for (int i = 0; i < points.npoints; i++)
    {
        if (*(cur) < min_value)
        {
            min_value = *(cur);
            min_value_ptr = cur;
        }
        cur = cur + 1;
    }

    return min_value_ptr;
}

/*
 * Function:  getMaxX 
 * --------------------
 *  gets the max value of X
 * 
 *  n: index of points struct
 * 
 *  returns: a pointer pointing to the max value of X
 */
double *getMaxX()
{
    double max_value = DBL_MIN;
    double *max_value_ptr = NULL;
    double *cur = points.x; /* Double pointer to search the x  points */

    for (int i = 0; i < points.npoints; i++)
    {
        if (*(cur) > max_value)
        {
            max_value = *(cur);
            max_value_ptr = cur;
        }
        cur = cur + 1;
    }

    return max_value_ptr;
}

/*
 * Function:  getMaxY 
 * --------------------
 *  gets the max value of Y
 * 
 *  n: index of points struct
 * 
 *  returns: a pointer pointing to the max value of Y
 */
double *getMaxY()
{
    double max_value = DBL_MIN;
    double *max_value_ptr = NULL;
    double *cur = points.y; /* Double pointer to search the y points */

    for (int i = 0; i < points.npoints; i++)
    {
        if (*(cur) > max_value)
        {
            max_value = *(cur);
            max_value_ptr = cur;
        }
        cur = cur + 1;
    }

    return max_value_ptr;
}

/*
 * Function:  getMaxZ 
 * --------------------
 *  gets the max value of Z
 * 
 *  n: index of points struct
 * 
 *  returns: a pointer pointing to the max value of Z
 */
double *getMaxZ()
{
    double max_value = DBL_MIN;
    double *max_value_ptr = NULL;
    double *cur = points.z; /* Double pointer to search the Z points */

    for (int i = 0; i < points.npoints; i++)
    {
        if (*(cur) > max_value)
        {
            max_value = *(cur);
            max_value_ptr = cur;
        }
        cur = cur + 1;
    }

    return max_value_ptr;
}

/*
 * Function:  getAvgX 
 * --------------------
 *  get the average of X
 * 
 *  n: index of points struct
 * 
 *  returns: the average value of the points
 */
double getAvgX()
{
    double *cur = points.x; /* Double pointer to search the x points */
    double sum = 0;

    for (int i = 0; i < points.npoints; i++)
    {
        sum = sum + *(cur);
        cur = cur + 1;
    }

    return sum / points.npoints;
}

/*
 * Function:  getAvgY 
 * --------------------
 *  get the average of Y
 * 
 *  n: index of points struct
 * 
 *  returns: the average value of the points
 */
double getAvgY()
{
    double *cur = points.y; /* Double pointer to search the y points */
    double sum = 0;

    for (int i = 0; i < points.npoints; i++)
    {
        sum = sum + *(cur);
        cur = cur + 1;
    }

    return sum / points.npoints;
}

/*
 * Function:  getAvgZ 
 * --------------------
 *  get the average of Z
 * 
 *  n: index of points struct
 * 
 *  returns: the average value of the points
 */
double getAvgZ()
{
    double *cur = points.z; /* Double pointer to search the z points */
    double sum = 0;

    for (int i = 0; i < points.npoints; i++)
    {
        sum = sum + *(cur);
        cur = cur + 1;
    }

    return sum / points.npoints;
}

/*
 * Function:  getDevX 
 * --------------------
 *  gets the deviation of a set of points, using the formula presented in:
 *  https://en.wikipedia.org/wiki/Standard_deviation
 * 
 *  avg: average of the set of points
 *  n: index of points struct
 * 
 *  returns: returns the standart deviation of the points 
 */
double getDevX(double avg)
{
    double *cur = points.x; /* Double pointer to search the x points */
    double sum = 0;
    double variance = 0;

    for (int i = 0; i < points.npoints; i++)
    {
        sum = sum + pow(*(cur)-avg, 2);
        cur = cur + 1;
    }

    variance = sum / points.npoints;
    return sqrt(variance);
}

/*
 * Function:  getDevY 
 * --------------------
 *  gets the deviation of a set of points, using the formula presented in:
 *  https://en.wikipedia.org/wiki/Standard_deviation
 * 
 *  avg: average of the set of points
 *  n: index of points struct
 * 
 *  returns: returns the standart deviation of the points 
 */
double getDevY(double avg)
{
    double *cur = points.y; /* Double pointer to search the y points */
    double sum = 0;
    double variance = 0;

    for (int i = 0; i < points.npoints; i++)
    {
        sum = sum + pow(*(cur)-avg, 2);
        cur = cur + 1;
    }

    variance = sum / points.npoints;
    return sqrt(variance);
}

/*
 * Function:  getDevZ 
 * --------------------
 *  gets the deviation of a set of points, using the formula presented in:
 *  https://en.wikipedia.org/wiki/Standard_deviation
 * 
 *  avg: average of the set of points
 *  n: index of points struct
 * 
 *  returns: returns the standart deviation of the points 
 */
double getDevZ(double avg)
{
    double *cur = points.z; /* Double pointer to search the z points */
    double sum = 0;
    double variance = 0;

    for (int i = 0; i < points.npoints; i++)
    {
        sum = sum + pow(*(cur)-avg, 2);
        cur = cur + 1;
    }

    variance = sum / points.npoints;
    return sqrt(variance);
}

void task1(int n)
{
    /* Name of the files */
    char *file_name1 = "point_cloud1.txt";
    char *file_name2 = "point_cloud2.txt";
    char *file_name3 = "point_cloud3.txt";
    printf("Loading points to the point cloud\n");
    /* LOCK */
    pthread_mutex_lock(&lock);

    /* gets the number os coordinates presented on the files */
    if (n == 0)
        points.npoints = getNPoints(file_name1);
    if (n == 1)
        points.npoints = getNPoints(file_name2);
    if (n == 2)
        points.npoints = getNPoints(file_name3);
    printf("npoints: %d\n", points.npoints);

    /* Allocates the memory for the coordinates */
    points.x = (double *)malloc(points.npoints * sizeof(double));
    points.y = (double *)malloc(points.npoints * sizeof(double));
    points.z = (double *)malloc(points.npoints * sizeof(double));

    /* Loads the data structs with the file content */
    if (n == 0)
        loadFile(file_name1);
    if (n == 1)
        loadFile(file_name2);
    if (n == 2)
        loadFile(file_name3);

    double *min = getMinX();
    double *max = getMaxX();
    double avg = getAvgX();
    double std = getDevX(avg);

    printf("X values for file: %d\n", n + 1);
    printf("Min: %lf\t Max: %lf\t Avg: %lf\tStd: %lf\n", *min, *max, avg, std);

    min = getMinY();
    max = getMaxY();
    avg = getAvgY();
    std = getDevY(avg);

    printf("Y values for file: %d\n", n + 1);
    printf("Min: %lf\t Max: %lf\t Avg: %lf\tStd: %lf\n", *min, *max, avg, std);

    min = getMinZ();
    max = getMaxZ();
    avg = getAvgZ();
    std = getDevZ(avg);

    printf("Z values for file: %d\n", n + 1);
    printf("Min: %lf\t Max: %lf\t Avg: %lf\tStd: %lf\n", *min, *max, avg, std);

    /* UNLOCK */
    pthread_mutex_unlock(&lock);
}

void task2(int n)
{
    printf("Doing pre processing...\n");
    /* LOCK */
    pthread_mutex_lock(&lock);
    int a;

    for (a = 0; a < points.npoints; a++)
    {
        if (points.x[a] < 0 || (points.x[a] <= 2 && points.y[a] >= -1 && points.y[a] <= 1) || points.x[a] > 30 || points.y[a] < -10 || points.y[a] > 10)
        {
            deleteIdx(a + 1);

            a--;
        }
    }

    pthread_mutex_unlock(&lock);
    /* UNLOCK */
    printf("npoints: %d\n", points.npoints);
}

void task3(int n)
{
    struct coord tl;
    struct square sq;
    struct grid g;

    double sqrx = 0;
    double sqry = 15;

    initCoord(&tl, sqrx, sqry);
    initSquare(&sq, tl);
    initGrid(&g, sq);
    printGrid(&g);
    printf("Loading the points to the grid...\n");
    /* lock */
    pthread_mutex_lock(&lock);
    loadPointsGrid(&g, points);
    pthread_mutex_unlock(&lock);
    /* unlock */
    printf("Checking for road squares\n");
    for (int i = 0; i < g.nsquares; i++)
    {
        checkDriveSQ(&g.sub_sq[i]);
    }

    eliminateNonDrivePoints(&g);

    printf("After deleting non drivable squares...\n");

    for (int i = 0; i < g.nsquares; i++)
    {
        checkDriveSQ(&g.sub_sq[i]);
    }

    freeGrid(&g);
}

void *performWork(void *input)
{
    struct threadInput *in = (struct threadInput *)input;
    struct timespec start, finish,  delta;


    if (in->task == 0)
    {
        clock_gettime(CLOCK_REALTIME, &start);
        task1(myIdx);
        clock_gettime(CLOCK_REALTIME, &finish);
        sub_timespec(start, finish, &delta);
        printf("Time task1: %d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);
    }
    if (in->task == 1)
    {
        clock_gettime(CLOCK_REALTIME, &start);
        task2(myIdx);
        clock_gettime(CLOCK_REALTIME, &finish);
        sub_timespec(start, finish, &delta);
        printf("Time task2: %d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);
    }
    if (in->task == 2)
    {
        clock_gettime(CLOCK_REALTIME, &start);
        task3(myIdx);
        clock_gettime(CLOCK_REALTIME, &finish);
        sub_timespec(start, finish, &delta);
        printf("Time task3: %d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);
    }

    pthread_exit(NULL);
}

void resetPointCloud()
{
    free(points.x);
    free(points.y);
    free(points.z);

    points.x = NULL;
    points.y = NULL;
    points.z = NULL;

    points.npoints = 0;
}

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

void deleteIdx(int idx)
{
    /* Gets the address of the number to eliminate */
    double *curx = points.x + idx - 1;
    double *cury = points.y + idx - 1;
    double *curz = points.z + idx - 1;

    /* Saves the start address to come back to the start */
    double *startx = points.x;
    double *starty = points.y;
    double *startz = points.z;

    /* Puts the last value of the struct into the point to eliminate */
    *curx = *(points.x + points.npoints - 1);
    *cury = *(points.y + points.npoints - 1);
    *curz = *(points.z + points.npoints - 1);

    /* Goes to the last element of the struct */
    points.x = points.x + points.npoints - 1;
    points.y = points.y + points.npoints - 1;
    points.z = points.z + points.npoints - 1;

    /* Puts the last value equal to NULL. This is not a problem because the last number is already saved to on the address to eliminate */
    points.x = NULL;
    points.y = NULL;
    points.z = NULL;

    /* Goes back to the start */
    points.x = startx;
    points.y = starty;
    points.z = startz;

    /* Updates the total of points */
    points.npoints--;
}