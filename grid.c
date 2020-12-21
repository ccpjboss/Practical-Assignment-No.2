#include "grid.h"

/*
 * Function:  initCoord 
 * --------------------
 *  initializes the coordinate struct
 * 
 *  c: pointer to the struct of coord
 *  x: x value
 *  y: y value
 */
void initCoord(struct coord *c, double x, double y)
{
    c->x = x;
    c->y = y;
}

/*
 * Function:  initSquare 
 * --------------------
 *  initializes the square struct
 * 
 * sq: pointer to the struct square created
 * tl: struct coord wiht the coordinates of the top left ver
 */
void initSquare(struct square *sq, struct coord tl)
{
    sq->side_len = SQSIDE;
    sq->top_left = tl;
}

/*
 * Function:  initGrid 
 * --------------------
 *  initializes the grid struct
 * 
 * g: pointer to the struct grid created
 * sq: struct square with the main square forming the grid
 */
void initGrid(struct grid *g, struct square sq)
{
    g->sub_sq = (struct sub_square *)malloc(NSQUARES * sizeof(struct sub_square)); /* Allocates memoory for the sub squares */
    g->main_sq = sq;

    int id = 1;                             /* Identifier of the sub square */
    int div_per_side = sqrt(NSQUARES);      /* Number of divisions of the side of the square */
    double sub_len = SQSIDE / div_per_side; /* Len of the sub square */

    /* Coord of the top left for the sub squares, this gets changed in the for loops */
    double x_coord = sq.top_left.x;
    double y_coord = sq.top_left.y;

    for (int x = 0; x < div_per_side; x++, x_coord += sub_len)
    {
        for (int y = 0; y < div_per_side; y++, y_coord -= sub_len)
        {
            struct coord sub_sq_coord;
            sub_sq_coord.x = x_coord;
            sub_sq_coord.y = y_coord;

            g->nsquares = NSQUARES;
            g->sub_sq->top_left.x = sub_sq_coord.x;
            g->sub_sq->top_left.y = sub_sq_coord.y;
            g->sub_sq->side_len = sub_len;
            g->sub_sq->id = id;
            g->sub_sq++;
            id++;
        }
        y_coord = sq.top_left.y;
    }
    /* Resets the pointer */
    g->sub_sq -= NSQUARES;
}

struct coord getTopLeft(struct sub_square *sq)
{
    return sq->top_left;
}

struct coord getTopRight(struct sub_square *sq)
{
    struct coord res;
    res.x = sq->top_left.x + sq->side_len;
    res.y = sq->top_left.y;
    return res;
}

struct coord getLowRight(struct sub_square *sq)
{
    struct coord res;
    res.x = sq->top_left.x + sq->side_len;
    res.y = sq->top_left.y - sq->side_len;
    return res;
}

struct coord getLowLeft(struct sub_square *sq)
{
    struct coord res;
    res.x = sq->top_left.x;
    res.y = sq->top_left.y - sq->side_len;
    return res;
}

/* Use for DEBUG - prints all the sub squares in the grid */
void printGrid(struct grid *g)
{
    for (int i = 0; i < g->nsquares; i++)
    {
        printf("Sub SQ: %d  x: %lf y: %lf trx: %lf try: %lf \n", g->sub_sq->id, g->sub_sq->top_left.x, g->sub_sq->top_left.y, getTopRight(g->sub_sq).x, getTopRight(g->sub_sq).y);
        g->sub_sq++;
    }
    g->sub_sq -= g->nsquares;
}

void initPoints(struct sub_square *sq, size_t initialSize)
{
    sq->x = (double *)malloc(initialSize * sizeof(double));
    sq->y = (double *)malloc(initialSize * sizeof(double));
    sq->z = (double *)malloc(initialSize * sizeof(double));
    sq->size = initialSize;
    sq->npoints = 0;
}

void insertPoint(struct sub_square *sq, double x, double y, double z)
{
    if (sq->npoints == sq->size)
    {
        sq->size *= 2;
        sq->x = (double *)realloc(sq->x, sq->size * sizeof(double));
        sq->y = (double *)realloc(sq->y, sq->size * sizeof(double));
        sq->z = (double *)realloc(sq->z, sq->size * sizeof(double));
    }

    sq->x[sq->npoints] = x;
    sq->y[sq->npoints] = y;
    sq->z[sq->npoints] = z;

    sq->npoints += 1;
}

void insertPointAddr(struct sub_square *sq, double *x, double *y, double *z)
{
    if (sq->npoints == sq->size)
    {
        sq->size *= 2;
        sq->x = (double *)realloc(sq->x, sq->size * sizeof(double));
        sq->y = (double *)realloc(sq->y, sq->size * sizeof(double));
        sq->z = (double *)realloc(sq->z, sq->size * sizeof(double));
    }

    sq->x += sq->npoints;
    sq->y += sq->npoints;
    sq->z += sq->npoints;

    sq->x = x;
    sq->y = y;
    sq->z = z;

    sq->x -= sq->npoints;
    sq->y -= sq->npoints;
    sq->z -= sq->npoints;

    sq->npoints += 1;
}

