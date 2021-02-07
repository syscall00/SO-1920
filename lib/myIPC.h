#ifndef __MY_IPC_H__
#define __MY_IPC_H__



#define TEST_ERROR    if (errno) {fprintf(stderr, \
					  "%s:%d: PID=%5d: Error %d (%s)\n", \
					  __FILE__,			\
					  __LINE__,			\
					  getpid(),			\
					  errno,			\
					  strerror(errno));exit(-1);}

#define MSG_LEN 10    

struct msgbuf {
	long mtype;           
	char mtext[MSG_LEN];    
};

union semun {
int val;
struct semid_ds* buf;
unsigned short* array;
#if defined(__linux__)
struct seminfo* __buf;
#endif
};

typedef struct ipcWrapper {
	int queueID;
	int semBoardID;

	int semMasterID;
	int semPlayerID;
	int semPawnID;

	int shmMaster;
	int shmPlayers;
	int *shmPawns;
	int shmFlags;
	int shmChessboard;
	int shmPlayerScore;
	int shmPlayerMoves;

} ipcWrapper ;

void createIPC(ipcWrapper* ipc);
void deleteIPC(ipcWrapper* ipc);

void sendFunction(struct msgbuf , int , int , char[], int );
int receiveFunction (struct msgbuf *, int , int );

int initSemAvailable(int, int);
int reserveSem(int, int, int);
int releaseSem(int, int, int);




#endif