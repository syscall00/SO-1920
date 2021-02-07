#include "lib/genericLib.h"
void handler(int);
void processoGiocatore(int i);
void processoPedina(int i, int j);

void doRound();

masterStruct * gameMaster;

int main(int argc, char *argv[]) {
	int i,j;
	struct msgbuf msg;	

	/* Leggi a runtime i valori del file di configurazione */
	getParameters();
	/* Inizializza la struttura di gioco creando le IPC necessarie */
	initStructure(&gameMaster);

	/* Imposta l'handler per il timer di gioco */
	if (signal(SIGALRM, handler) == SIG_ERR || signal(SIGINT, handler) == SIG_ERR) {
		fprintf(stderr,"Errore della disposizione dell'handler\n");
		exit(-1);
	}

	/* Crea SO_NUM_G processi giocatore e sposta l'esecuzione sulla funzione processoGiocatore */
	for (i = 0; i < SO_NUM_G; i++) {
		switch (fork()) {
			case -1:
			    fprintf(stderr,"Errore #%03d: %s\n", errno, strerror(errno));
				exit(-1);
			case 0:	
				processoGiocatore(i);
		}
	}

	/* Invia un messaggio ai giocatori per cominciare a piazzare le pedine in ordine */
	releaseSem(MASTER->ipc.semPlayerID,0,1);

	/* Attendi che tutte le pedine siano piazzate */	
	reserveSem(MASTER->ipc.semMasterID,0,1);
	
	/* Avvia i round finché il flag di gioco è attivo */
	while (gameMaster->endGame == 0) {
		doRound();
	}

	/* Aspetta l'uscita di tutti i processi giocatore */
	for (i = 0; i < SO_NUM_G; i++)	wait(NULL);
	
	/* Stampa la tabella e le metriche */
	printState(*MASTER);
	printStatistics(MASTER->stats);

	/* Stacca dal processo tutte le IPC ed eliminale */
	detatchStructure(MASTER);
	deleteIPC(&(MASTER->ipc));
	
	exit(0);

}

/**
 * Definisce il comportamento del master nel round
 * @param 
 * @return
 */
void doRound() {
	int i = 0;
	int playTime;
	struct msgbuf msg;	

	/* Calcola il numero di bandiere per il round attuale e le piazza */
	CHESSBOARD.totalRoundsFlag = myRandom(SO_FLAG_MIN,SO_FLAG_MAX);
	placeFlags(&CHESSBOARD);

	/* Comunica ai giocatori di calcolare e notificare le strategie alle pedine */
	for (i = 0; i < SO_NUM_G; i++) releaseSem(MASTER->ipc.semPlayerID,i,1);
	

	/* Aspetta che i giocatori finiscano */
	for (i = 0; i < SO_NUM_G; i++) reserveSem(MASTER->ipc.semMasterID,0, 1);
	
	printf(YELLOW"ROUND NUMERO %d\n"NC, MASTER->stats.roundGiocati + 1);

	/* Stampa lo stato del gioco */
	printState(*MASTER);
			
	/* Comunica ai giocatori che il round è iniziato */
	for (i= 0; i < SO_NUM_G; i++) releaseSem(MASTER->ipc.semPlayerID, i, 1);
	

	/* Avvia il timer di gioco */
	playTime =alarm(SO_MAX_TIME);

	/* Aspetta che tutte le pedine vengano conquistate o il round finisca a causa dello scadere del tempo */
	while(MASTER->endGame == 0 && CHESSBOARD.remainingFlags > 0) { 
		int p = receiveFunction(&msg, MASTER->ipc.queueID, 20);
		int x, y, playerID;
		if(p!=-1) {
			sscanf(msg.mtext, "%d %d %d", &x, &y, &playerID);
			gameMaster->table.board[y*SO_ALTEZZA+x].isFlag = 0;
			gameMaster->table.remainingFlags--;
			gameMaster->stats.playerScore[playerID] += gameMaster->table.board[y*SO_ALTEZZA+x].f.value;
		}
		/* Se il round è finito senza problemi, ferma il timer */
		if(gameMaster->table.remainingFlags == 0) {alarm(0); break;}
	}	

	/* Aggiorna le statistiche di gioco */
	MASTER->stats.totalTime+=(SO_MAX_TIME-playTime);
	gameMaster->stats.roundGiocati++;

	/* Aspetta gli altri processi per ricominciare il round */
	for (i=0; i < SO_NUM_G; i++) reserveSem(MASTER->ipc.semMasterID,0,1);
}

