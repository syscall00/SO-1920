#include "../lib/myIPC.h"
#include "../lib/configLib.h"
#include "../lib/positionLib.h"
#include "../lib/cellLib.h"
#include "../lib/playerLib.h"
#include "../lib/scacchieraLib.h"
#include "../lib/statisticsLib.h"
#include "../lib/masterStructLib.h"
#include "../lib/genericLib.h"
#include <sys/sem.h>


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <signal.h> 
#include <stdlib.h>


/**
 * Crea le ipc necessarie al funzionamento del programma
 * @param ipcWrapper    *ipc    puntatore alla struttura nella quale salvare i dati delle ipc
 * @return
 */
void createIPC(ipcWrapper* ipc) {
    int i = 0;
    /* Coda di messaggi */
    ipc->queueID = msgget(0x1e57, IPC_CREAT | 0666); TEST_ERROR;
    
	/* Spazio per i giocatori */
	ipc->shmPlayers = shmget(IPC_PRIVATE, sizeof(player) * SO_NUM_G, 0666 | IPC_CREAT); TEST_ERROR;


    /* Creo lo spazio per salvare gli id della shmem delle liste di pedine.
    Nota: viene utilizzata la malloc che restituisce un indirizzo di memoria 
    privato per il proceso chiamante (in questo caso il master) e quindi questo 
    vettore può essere utilizzato e verrà utilizzato SOLO dal processo master. */
    ipc->shmPawns = malloc(sizeof(int)*SO_NUM_G);

	/* CREO LO SPAZIO PER LE PEDINE DI OGNI GIOCATORE*/
	for(i = 0; i < SO_NUM_G; i++)
		ipc->shmPawns[i] = shmget(IPC_PRIVATE, sizeof(cell*)*SO_NUM_P, 0666 | IPC_CREAT);
    /* Spazio per le metriche*/
    ipc->shmPlayerScore = shmget(IPC_PRIVATE, sizeof(int)* SO_NUM_G, 0666 | IPC_CREAT); TEST_ERROR;
    ipc->shmPlayerMoves = shmget(IPC_PRIVATE, sizeof(float) * SO_NUM_G, 0666 | IPC_CREAT); TEST_ERROR;

	/* Spazio per le bandierine */
	ipc->shmFlags = shmget(IPC_PRIVATE, sizeof(cell*) * SO_FLAG_MAX, 0666 | IPC_CREAT); TEST_ERROR;

	/* Spazio per la scacchiera */
	ipc->shmChessboard = shmget(IPC_PRIVATE,sizeof(cell) * SO_BASE * SO_ALTEZZA, 0666 | IPC_CREAT); TEST_ERROR;

    /* Semafori della scacchiera */
	ipc->semBoardID = semget(IPC_PRIVATE, SO_BASE *SO_ALTEZZA, 0666 | IPC_CREAT); TEST_ERROR;

    /* Semafori la sincronizzazione tra player e pedine */


    ipc->semMasterID = semget(IPC_PRIVATE, 1,0666 | IPC_CREAT);
    ipc->semPlayerID = semget(IPC_PRIVATE, SO_NUM_G,0666 | IPC_CREAT);
    ipc->semPawnID = semget(IPC_PRIVATE, SO_NUM_P*SO_NUM_G,0666 | IPC_CREAT);

}

/**
 * Pulisce le ipcs utilizzate dal programma 
 * @param ipcWrapper    ipc    struttura che contiene le ipc da eliminare
 * @return
 */
void deleteIPC(ipcWrapper *ipc) {
    int i = 0;

	shmctl(ipc->shmChessboard,IPC_RMID,NULL);
    shmctl(ipc->shmFlags,IPC_RMID,NULL);
    shmctl(ipc->shmMaster,IPC_RMID,NULL);
    shmctl(ipc->shmPlayers,IPC_RMID,NULL);
    shmctl(ipc->shmPlayerScore,IPC_RMID,NULL);
    shmctl(ipc->shmPlayerMoves,IPC_RMID,NULL);
    for(i = 0; i< SO_NUM_G; i++)
    shmctl(ipc->shmPawns[i],IPC_RMID,NULL);
    free(ipc->shmPawns);
    
    }



/**
 * Invia un messaggio alla coda di messaggi specificata 
 * @param struct msgbuf *m          struttura nella quale verrà salvato il messaggio ricevuto
 * @param int           qID         id della coda di messaggi
 * @param int           msgType     tipo del messaggio
 * @param char          *text       messaggio che verrà inviato
 * @param int           len         lunghezza del messaggio che si vuole inviare
 * @return
 */
void sendFunction(struct msgbuf m, int qID, int msgType, char *text, int len) {
    int num_bytes = sprintf(m.mtext, "%s\n", text);
    m.mtype = msgType;
    msgsnd(qID, &m, len, 0);
    
}

/**
 * Riceve un messaggio dalla coda di messaggi specificata 
 * @param struct msgbuf *m          struttura nella quale verrà salvato il messaggio ricevuto
 * @param int           qID         id della coda di messaggi
 * @param int           rcvType     mtype che verrà letto
 * @return
 */
int receiveFunction (struct msgbuf * m, int qID, int rcvType) {
    int num_bytes;
    while (1) {
		num_bytes = msgrcv(qID, m, MSG_LEN, rcvType, 0);

		if (num_bytes >= 0 || errno == 22) {
			break;
		}
     } 
     return num_bytes;
}

/**
 * Inizializza un semaforo come "disponibile" (1) 
 * @param int    semId      id ipc semaforo
 * @param int    semNum     numero del semaforo da inizializzare
 * @return
 */
int initSemAvailable(int semId, int semNum) {
    union semun arg;
    arg.val = 1;
    return semctl(semId, semNum, SETVAL, arg);
}

/**
 * Richiedi un semaforo (-1) 
 * @param int    semId      id ipc semaforo
 * @param int    semNum     numero del semaforo da decrementare
 * @param int    flag       Se vale 0, la chiamata è fatta con IPC_NOWAIT
 * @return
 */
int reserveSem(int semId, int semNum, int flag) {
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg =flag == 0? IPC_NOWAIT : 0;
    return semop(semId, &sops, 1);
}

/**
 * Rilascia un semaforo (+1) 
 * @param int    semId      id ipc semaforo
 * @param int    semNum     numero del semaforo da incrementare
 * @param int    flag       Se vale 0, la chiamata è fatta con IPC_NOWAIT
 * 
 * @return
 */
int releaseSem(int semId, int semNum, int flag) {
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg =flag == 0? IPC_NOWAIT : 0;
    return semop(semId, &sops, 1);
}

