#pragma once
#include <iostream>
using namespace std;
#include "stdafx.h"
#include "Console.h"


#include "Player.h"
#include "Deck.h"
#include "FmodSound.h"

// Enum of gamestates, you can add and remove from this if you need additional ones (examples include: GAME_TITLE, GAME_MENU, GAME_OPTIONS, etc).
enum GAMESTATE { GAME_INIT, GAME_TOPSCORES, GAME_MENU, GAME_PLAY, GAME_END, GAME_CHEATS, GAME_INSTRUCTIONS, GAME_QUIT };

class Game
{
	// Necessary data members
	Player* m_players[4];	// The players in the game
	Deck m_Deck;			// The deck of cards
	int m_currPlayer;		// Keeps track of which index is currently taking their turn
	int m_numPlayers;		// The number of players playing in the game
	ConsoleColor m_color;
	bool m_paused;
	bool init;
	Sound Music;
	SoundEffect * Beep;


	void SetCoutLine() const;
	void ShowAll() const;
	void Pause();
	void LookNice() const;
	bool IsPlayersHandEmpty() const;
	bool AskToPlayAgain() const;
	bool AskToKeepScores() const;
	void DisplayName(int i) const;
	void UpdateTopScores();
	void DisplayTopScores();
	void OverWriteScores(const char _Arr[11][40], int _scores[]) const;
	void clear() const;

	void PopulateHand(Player& _player);
	void CheckWhoWins();
	bool AskTheComputer(int& _cardChoice, int& _playerChoice);
	void CreatePlayers();
	void CreatePlayers(char * _name);
	bool PickAComputer(int& _playerChoice);
	void PlayerInput();
	void CheckAndPopulate();
	void ComputerInput();
	bool CheckGameOverAndPlayerHand();
	void DisplayInstructionScreen();
	void InitializeGoFish();
	void CheatsMenu();
	void HomeMenu();
	void Sort();



	// GAMESTATE object to store the game's current state.
	GAMESTATE m_state;

	// Add game data members as necessary

public:
	// Default ctor
	Game();

	// Dtor
	~Game();

	// The main function of the game, this loops until a certain gamestate is set.
	void Run();

	bool SearchCards(int _playerChoice, int _cardIndex);

	// Modifier for the gamestate data member.
	void SetState(GAMESTATE _state) { m_state = _state; }

	// Get the score for the game
	//	For Go Fish, this will take care of pairing
	int Score(Player* _player);
};