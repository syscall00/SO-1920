#include "../lib/configLib.h"
#include "../lib/positionLib.h"
#include "../lib/myIPC.h"
#include "../lib/genericLib.h"

/**
 * Sposta una pedina in una posizione specificata
 * @param position	newPos		posizione da controllare
 * @param int		semID		identificativo dei semafori della scacchiera
 * @return          1 se la mossa è valida, 0 altrimenti
 */
int checkValidMove(position newPos, int semID) {
    
    int ret = 0;
    /* Se la posizione è ancora nella scacchiera */
    if(newPos.x < SO_ALTEZZA && newPos.y < SO_BASE && newPos.x >= 0 && newPos.y >= 0){
        /* Se la posizione non è occupata da altre pedine */
        if(reserveSem(semID,(newPos.y*SO_ALTEZZA)+newPos.x,0)==0) {    
            ret = 1;   
        }
    }
    return ret;
}

/**
 * Sposta una pedina in una posizione specificata
 * @param position	oldPos		    posizione precedente
 * @param position	destPos		    posizione di destinazione
 * @param int		lastMovement	identificativo dei semafori della scacchiera
 * @return          restituisce una posizione valida per il gioco (+1,-1) più vicina a destPos
 */
position calculateStep(position oldPos, position destPos, int lastMovement) {
    position newPos = oldPos;  
    
    int r = myRandom(0,1);
    if(lastMovement != 0 && r == 1){
        if(newPos.x!=destPos.x) 
            newPos.x = newPos.x < destPos.x? newPos.x+1:newPos.x-1; 
        else if(newPos.y!=destPos.y) 
        newPos.y = newPos.y < destPos.y? newPos.y+1:newPos.y-1; 
    }
    else {
        if(newPos.y!=destPos.y) 
            newPos.y = newPos.y < destPos.y? newPos.y+1:newPos.y-1; 
        else if(newPos.x!=destPos.x) 
            newPos.x = newPos.x < destPos.x? newPos.x+1:newPos.x-1; 
    }
    return newPos;
    
}

/**
 * Calcola il valore del quadrante di una posizione
 * @param position	pos		    posizione da controllare
 * @return          numero del quadrante corrispondente alla posizione
 */
int calculateQuadrant(position pos) {
	int ret;

	if(0 <= pos.y && pos.y < SO_BASE / 4) 
		ret = 0 <= pos.x && pos.x < SO_ALTEZZA / 2 ? 0 : 4;
	else if(SO_BASE / 4 <= pos.y && pos.y < SO_BASE / 2) 
		ret = 0 <= pos.x && pos.x < SO_ALTEZZA / 2 ? 1: 5;
	
	else if(SO_BASE / 2 <= pos.y && pos.y < ((SO_BASE / 2)+SO_BASE)/2) 
		ret = 0 <= pos.x && pos.x < SO_ALTEZZA / 2 ? 2 : 6;
	
	else if(((SO_BASE / 2)+SO_BASE)/2 <= pos.y && pos.y < SO_BASE ) 
		ret = 0 <= pos.x && pos.x < SO_ALTEZZA / 2 ? 3 : 7;
	
	return ret;

}