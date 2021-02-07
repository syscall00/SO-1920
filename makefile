# flags per la compilazione
FLAGS = -std=c89 -Wpedantic -D_POSIX_C_SOURCE=199309L

all: compile link

run: compile link exec

link: 
	gcc *.o src/*.o -o game -lm

compile : src/configLib.o src/positionLib.o src/cellLib.o src/playerLib.o src/scacchieraLib.o src/statisticsLib.o src/masterStructLib.o src/myIPC.o src/genericLib.o master.o 

exec:
	./game

master: master.c 
	gcc $(FLAGS) master.c 

configLib : configLib.c
	gcc $(FLAGS) src/configLib.c

positionLib : positionLib.c
	gcc $(FLAGS) src/positionLib.c

cellLib : cellLib.c
	gcc $(FLAGS) src/cellLib.c

playerLib : playerLib.c
	gcc $(FLAGS) src/playerLib.c

scacchieraLib : scacchieraLib.c
	gcc $(FLAGS) src/scacchieraLib.c

statisticsLib : statisticsLib.c
	gcc $(FLAGS) src/statisticsLib.c

masterStructLib : masterStructLib.c
	gcc $(FLAGS) src/masterStructLib.c

myIPC : myIPC.c
	gcc $(FLAGS) src/myIPC.c

genericLib : genericLib.c
	gcc $(FLAGS) src/genericLib.c

clean:
	rm -f src/*.o *.o game

