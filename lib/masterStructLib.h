#ifndef __MASTER_H__
#define __MASTER_H__


typedef struct masterStruct {
    player *playerList;
    chessBoard table;
    statistics stats;
    ipcWrapper ipc;
    int endGame;    
} masterStruct;

void printState (masterStruct);
void initStructure(masterStruct**);
void detatchStructure(masterStruct* m);


#endif