#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, const char **argv) {
	srand(time(NULL));

	struct Game game;

	game_init(&game);
	game_start(&game);
	game_destroy(&game);

	return 0;
}
