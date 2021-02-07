#ifndef __STATISTICS_H__
#define __STATISTICS_H__

typedef struct statistics {
    int roundGiocati;
    
    float *usedMoves;
    int *playerScore;
    int totalPoints;
    unsigned int totalTime;


} statistics;

void printStatistics(statistics );

#endif 