/**
 * Definisce il comportamento degli SO_NUM_G processi giocatore
 * @param 	i	indice che rappresenta il numero del giocatore
 * @return
 */
void processoGiocatore(int i) {
	struct msgbuf msg;
	int j;
	PLAYER(i).playerID = i;

	/* Crea SO_NUM_P pedine per il giocatore i */
	for (j = 0; j < SO_NUM_P; j++) {
		switch (fork()) {
			case -1:
			    fprintf(stderr,"Errore #%03d: %s\n", errno, strerror(errno));
			case 0:
				processoPedina(i, j);
		}
	}

	/* Piazza le pedine in maniera ordinata. Comincia la pedina 0 del giocatore 0 e si continua in modo ordinato */
	for(j = 0; j < SO_NUM_P; j++)  {

		reserveSem(MASTER->ipc.semPlayerID,i,1);
		placePawns(PLAYER(i).pawnList, &CHESSBOARD, i,j, MASTER->ipc.semBoardID);

		if(i == SO_NUM_G-1 && j != SO_NUM_P-1) 
			releaseSem(MASTER->ipc.semPlayerID,0,1);
		else if(i == SO_NUM_G-1 && j == SO_NUM_P-1)
			releaseSem(MASTER->ipc.semMasterID,0,1);
		else
			releaseSem(MASTER->ipc.semPlayerID,i+1,1);
	}

	while (MASTER->endGame == 0) {

		/* Aspetta il master per calcolare la strategia */
		reserveSem(MASTER->ipc.semPlayerID,i,1);

		/* Calcola per ogni pedina, le bandierine target */
		for(j = 0; j < SO_NUM_P; j++) calculateStrategy(CHESSBOARD.flagsList, PAWN(i,j), CHESSBOARD.totalRoundsFlag);

		/* Comunica al master  */
		releaseSem(MASTER->ipc.semMasterID,0,1);

		/* Sblocca le pedine */
		reserveSem(MASTER->ipc.semPlayerID,i,1);
		for (j = 0; j < SO_NUM_P; j++) releaseSem(MASTER->ipc.semPawnID,j*SO_NUM_G+i,1);
		
		/* Aspetta che tutte le SO_NUM_P pedine finiscano la partita */
		for (j = 0; j < SO_NUM_P; j++) reserveSem(MASTER->ipc.semPlayerID,i,1);

		/* Notifica al master che le pedine hanno finito il lavoro */
		releaseSem(MASTER->ipc.semMasterID,0,1);
		 
	}	

	/* Aspetta che le pedine escano */
	for (j = 0; j < SO_NUM_P; j++)	wait(NULL);

	/* Stacca dal processo tutte le IPC */
	detatchStructure(MASTER);
	exit(0);

}


/**
 * Definisce il comportamento degli SO_NUM_P processi pedine
 * @param 	i	indice che rappresenta il numero del giocatore
 * @param 	j	indice che rappresenta il numero della pedina
 * @return
 */
