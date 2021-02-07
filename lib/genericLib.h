#ifndef __GENERIC_H__
#define __GENERIC_H__

#include "myIPC.h"
#include "configLib.h"
#include "positionLib.h"
#include "cellLib.h"
#include "playerLib.h"
#include "scacchieraLib.h"
#include "statisticsLib.h"
#include "masterStructLib.h"

#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <signal.h> 
#include <sys/wait.h> 
#include <math.h>
#include <errno.h>
#include <string.h>
#include <float.h>
#include <time.h>

#define YELLOW	"\033[1;33m"
#define NC	"\033[0m"
#define MASTER gameMaster
#define PLAYER(i) MASTER->playerList[i]
#define PAWN(i,j) PLAYER(i).pawnList[j]
#define FLAG(i)	  CHESSBOARD.flagsList[i]
#define CHESSBOARD	MASTER->table


int myRandom(int minVal, int maxVal);



#endif