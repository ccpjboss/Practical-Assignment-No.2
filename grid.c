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

/* Use for DEBUG - prints all the sub squares in the grid */
void printGrid(struct grid *g)
{
    for (int i = 0; i < g->nsquares; i++)
    {
        printf("Sub SQ: %d Top Corner x: %lf y: %lf\n", g->sub_sq->id, g->sub_sq->top_left.x, g->sub_sq->top_left.y);
        g->sub_sq++;
    }
    g->sub_sq -= NSQUARES;
}
