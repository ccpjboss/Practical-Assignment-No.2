#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

#define SQSIDE 30 /* Side of the grid */
#define NSQUARES 9 /* Number of squares to divide the grid into */

/* Used for square coordinates */
struct coord
{
    double x; 
    double y;
};

/* Main square of the grid */
struct square
{
    struct coord top_left;
    double side_len;
};

/* Smaller squares that will contains the points */
struct sub_square
{
    struct coord top_left;
    double side_len;
    int id;
    double *x, *y, *z; /* This will need to be a dynamic array */
    int size;
    int npoints;
    bool safe;
};

/* Grid struct */
struct grid
{
    struct square main_sq;     /* Main square */
    struct sub_square *sub_sq; /* Array of sub squares that the grid contains */
    int nsquares;
};

struct t_point_cloud
{
    double *x, *y, *z; /* Points coordinates */
    int npoints;       /* Number of points */
};

void initCoord(struct coord *c, double x, double y);
void initGrid(struct grid *Grid, struct square sq);
void initSquare(struct square *sq, struct coord tl);
void printGrid(struct grid *g);
struct coord getTopLeft(struct sub_square *sq);
struct coord getTopRight(struct sub_square *sq);
struct coord getLowRight(struct sub_square *sq);
struct coord getLowLeft(struct sub_square *sq);
void initPoints(struct sub_square *sq, size_t initialSize);
void insertPoint(struct sub_square *sq, double x, double y, double z);
void freePoints(struct sub_square *sq);
void freeGrid(struct grid *g);
void loadPointsGrid(struct grid *g, struct t_point_cloud p);
void printPointsSQ(struct sub_square sq);
void printPointsGrid(struct grid g);
double getMaxZSQ(struct sub_square *sq);
double getMinZSQ(struct sub_square *sq);
void checkDriveSQ(struct sub_square *sq);
void insertPointsAddr(struct grid *g,struct t_point_cloud p);
void loadPointsGridAddr(struct grid *g,struct t_point_cloud p);
void eliminateNonDrivePoints(struct grid *g);