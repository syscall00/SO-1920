#include "../lib/configLib.h"
#include "../lib/positionLib.h"
#include "../lib/cellLib.h"
#include "../lib/playerLib.h"
#include "../lib/genericLib.h"

/**
 * Calcola e passa un bersaglio alla pedina chiamante
 * @param cell **flagsList La lista di pedine in gioco 
 * @param cell *pawn   	La pedina di cui calcolare il bersaglio
 * @param int	numFlags   	Il numero di bandierine del round
 * @return
 */
void calculateStrategy(cell **flagsList, cell* pawn, int numFlags)  {
	int i, index=-1;
	float minDist = FLT_MAX;
	for(i = 0; i < numFlags; i++) {
		position flagPosition = flagsList[i]->pos;
		
		if(pawn->p.hasTarget == 0 && pawn->quadrant == flagsList[i]->quadrant) {

			float tempDist = sqrt(((flagPosition.x - pawn->pos.x) *(flagPosition.x - pawn->pos.x)) + 
				   		((flagPosition.y - pawn->pos.y) *(flagPosition.y - pawn->pos.y)));

			if(tempDist< minDist && tempDist <= MAX_DISTANCE && pawn->p.remainingMoves >= minDist) {
				minDist = tempDist;
				index = i;
			}
		}
	}
	if(index >=0) {
		pawn->p.hasTarget = 1;
		pawn->p.target = flagsList[index]->pos;

	}
}