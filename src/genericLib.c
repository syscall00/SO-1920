
#include "../lib/genericLib.h"

/**
 * Calcola un numero random definito nell'intervallo (estremi compresi)
 * @param minVal	valore più piccolo dell'intervallo
 * @param maxVal	valore più grande dell'intervallo
 * @return
 */
int myRandom(int minVal, int maxVal) {
	unsigned int seed;
	FILE *urandom = fopen("/dev/urandom", "r");
	fread(&seed, sizeof(int), 1, urandom);
	fclose(urandom);
	srand(seed);
	return minVal + (int)((double) rand() / RAND_MAX *(maxVal - minVal + 1));
}