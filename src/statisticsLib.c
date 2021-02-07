#include "../lib/configLib.h"
#include "../lib/positionLib.h"
#include "../lib/statisticsLib.h"
#include "../lib/genericLib.h"
#include <stdio.h>

/**
 * Stampa le statistiche del gioco al momento della chiamata
 * @param statistics m   La struttura da cui prendere le statistiche
 * @return
 */
void printStatistics(statistics m) {
	int i;
	printf(YELLOW"METRICHE\n"NC);
	printf("NUMERO DI ROUND GIOCATI: %d\n\n", m.roundGiocati);

	for(i = 0; i < SO_NUM_G; i++) {
		printf(YELLOW"GIOCATORE %d\n"NC,i+1);
		printf("MOSSE UTILIZZATE SU MOSSE TOTALI: %.2f\n", (float)(m.usedMoves[i]/(SO_N_MOVES*SO_NUM_P)));
		

		printf("PUNTI OTTENUTI SU MOSSE UTILIZZATE: %.2f\n", (float)(m.playerScore[i]/(float)(m.usedMoves[i])));
		m.totalPoints+=m.playerScore[i];

	}
	printf("\n");
	printf("PUNTI TOTALI SU TEMPO DI GIOCO: %.2f\n", (float)(m.totalPoints / (m.totalTime)));

}
