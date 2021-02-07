#ifndef __CHESSBOARD_H__
#define __CHESSBOARD_H__

typedef struct chessBoard {
    cell *board;
    cell **flagsList;
    int remainingFlags;
    int totalRoundsFlag;
} chessBoard;

void placeFlags(chessBoard *);
void placePawns(cell **, chessBoard *, int, int , int) ;
void move(position , position , chessBoard * , cell **, int);


#endif 