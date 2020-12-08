#include <stdio.h>
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

    return 0;
}

/* Gets the number of points on the file
   char *filename -> name of the file */
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

/**
 * Loads the data struct with file content
 * 
 * char *filename -> file name
 * int i -> index of the data struct */

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
