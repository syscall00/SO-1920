#ifndef __CELL_H__
#define __CELL_H__

typedef struct flag {
    int value;
} flag;

typedef struct pawn {
    int playerIDOwner;
    int remainingMoves;
    int hasTarget;
    int lastMovement;
    position target;
} pawn;

typedef struct cell {
    position pos;
    flag f;
    pawn p;
    int isPawn;
    int isFlag;
    int quadrant;
} cell;

void initPawn(cell *, position , int);
void initFlag(cell *, position, int);
position getNearFlag(position, cell*[], cell, int);




#endif
