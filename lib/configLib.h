#ifndef __CONFIG_H__
#define __CONFIG_H__


#define CONFIG_FILE "config/config.txt"

extern int SO_BASE;
extern int SO_ALTEZZA;
extern int SO_FLAG_MIN;     
extern int SO_FLAG_MAX;    
extern int SO_NUM_G;     
extern int SO_NUM_P; 
extern int SO_N_MOVES;       
extern int SO_ROUND_SCORE;  
extern int SO_MAX_TIME;
extern int SO_MIN_HOLD_NSEC;

extern int MAX_DISTANCE;
void getParameters();
void setMaxDist();


#endif