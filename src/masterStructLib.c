#include "../lib/myIPC.h"
#include "../lib/configLib.h"
#include "../lib/positionLib.h"
#include "../lib/cellLib.h"
#include "../lib/playerLib.h"
#include "../lib/scacchieraLib.h"
#include "../lib/statisticsLib.h"
#include "../lib/masterStructLib.h"
#include "../lib/genericLib.h"

#include <stdio.h>
#include <sys/shm.h>


/**
 * Crea e popola la struttura principale di tutti gli strumenti IPC
 * @param masterStruct **m	Un puntatore alla struttura da popolare
 * @return
 */
void initStructure(masterStruct **m) {
    masterStruct *edit;
    int i = 0;
	int shmID = shmget(IPC_PRIVATE, sizeof(struct masterStruct),0666 | IPC_CREAT); TEST_ERROR;
    *m = (masterStruct*)shmat(shmID,NULL,0); TEST_ERROR;
	edit = *m;
	createIPC(&(edit->ipc));
	edit->ipc.shmMaster = shmID;

	/* PLAYER */
	edit->playerList = shmat(edit->ipc.shmPlayers,0,0); TEST_ERROR;

	/* PEDINE */
	for(i = 0; i < SO_NUM_G; i++) {
		edit->playerList[i].pawnList = shmat(edit->ipc.shmPawns[i],NULL,0);
	}

	/* BANDIERINE */
	edit->table.flagsList = shmat(edit->ipc.shmFlags,NULL,0); TEST_ERROR;

	/* SCACCHIERA */
	edit->table.board = shmat(edit->ipc.shmChessboard,NULL,0); TEST_ERROR;

    /* METRICHE */
    edit->stats.usedMoves = shmat(edit->ipc.shmPlayerMoves,NULL,0); TEST_ERROR;
    edit->stats.playerScore = shmat(edit->ipc.shmPlayerScore,NULL,0); TEST_ERROR;

	edit->endGame = 0;
	edit->stats.roundGiocati = 0;

    for(i = 0; i < SO_BASE*SO_ALTEZZA;i++) {
        initSemAvailable(edit->ipc.semBoardID,i);  TEST_ERROR;
    }     
}


/**
 * Stacca tutte le memorie condivise attaccate al parametro m
 * @param masterStruct **m   Un puntatore alla struttura da cui staccare le shmem
 * @return
 */
void detatchStructure(masterStruct* m) {
	int i;
	
	for(i = 0; i < SO_NUM_G;i++) shmdt(m->playerList[i].pawnList);
	shmdt(m->playerList);
	shmdt(m->table.board);
	shmdt(m->table.flagsList);
    shmdt(m->stats.playerScore);
    shmdt(m->stats.usedMoves);
	shmdt(&m);
}

/**
 * Stampa lo stato del gioco al momento della chiamata
 * @param masterStruct m   La struttura da cui prendere lo stato attuale del round
 * @return
 */
void printState(masterStruct m) {

    int i, j;
    for(i = 0; i < SO_ALTEZZA; i++) {
        for(j = 0; j < SO_BASE; j++) {
         printf("--");}
        printf("-\n");

        for(j = 0; j < SO_BASE; j++) {
            if(m.table.board[SO_ALTEZZA*j+i].isFlag == 1){
                 printf ("|"YELLOW"@"NC);
            }
            if(m.table.board[SO_ALTEZZA*j+i].isPawn==1) {
                 printf("|%d",(m.table.board[SO_ALTEZZA*j+i].p.playerIDOwner)+1);
            }
            else if(m.table.board[SO_ALTEZZA*j+i].isFlag==0 && m.table.board[SO_ALTEZZA*j+i].isPawn==0)
            {
                     printf("| ");
            }
        }
        printf("|\n");
    }
        for(j = 0; j < SO_BASE; j++) {
         printf("--");}
    printf("-\n");
    
    for(i = 0; i < SO_NUM_G; i++) {
        /*calcola mosse */
        int mosse = 0;
        for(j = 0; j < SO_NUM_P; j++) mosse += m.playerList[i].pawnList[j]->p.remainingMoves;
        m.stats.usedMoves[i] = (SO_N_MOVES*SO_NUM_P) - mosse;

        printf(YELLOW"GIOCATORE %d\n"NC"Punteggio: %d\nMosse Rimanenti: %d\n",(m.playerList[i].playerID)+1,m.stats.playerScore[i], mosse);        
    }
    printf("\n\n\n");

}
