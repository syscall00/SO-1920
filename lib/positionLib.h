#ifndef __POSITION_H__
#define __POSITION_H__

typedef struct position {
    int x;
    int y;
} position;

int checkValidMove(position, int);
position calculateStep(position,position, int);
int calculateQuadrant(position );


#endif