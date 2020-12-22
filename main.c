/* 
 *To run the code use: gcc * main.c -g -Wall -lm
 */
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>
#include "timespec.h"
#include "grid.h"

//Struct with the input of the thread function call
struct threadInput
{
    int idx;
    int task;
    struct timespec period;
    struct timespec start;
    struct timespec end;
};

struct t_point_cloud points[3]; /* 3 arrays com os as structs das coordenadas */
pthread_mutex_t lock;           /* Semaphore to lock global point cloud */
int myIdx = 0;                  /* Variable that stores the idx of the file read */

int getNPoints(char *filename);
void resetPointers(int j);
void loadFile(char *filename, int i);
void readStruct(int n);
double *getMinX(int n);
double *getMinY(int n);
double *getMinZ(int n);
double *getMaxX(int n);
double *getMaxY(int n);
double *getMaxZ(int n);
double getAvgX(int n);
double getAvgY(int n);
double getAvgZ(int n);
double getDevX(double avg, int n);
double getDevY(double avg, int n);
double getDevZ(double avg, int n);
void freePointCloud(struct grid *g);
void task1(int n);
void task3(int n);
void preProcessing(struct t_point_cloud *ptr);
void preProcessing2(int n);
void *performWork(void *input);

pthread_t thread[3];

int main(int argc, char const *argv[])
{
    struct timespec start, finish;
    struct threadInput input[3];
    int periodos = 1000000;

    //Prevents the memory from being paged to the swap area
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
        perror("mklockall");
    }

    /* Setting the start time and finish time */
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) //Gets the universal start time
    {
        perror("clock_gettime(start)");
    }

    start = timeSum(start, timespecFormat((int)2, 0)); //Adds 3 seconds to the start time to make sure that all the thread are created
    finish = timeSum(start, timespecFormat((int)10, 0));

    for (int i = 0; i < 3; i++)
    {
        input[i].task = i;
        input[i].period = timespecFormat(0, periodos);
        input[i].start = start;
        input[i].end = finish;
    }
    struct timespec p;
    p=timespecFormat(0,periodos);

    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            if (pthread_create(&(thread[i]), NULL, performWork, &(input[i])) != 0)
            {
                perror("pthread_create");
            }

            if (pthread_join(thread[i], NULL) != 0)
            {
                perror("thread join");
            }
        }
        if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &p, NULL) != 0)
        {
            perror("nanosleep");
            pthread_exit(NULL);
        }
        myIdx++;
    }
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
void loadFile(char *filename, int i)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("readFile->Opening file...");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%lf%lf%lf", points[i].x, points[i].y, points[i].z) != EOF)
    {
        points[i].x++;
        points[i].y++;
        points[i].z++;
    }

    /* Resets the pointers to the initial location */
    points[i].x = points[i].x - points[i].npoints;
    points[i].y = points[i].y - points[i].npoints;
    points[i].z = points[i].z - points[i].npoints;

    fclose(file);
}

