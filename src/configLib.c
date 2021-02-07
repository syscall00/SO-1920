#include <stdio.h>

#include "../lib/myIPC.h"
#include "../lib/configLib.h"
#include "../lib/positionLib.h"
#include "../lib/cellLib.h"
#include "../lib/playerLib.h"
#include "../lib/scacchieraLib.h"
#include "../lib/statisticsLib.h"
#include "../lib/masterStructLib.h"
#include "../lib/genericLib.h"


int SO_BASE;
int SO_ALTEZZA;
int SO_FLAG_MIN;     
int SO_FLAG_MAX;    
int SO_NUM_G;     
int SO_NUM_P; 
int SO_N_MOVES;       
int SO_ROUND_SCORE;  
int SO_MAX_TIME;
int SO_MIN_HOLD_NSEC;

int MAX_DISTANCE;
/**
 * Legge i parametri del file di configurazione e li salva in variabili globali
 * @param 
 * @return
 */
void getParameters() {

    FILE *f = fopen(CONFIG_FILE,"r");
    TEST_ERROR;
    fscanf(f,"SO_BASE=%d\n",&SO_BASE);
    fscanf(f,"SO_ALTEZZA=%d\n",&SO_ALTEZZA);
    fscanf(f,"SO_FLAG_MIN=%d\n",&SO_FLAG_MIN);
    fscanf(f,"SO_FLAG_MAX=%d\n",&SO_FLAG_MAX);
    fscanf(f,"SO_NUM_G=%d\n",&SO_NUM_G);
    fscanf(f,"SO_NUM_P=%d\n",&SO_NUM_P);
    fscanf(f,"SO_N_MOVES=%d\n",&SO_N_MOVES);
    fscanf(f,"SO_ROUND_SCORE=%d\n",&SO_ROUND_SCORE);
    fscanf(f,"SO_MAX_TIME=%d\n",&SO_MAX_TIME);
    fscanf(f,"SO_MIN_HOLD_NSEC=%d\n",&SO_MIN_HOLD_NSEC);
    fclose(f);
    setMaxDist();


}

void setMaxDist() {

    int totalCells = SO_BASE*SO_ALTEZZA;
    int totalPawns = SO_NUM_G*SO_NUM_P;
    int totalMaxFlags = SO_FLAG_MAX;
    int totalFreeCells = totalCells-(totalPawns-totalMaxFlags);

    MAX_DISTANCE =  ((totalFreeCells/totalMaxFlags) / 8)/2;
    
}