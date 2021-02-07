#include "../lib/configLib.h"
#include "../lib/positionLib.h"
#include "../lib/cellLib.h"
#include "../lib/playerLib.h"
#include "../lib/scacchieraLib.h"
#include "../lib/genericLib.h"


/**
 * Piazza le bandiere sulla scacchiera in modo casuale
 * @param chessBoard *s		puntatore alla scacchiera sulla quale verranno piazzate le bandiere
 * @return
 */
void placeFlags(chessBoard *s) {

	int remainingValue = SO_ROUND_SCORE;
	
	int i = 0;
	while (i < s->totalRoundsFlag) {

		/* calcola le coordinate in maniera casuale */
		position ranPos;
		ranPos.x = myRandom(0, SO_ALTEZZA - 1);
		ranPos.y = myRandom(0, SO_BASE - 1);
		
		if (s->board[ranPos.y*SO_ALTEZZA+ranPos.x].isFlag == 0 && s->board[ranPos.y*SO_ALTEZZA+ranPos.x].isPawn == 0) { 
			
			/* 
			calcola il valore della bandiera in modo dinamico. Calcola un valore casuale tra 1 il valore massimo 
			calcolato in modo dinamico di volta in volta. Se è l'ultima bandiera, imposta come valore della bandiera
			il valore restante per arrivare a SO_ROUND_SCORE
			*/
			int randomNow = i == s->totalRoundsFlag-1? remainingValue: myRandom(1,remainingValue/(s->totalRoundsFlag-i));
			remainingValue-=randomNow;

			/* crea la bandiera */
			initFlag(&(s->board[ranPos.y*SO_ALTEZZA+ranPos.x]), ranPos, randomNow);

			/* faccio puntare il puntatore della lista delle bandiere alla bandiera appena creata */
			s->flagsList[i] = &s->board[ranPos.y*SO_ALTEZZA+ranPos.x];
			
			i++;
		}
	}
	s->remainingFlags = s->totalRoundsFlag;
}


/**
 * Piazza le pedine sulla scacchiera divise per quadranti
 * @param cell		**cellePedine	array di puntatori di celle nella quale salvare le pedine
 * @param chessBoard *s				puntatore alla scacchiera sulla quale verranno piazzate le bandiere 
 * @param int 		owner			indice che rappresenta il proprietario delle pedine
 * @param int 		i				indice che rappresenta il numero della pedina che si sta iterando
 * @param int		semCellId		identificativo dei semafori della scacchiera
 * @return
 */
void placePawns(cell **cellePedine, chessBoard *s, int owner, int i, int semCellId) {

		position ranPos;
		int d1,d2,d3,d4,d5,d6,d7;

		/* Calcola il quadrante nel quale inserire la pedina in base al numero i di pedina */
		d4 = floor((0+SO_NUM_P)/2);
		d2 = floor((0+d4) / 2); 
		d6 = floor((d4+SO_NUM_P)/2); 
		d1 = floor((0+d2) /2); 
		d3 = floor((d2+d4) /2); 
		d5 = floor((d4+d6) / 2); 
		d7 = ceil((d6+SO_NUM_P) / 2); 
		
		if(0 <= i && i <= d1) { 
			ranPos.x = myRandom(0, SO_ALTEZZA/2); ranPos.y = myRandom(0, SO_BASE/4-1); }
		
		else if(d1 < i && i <= d2) { 
			ranPos.x = myRandom(0, SO_ALTEZZA/2); ranPos.y = myRandom(SO_BASE/4, SO_BASE/2-1); }
		
		else if(d2 < i && i <= d3) { 
			ranPos.x = myRandom(0, SO_ALTEZZA/2); ranPos.y = myRandom(SO_BASE/2, ((SO_BASE+(SO_BASE/2))/2)-1); }
		
		else if(d3 < i && i <= d4) { 
			ranPos.x = myRandom(0, SO_ALTEZZA/2); ranPos.y = myRandom(((SO_BASE+(SO_BASE/2))/2),SO_BASE-1); }
		
		else if(d4 < i && i <= d5) { 
			ranPos.x = myRandom(SO_ALTEZZA/2, SO_ALTEZZA - 1); ranPos.y = myRandom(0, SO_BASE/4-1); }
		
		else if(d5 < i && i <= d6) {
			ranPos.x = myRandom(SO_ALTEZZA/2, SO_ALTEZZA - 1); ranPos.y = myRandom(SO_BASE/4, SO_BASE/2-1); }
		
		else if(d6 < i && i <= d7) {
			ranPos.x = myRandom(SO_ALTEZZA/2, SO_ALTEZZA - 1); ranPos.y = myRandom(SO_BASE/2, ((SO_BASE+(SO_BASE/2))/2)-1); }
		
		else if(d7 < i && i < SO_NUM_P) { 
			ranPos.x = myRandom(SO_ALTEZZA/2, SO_ALTEZZA - 1); ranPos.y = myRandom(((SO_BASE+(SO_BASE/2))/2),SO_BASE-1); }
		
		/* Se la cella della posizione appena calcolata è libera, inseriscila */
		if (s->board[(ranPos.y * SO_ALTEZZA) + ranPos.x].isFlag == 0 && reserveSem(semCellId, (ranPos.y * SO_ALTEZZA) + ranPos.x,0) == 0) {
			initPawn(&(s->board[ranPos.y*SO_ALTEZZA+ranPos.x]), ranPos, owner);
			
			cellePedine[i] = &s->board[ranPos.y*SO_ALTEZZA+ranPos.x];
		}
		/* Altrimenti calcola una nuova posizione */
		else placePawns(cellePedine,s,owner,i, semCellId);	
}