void freePoints(struct sub_square *sq)
{
    free(sq->x);
    free(sq->y);
    free(sq->z);

    sq->x = NULL;
    sq->y = NULL;
    sq->z = NULL;

    sq->npoints = sq->size = 0;
}

void freeGrid(struct grid *g)
{
    free(g->sub_sq);
    g->sub_sq = NULL;
}

void eliminateNonDrivePoints(struct grid *g)
{
    for (int i = 0;i<g->nsquares;i++)
    {
        if (g->sub_sq->safe == false)
        {
            freePoints(g->sub_sq);
        }
        g->sub_sq+=1;
    }
    g->sub_sq-=g->nsquares;
}

void loadPointsGrid(struct grid *g, struct t_point_cloud p)
{
    double *curx = p.x;
    double *cury = p.y;
    double *curz = p.z;

    for (int i = 0; i < g->nsquares; i++)
    {
        struct coord tl = getTopLeft(g->sub_sq);
        struct coord tr = getTopRight(g->sub_sq);
        struct coord ll = getLowLeft(g->sub_sq);

        initPoints(g->sub_sq, 100);

        for (int j = 0; j < p.npoints; j++)
        {
            if (*(curx) > tl.x && *(curx) < tr.x)
            {
                if (*(cury) > ll.y && *(cury) < tl.y)
                {
                    insertPoint(g->sub_sq, *(curx), *(cury), *(curz));
                }
            }
            curx += 1;
            cury += 1;
            curz += 1;
        }
        curx -= p.npoints;
        cury -= p.npoints;
        curz -= p.npoints;
        g->sub_sq += 1;
    }
    g->sub_sq -= g->nsquares;
}

void loadPointsGridAddr(struct grid *g, struct t_point_cloud p)
{
    double *curx = p.x;
    double *cury = p.y;
    double *curz = p.z;

    for (int i = 0; i < g->nsquares; i++)
    {
        struct coord tl = getTopLeft(g->sub_sq);
        struct coord tr = getTopRight(g->sub_sq);
        struct coord ll = getLowLeft(g->sub_sq);

        initPoints(g->sub_sq, 100);

        for (int j = 0; j < p.npoints; j++)
        {
            if (*(curx) > tl.x && *(curx) < tr.x)
            {
                if (*(cury) > ll.y && *(cury) < tl.y)
                {
                    insertPointAddr(g->sub_sq, curx, cury, curz);
                }
            }
            curx += 1;
            cury += 1;
            curz += 1;
        }
        curx -= p.npoints;
        cury -= p.npoints;
        curz -= p.npoints;
        g->sub_sq += 1;
    }
    g->sub_sq -= g->nsquares;
}

void printPointsSQ(struct sub_square sq)
{
    for (int i = 0; i < sq.npoints; i++)
    {
        sq.x++;
        sq.y++;
        sq.z++;
        printf("%lf %lf %lf\n", *(sq.x), *(sq.y), *(sq.z));
    }

    sq.x -= sq.npoints;
    sq.y -= sq.npoints;
    sq.z -= sq.npoints;
}

void printPointsGrid(struct grid g)
{
    for (int i = 0; i < g.nsquares; i++)
    {
        printPointsSQ(g.sub_sq[i]);
        getchar();
    }
}

double getMinZSQ(struct sub_square *sq)
{
    double min_value = DBL_MAX;
    double *cur = sq->z; /* Double pointer to search the x points */

    for (int i = 0; i < sq->npoints; i++)
    {
        if (*(cur) < min_value)
        {
            min_value = *(cur);
        }
        cur = cur + 1;
    }

    return min_value;
}

double getMaxZSQ(struct sub_square *sq)
{
    double max_value = DBL_MIN;
    double *cur = sq->z; /* Double pointer to search the x points */

    for (int i = 0; i < sq->npoints; i++)
    {
        if (*(cur) > max_value)
        {
            max_value = *(cur);
        }
        cur = cur + 1;
    }

    return max_value;
}

bool checkEmptySQ(struct sub_square *sq)
{
    if (sq->npoints == 0)
    {
        return true;
    }
    return false;
}

void checkDriveSQ(struct sub_square *sq)
{
    double maxZ = getMaxZSQ(sq);
    double minZ = getMinZSQ(sq);

    if (checkEmptySQ(sq) == true)
    {
        printf("ID: %d Empty square\n", sq->id);
        return;
    }

    printf("ID: %d maxz: %lf minz: %lf\n", sq->id, maxZ, minZ);

    if (maxZ > 2 || (maxZ - minZ) > 1)
        sq->safe = false;
    else
        sq->safe = true;

    printf("ID: %d driveable: ", sq->id);

    if (sq->safe == true)
        printf("yes\n");

    if (sq->safe == false)
        printf("no\n");
}
