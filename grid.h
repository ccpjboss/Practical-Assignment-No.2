#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SQSIDE 30 /* Side of the grid */
#define NSQUARES 9 /* Numeber of squares to divide the grid into */

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
    double *x, *y, *z;
    int npoints;
};

/* Grid struct */
struct grid
{
    struct square main_sq;     /* Main square */
    struct sub_square *sub_sq; /* Array of sub squares that the grid contains */
    int nsquares;
};

void initCoord(struct coord *c, double x, double y);
void initGrid(struct grid *Grid, struct square sq);
void initSquare(struct square *sq, struct coord tl);
void printGrid(struct grid *g);