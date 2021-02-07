#include "../lib/configLib.h"
#include "../lib/positionLib.h"
#include "../lib/cellLib.h"
#include <float.h>
#include <math.h>


/**
 * Inizializza una cella come pedina
 * @param cell *c	il puntatore della cella in cui verrà inserita la nuova pedina
 * @param position pos	la posizione della nuova pedina
 * @param int playerIDowner		indice del giocatore proprietario della pedina
 * @return
 */
void initPawn(cell *c, position pos, int playerIDowner) {
    c->pos = pos;
    c->p.playerIDOwner = playerIDowner;
    c->isPawn = 1;
    c->isFlag = 0;
	c->p.hasTarget = 0;
	c->p.target = pos;
	c->p.remainingMoves = SO_N_MOVES;
	c->quadrant = calculateQuadrant(pos);
}

/**
 * Inizializza una cella come bandiera
 * @param cell *c	il puntatore della cella in cui verrà inserita la nuova bandiera
 * @param position pos	la posizione della nuova bandiera
 * @param int value		valore della bandiera
 * @return
 */
void initFlag(cell *c, position pos, int value) {
    c->pos = pos;
    c->f.value = value;
    c->isPawn = 0;
    c->isFlag = 1;
	c->quadrant = calculateQuadrant(pos);

} 
/**
 * Trova e restituisce la posizione della bandierina più vicina alla bandierina chiamante 
 * Non so se mi servirà, quindi per ora non ne parlo molto.
 * @param cell *c	il puntatore della cella in cui verrà inserita la nuova bandiera
 * @param position pos	la posizione della nuova bandiera
 * @param int value		valore della bandiera
 * @return
 */
position getNearFlag(position pos, cell *flagsList[], cell pedinaCall, int numFlags)
{
	position near = pos;
	float minDist = FLT_MAX;
	int i;	
	for (i = 0; i < numFlags; i++)
	{
		if(flagsList[i]->isFlag && pedinaCall.quadrant == flagsList[i]->quadrant) {
			position temp = flagsList[i]->pos;
			float distance =sqrt(((pos.x - temp.x) *(pos.x - temp.x)) + ((pos.y - temp.y) *(pos.y - temp.y)));

			if (distance < minDist && pedinaCall.p.remainingMoves >=distance && distance <= MAX_DISTANCE) { 
				minDist = distance;
				near = temp;
				pedinaCall.p.hasTarget = 1;
				pedinaCall.p.target = near;
			}
		}
	}
	return near;
}