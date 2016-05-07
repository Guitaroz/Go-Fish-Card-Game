// main.cpp : Entry point for the project

// This includes all of our standard headers

#include "stdafx.h"

#include "Game.h"



/***********/
/* Globals */
/***********/

/**************/
/* Prototypes */
/**************/


int main(int, const char*[])
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1);
	
	srand(unsigned int(time(0)));
	Game game;
	game.Run();
	return 0;
}

