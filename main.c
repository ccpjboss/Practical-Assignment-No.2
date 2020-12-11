#include <stdio.h>
#include <float.h>
#include <stdlib.h>

struct t_point_cloud
{
    double *x, *y, *z; /* Points coordinates */
    int npoints;       /* Number of points */
};

struct t_point_cloud points[3]; /* 3 arrays com os as structs das coordenadas */

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

int main(int argc, char const *argv[])
{
    /* Name of the files */
    char *file_name1 = "point_cloud1.txt";
    char *file_name2 = "point_cloud2.txt";
    char *file_name3 = "point_cloud3.txt";

    /* gets the number os coordinates presented on the files */
    for (int i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            points[i].npoints = getNPoints(file_name1);
            printf("%s: %d\n", file_name1, points[i].npoints);
        }
        if (i == 1)
        {
            points[i].npoints = getNPoints(file_name2);
            printf("%s: %d\n", file_name2, points[i].npoints);
        }
        if (i == 2)
        {
            points[i].npoints = getNPoints(file_name3);
            printf("%s: %d\n", file_name3, points[i].npoints);
        }
    }

    /* Allocates the memory for the coordinates */
    for (int i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            points[i].x = (double *)malloc(points[i].npoints * sizeof(double));
            points[i].y = (double *)malloc(points[i].npoints * sizeof(double));
            points[i].z = (double *)malloc(points[i].npoints * sizeof(double));
        }
        if (i == 1)
        {
            points[i].x = (double *)malloc(points[i].npoints * sizeof(double));
            points[i].y = (double *)malloc(points[i].npoints * sizeof(double));
            points[i].z = (double *)malloc(points[i].npoints * sizeof(double));
        }
        if (i == 2)
        {
            points[i].x = (double *)malloc(points[i].npoints * sizeof(double));
            points[i].y = (double *)malloc(points[i].npoints * sizeof(double));
            points[i].z = (double *)malloc(points[i].npoints * sizeof(double));
        }
    }

    /* Loads the data structs with the file content */
    loadFile(file_name1, 0);
    loadFile(file_name2, 1);
    loadFile(file_name3, 2);
    getchar();

    double avgX = getAvgX(1);
    double avgY = getAvgY(1);
    double avgZ = getAvgZ(1);

    printf("Avg X: %lf\n",avgX);
    printf("Avg Y: %lf\n",avgY);
    printf("Avg Z: %lf\n",avgZ);

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
        printf("%lf %lf %lf\n", *(points[i].x), *(points[i].y), *(points[i].z));
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
        if (*(points[n].x) == 0)
        {
            printf("zero\n");
            return;
        }
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
    
    return sum/points[n].npoints;
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

    return sum/points[n].npoints;
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

    return sum/points[n].npoints;
}