void processoPedina(int i, int j) {
	
	struct msgbuf msg;
	struct timespec s; 
	s.tv_sec = 0; s.tv_nsec = SO_MIN_HOLD_NSEC;
	/* Finchè il flag di gioco è attivo */
	while (MASTER->endGame == 0) {

		int work = 1;

		/* Aspetta il via del giocatore */
		reserveSem(MASTER->ipc.semPawnID,j*SO_NUM_G+i,1);
	
		/* Finché il round è attivo, la pedina ha ancora mosse disponibili e c'è del lavoro da fare */
		while (MASTER->endGame == 0 && work == 1 && PAWN(i,j)->p.remainingMoves > 0) {
				
				position destPos, newPos, nearFlag, oldPos = PAWN(i,j)->pos, flagTarget = PAWN(i,j)->p.target; 

				/* Se la pedina ha un bersaglio già impostato, seguila */
				if(PAWN(i,j)->p.hasTarget) {
					destPos = flagTarget;
				}
				/* Altrimenti, prova a calcolare un nuovo bersaglio */
				else {
					nearFlag = getNearFlag(oldPos, CHESSBOARD.flagsList, *PAWN(i,j),CHESSBOARD.totalRoundsFlag);
					/* Se non esiste ferma la pedina */
					if (nearFlag.x == oldPos.x && nearFlag.y == oldPos.y){
							work = 0; continue;
					}
					/* Se esiste seguilo */
					else { 
						PAWN(i,j)->p.hasTarget = 1;
						PAWN(i,j)->p.target = nearFlag;
						destPos = nearFlag;
					}
				}

				/* Calcola la mossa */
				newPos = calculateStep(oldPos, destPos, PAWN(i,j)->p.lastMovement);

				/* Se la mossa appena calcolata è una mossa valida */
				if (checkValidMove(newPos, MASTER->ipc.semBoardID)) {

					/* Esegui lo spostamento */
					move(oldPos, newPos,&CHESSBOARD,&PAWN(i,j),MASTER->ipc.queueID); 

					/* Libera il semaoforo della casella precedentemente occupata */
					releaseSem(MASTER->ipc.semBoardID,(oldPos.y*SO_ALTEZZA)+oldPos.x,0); 
					
					nanosleep(&s, NULL); 

				}
				/* Se la mossa non è valida */
				else { 

				PAWN(i,j)->p.lastMovement = PAWN(i,j)->p.lastMovement == 0? 1: 0;
				newPos = calculateStep(oldPos, destPos, PAWN(i,j)->p.lastMovement);

				/* Se la mossa appena ricalcolata è valida */			
				if (checkValidMove(newPos, MASTER->ipc.semBoardID)) {
					
					/* Esegui lo spostamento */
					move(oldPos, newPos,&CHESSBOARD,&PAWN(i,j),MASTER->ipc.queueID);

					/* Libera il semaoforo della casella precedentemente occupata */
					releaseSem(MASTER->ipc.semBoardID,(oldPos.y*SO_ALTEZZA)+oldPos.x,0);

			        nanosleep(&s, NULL);

				}
			}

			/* Se la bandiera bersaglio è stata conquistata, modifica le impostazioni della pedina */
			if(PAWN(i,j)->p.hasTarget == 1 && CHESSBOARD.board[flagTarget.y*SO_ALTEZZA+flagTarget.x].isFlag == 0)
				PAWN(i,j)->p.hasTarget = 0;	
				
			
		}
		/* Notifica il giocatore che la pedina ha finito il suo lavoro */
		releaseSem(MASTER->ipc.semPlayerID,i,1);
	}

	/* Stacca dal processo tutte le IPC */
	detatchStructure(MASTER);
	exit(0);

}

/**
 * Handler dei segnali di sistema
 * @param 	sig	
 * @return
 */
void handler(int sig) {
	MASTER->endGame = 1;
	msgctl(MASTER->ipc.queueID, IPC_RMID, NULL);
	semctl(MASTER->ipc.semBoardID, 0, IPC_RMID);
	semctl(MASTER->ipc.semPawnID, 0, IPC_RMID);
	semctl(MASTER->ipc.semPlayerID, 0, IPC_RMID);
	semctl(MASTER->ipc.semMasterID, 0, IPC_RMID);

}



