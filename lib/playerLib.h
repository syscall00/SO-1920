#ifndef __PLAYER_H__
#define __PLAYER_H__
typedef struct player {
    int playerID;
    cell **pawnList;
    
} player;

void calculateStrategy(cell **flagsList, cell* pawn, int numFlags);

#endif