/* Use this for debug */
void readStruct(int n)
{
    for (int i = 0; i < points[n].npoints; i++)
    {
        printf("%lf %lf %lf\n", *(points[n].x), *(points[n].y), *(points[n].z));
        points[n].x++;
        points[n].y++;
        points[n].z++;
    }

    /* Resets the pointers to the initial location */
    points[n].x = points[n].x - points[n].npoints;
    points[n].y = points[n].y - points[n].npoints;
    points[n].z = points[n].z - points[n].npoints;
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
double *getMinX(int n)
{
    double min_value = DBL_MAX;   /* High value so it gets replaced */
    double *min_value_ptr = NULL; /* Pointer pointing to the lowerst x pointer */
    double *cur = points[n].x;    /* Double pointer to search the x  points */

    for (int i = 0; i < points[n].npoints; i++)
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
double *getMinY(int n)
{
    double min_value = DBL_MAX;
    double *min_value_ptr = NULL;
    double *cur = points[n].y; /* Double pointer to search the x  points */

    for (int i = 0; i < points[n].npoints; i++)
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
double *getMinZ(int n)
{
    double min_value = DBL_MAX;
    double *min_value_ptr = NULL;
    double *cur = points[n].z; /* Double pointer to search the x  points */

    for (int i = 0; i < points[n].npoints; i++)
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
double *getMaxX(int n)
{
    double max_value = DBL_MIN;
    double *max_value_ptr = NULL;
    double *cur = points[n].x; /* Double pointer to search the x  points */

    for (int i = 0; i < points[n].npoints; i++)
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
double *getMaxY(int n)
{
    double max_value = DBL_MIN;
    double *max_value_ptr = NULL;
    double *cur = points[n].y; /* Double pointer to search the y points */

    for (int i = 0; i < points[n].npoints; i++)
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
double *getMaxZ(int n)
{
    double max_value = DBL_MIN;
    double *max_value_ptr = NULL;
    double *cur = points[n].z; /* Double pointer to search the Z points */

    for (int i = 0; i < points[n].npoints; i++)
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
double getAvgX(int n)
{
    double *cur = points[n].x; /* Double pointer to search the x points */
    double sum = 0;

    for (int i = 0; i < points[n].npoints; i++)
    {
        sum = sum + *(cur);
        cur = cur + 1;
    }

    return sum / points[n].npoints;
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
double getAvgY(int n)
{
    double *cur = points[n].y; /* Double pointer to search the y points */
    double sum = 0;

    for (int i = 0; i < points[n].npoints; i++)
    {
        sum = sum + *(cur);
        cur = cur + 1;
    }

    return sum / points[n].npoints;
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
double getAvgZ(int n)
{
    double *cur = points[n].z; /* Double pointer to search the z points */
    double sum = 0;

    for (int i = 0; i < points[n].npoints; i++)
    {
        sum = sum + *(cur);
        cur = cur + 1;
    }

    return sum / points[n].npoints;
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
double getDevX(double avg, int n)
{
    double *cur = points[n].x; /* Double pointer to search the x points */
    double sum = 0;
    double variance = 0;

    for (int i = 0; i < points[n].npoints; i++)
    {
        sum = sum + pow(*(cur)-avg, 2);
        cur = cur + 1;
    }

    variance = sum / points[n].npoints;
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
double getDevY(double avg, int n)
{
    double *cur = points[n].y; /* Double pointer to search the y points */
    double sum = 0;
    double variance = 0;

    for (int i = 0; i < points[n].npoints; i++)
    {
        sum = sum + pow(*(cur)-avg, 2);
        cur = cur + 1;
    }

    variance = sum / points[n].npoints;
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
double getDevZ(double avg, int n)
{
    double *cur = points[n].z; /* Double pointer to search the z points */
    double sum = 0;
    double variance = 0;

    for (int i = 0; i < points[n].npoints; i++)
    {
        sum = sum + pow(*(cur)-avg, 2);
        cur = cur + 1;
    }

    variance = sum / points[n].npoints;
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
        points[n].npoints = getNPoints(file_name1);
    if (n == 1)
        points[n].npoints = getNPoints(file_name2);
    if (n == 2)
        points[n].npoints = getNPoints(file_name3);
    printf("npoints: %d\n", points[n].npoints);
    /* Allocates the memory for the coordinates */
    points[n].x = (double *)malloc(points[n].npoints * sizeof(double));
    points[n].y = (double *)malloc(points[n].npoints * sizeof(double));
    points[n].z = (double *)malloc(points[n].npoints * sizeof(double));

    /* Loads the data structs with the file content */
    if (n == 0)
        loadFile(file_name1, 0);
    if (n == 1)
        loadFile(file_name2, 1);
    if (n == 2)
        loadFile(file_name3, 2);
    /* UNLOCK */
    pthread_mutex_unlock(&lock);
}

void preProcessing2(int n)
{
    printf("Doing pre processing...\n");
    /* LOCK */
    pthread_mutex_lock(&lock);
    struct t_point_cloud *ptr;
    ptr = &points[n];
    int a, b, c, d, e, f;

    for (a = 0; a < ptr->npoints; a++)
    {

        if (*(ptr->x) < 0)
        {

            for (b = a + 1; b < ptr->npoints; b++)
            {

                *(ptr->x) = *(ptr->x + 1);
                *(ptr->y) = *(ptr->y + 1);
                *(ptr->z) = *(ptr->z + 1);

                ptr->x = ptr->x + 1;
                ptr->y = ptr->y + 1;
                ptr->z = ptr->z + 1;
            }

            ptr->npoints = ptr->npoints - 1;

            ptr->x = ptr->x - ptr->npoints;
            ptr->y = ptr->y - ptr->npoints;
            ptr->z = ptr->z - ptr->npoints;

            a = 0;
        }

        ptr->x = ptr->x + 1;
        ptr->y = ptr->y + 1;
        ptr->z = ptr->z + 1;
    }

    /* Resets the pointers to the initial location */
    ptr->x = ptr->x - ptr->npoints;
    ptr->y = ptr->y - ptr->npoints;
    ptr->z = ptr->z - ptr->npoints;

    for (c = 0; c < ptr->npoints; c++)
    {

        if (*(ptr->x) <= 2 && *(ptr->y) >= -1 && *(ptr->y) <= 1)
        {
            for (d = c + 1; d < ptr->npoints; d++)
            {

                *(ptr->x) = *(ptr->x + 1);
                *(ptr->y) = *(ptr->y + 1);
                *(ptr->z) = *(ptr->z + 1);

                ptr->x = ptr->x + 1;
                ptr->y = ptr->y + 1;
                ptr->z = ptr->z + 1;
            }

            ptr->npoints = ptr->npoints - 1;

            ptr->x = ptr->x - ptr->npoints;
            ptr->y = ptr->y - ptr->npoints;
            ptr->z = ptr->z - ptr->npoints;

            c = 0;
        }

        ptr->x = ptr->x + 1;
        ptr->y = ptr->y + 1;
        ptr->z = ptr->z + 1;
    }

    /* Resets the pointers to the initial location */
    ptr->x = ptr->x - ptr->npoints;
    ptr->y = ptr->y - ptr->npoints;
    ptr->z = ptr->z - ptr->npoints;

    for (e = 0; e < ptr->npoints; e++)
    {

        if (*(ptr->x) > 30 || *(ptr->y) < -10 || *(ptr->y) > 10)
        {
            for (f = e + 1; f < ptr->npoints; f++)
            {

                *(ptr->x) = *(ptr->x + 1);
                *(ptr->y) = *(ptr->y + 1);
                *(ptr->z) = *(ptr->z + 1);

                ptr->x = ptr->x + 1;
                ptr->y = ptr->y + 1;
                ptr->z = ptr->z + 1;
            }

            ptr->npoints = ptr->npoints - 1;

            ptr->x = ptr->x - ptr->npoints;
            ptr->y = ptr->y - ptr->npoints;
            ptr->z = ptr->z - ptr->npoints;

            e = 0;
        }

        ptr->x = ptr->x + 1;
        ptr->y = ptr->y + 1;
        ptr->z = ptr->z + 1;
    }

    /* Resets the pointers to the initial location */
    ptr->x = ptr->x - ptr->npoints;
    ptr->y = ptr->y - ptr->npoints;
    ptr->z = ptr->z - ptr->npoints;

    pthread_mutex_unlock(&lock);
    /* UNLOCK */
    printf("npoints: %d\n", points[n].npoints);
}

void preProcessing(struct t_point_cloud *ptr)
{
    /* LOCK */
    pthread_mutex_lock(&lock);
    int a, b, c, d, e, f;

    for (a = 0; a < ptr->npoints; a++)
    {

        if (*(ptr->x) < 0)
        {

            for (b = a + 1; b < ptr->npoints; b++)
            {

                *(ptr->x) = *(ptr->x + 1);
                *(ptr->y) = *(ptr->y + 1);
                *(ptr->z) = *(ptr->z + 1);

                ptr->x = ptr->x + 1;
                ptr->y = ptr->y + 1;
                ptr->z = ptr->z + 1;
            }

            ptr->npoints = ptr->npoints - 1;

            ptr->x = ptr->x - ptr->npoints;
            ptr->y = ptr->y - ptr->npoints;
            ptr->z = ptr->z - ptr->npoints;

            a = 0;
        }

        ptr->x = ptr->x + 1;
        ptr->y = ptr->y + 1;
        ptr->z = ptr->z + 1;
    }

    /* Resets the pointers to the initial location */
    ptr->x = ptr->x - ptr->npoints;
    ptr->y = ptr->y - ptr->npoints;
    ptr->z = ptr->z - ptr->npoints;

    for (c = 0; c < ptr->npoints; c++)
    {

        if (*(ptr->x) <= 2 && *(ptr->y) >= -1 && *(ptr->y) <= 1)
        {
            for (d = c + 1; d < ptr->npoints; d++)
            {

                *(ptr->x) = *(ptr->x + 1);
                *(ptr->y) = *(ptr->y + 1);
                *(ptr->z) = *(ptr->z + 1);

                ptr->x = ptr->x + 1;
                ptr->y = ptr->y + 1;
                ptr->z = ptr->z + 1;
            }

            ptr->npoints = ptr->npoints - 1;

            ptr->x = ptr->x - ptr->npoints;
            ptr->y = ptr->y - ptr->npoints;
            ptr->z = ptr->z - ptr->npoints;

            c = 0;
        }

        ptr->x = ptr->x + 1;
        ptr->y = ptr->y + 1;
        ptr->z = ptr->z + 1;
    }

    /* Resets the pointers to the initial location */
    ptr->x = ptr->x - ptr->npoints;
    ptr->y = ptr->y - ptr->npoints;
    ptr->z = ptr->z - ptr->npoints;

    for (e = 0; e < ptr->npoints; e++)
    {

        if (*(ptr->x) > 30 || *(ptr->y) < -10 || *(ptr->y) > 10)
        {
            for (f = e + 1; f < ptr->npoints; f++)
            {

                *(ptr->x) = *(ptr->x + 1);
                *(ptr->y) = *(ptr->y + 1);
                *(ptr->z) = *(ptr->z + 1);

                ptr->x = ptr->x + 1;
                ptr->y = ptr->y + 1;
                ptr->z = ptr->z + 1;
            }

            ptr->npoints = ptr->npoints - 1;

            ptr->x = ptr->x - ptr->npoints;
            ptr->y = ptr->y - ptr->npoints;
            ptr->z = ptr->z - ptr->npoints;

            e = 0;
        }

        ptr->x = ptr->x + 1;
        ptr->y = ptr->y + 1;
        ptr->z = ptr->z + 1;
    }

    /* Resets the pointers to the initial location */
    ptr->x = ptr->x - ptr->npoints;
    ptr->y = ptr->y - ptr->npoints;
    ptr->z = ptr->z - ptr->npoints;
    pthread_mutex_unlock(&lock);
    /* UNLOCK */
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
    loadPointsGrid(&g, points[n]);
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
    struct timespec next;
    next = in->start;

    if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL) != 0)
    {
        perror("nanosleep");
        pthread_exit(NULL);
    }

    if (in->task == 0)
    {
        task1(myIdx);
    }
    if (in->task == 1)
    {
        preProcessing2(myIdx);
    }
    if (in->task == 2)
    {
        task3(myIdx);
    }

    pthread_exit(NULL);
}