/**
 * Sposta una pedina in una posizione specificata
 * @param position		oldPos		posizione precedente della pedina
 * @param position 		newPos		posizione nuova della pedina 
 * @param chessBoard 	*s			puntatore alla scacchiera sulla quale si muove la pedina
 * @param cell 		**pawn		puntatore della lista di pedine da aggiornare
 * @param int			qID			identificativo della coda di messaggi al quale sono collegati i processi
 * @return
 */
void move(position oldPos, position newPos, chessBoard *s, cell **pawn, int qID) {

	struct msgbuf msg;
	int id_owner = s->board[oldPos.y*SO_ALTEZZA+oldPos.x].p.playerIDOwner;

	/* Se è una pedina, notifica il player inviandogli la posizione e 
	l'indice del giocatore proprietario della pedina*/
	if (s->board[newPos.y*SO_ALTEZZA+newPos.x].isFlag == 1) {
		char ss[10];	
		
		s->board[newPos.y*SO_ALTEZZA+newPos.x].isFlag = 0;
		sprintf(ss, "%d %d %d", newPos.x, newPos.y, id_owner);
		sendFunction(msg, qID, 20, ss,10);
	}


	/* Cancella la pedina nella vecchia posizione e creala nella nuova */
	s->board[oldPos.y*SO_ALTEZZA+oldPos.x].isPawn = 0;

	s->board[newPos.y*SO_ALTEZZA+newPos.x].p.remainingMoves = (s->board[oldPos.y*SO_ALTEZZA+oldPos.x].p.remainingMoves) - 1;
	s->board[newPos.y*SO_ALTEZZA+newPos.x].isPawn = 1;
	s->board[newPos.y*SO_ALTEZZA+newPos.x].p.playerIDOwner = id_owner;
	s->board[newPos.y*SO_ALTEZZA+newPos.x].pos = newPos;
	s->board[newPos.y*SO_ALTEZZA+newPos.x].p.target = s->board[oldPos.y*SO_ALTEZZA+oldPos.x].p.target;
	s->board[newPos.y*SO_ALTEZZA+newPos.x].p.hasTarget = s->board[oldPos.y*SO_ALTEZZA+oldPos.x].p.hasTarget;
	s->board[newPos.y*SO_ALTEZZA+newPos.x].p.lastMovement = newPos.x-oldPos.x == 0? 1: 0;
	
	s->board[newPos.y*SO_ALTEZZA+newPos.x].quadrant = s->board[oldPos.y*SO_ALTEZZA+oldPos.x].quadrant;


	*pawn = &s->board[newPos.y*SO_ALTEZZA+newPos.x];	

}
