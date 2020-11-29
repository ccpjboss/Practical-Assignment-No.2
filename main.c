#include <stdio.h>
#include <stdlib.h>

struct t_point_cloud
{
    double *x, *y, *z;
    int npoints;
};

int getNPoints(char *filename);

int main(int argc, char const *argv[])
{
    char *file_name1 = "point_cloud1.txt";
    char *file_name2 = "point_cloud2.txt";
    char *file_name3 = "point_cloud3.txt";
    struct t_point_cloud points[3];

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

    return 0;
}

int getNPoints(char *filename)
{
    FILE *file = fopen(filename, "r");
    char c;
    int n;

    if (file == NULL)
    {
        perror("Opening file...");
        exit(EXIT_FAILURE);
    }

    for (c = getc(file); c != EOF; c = getc(file))
        if (c == '\n')
            n = n + 1;

    fclose(file);

    return n;
}