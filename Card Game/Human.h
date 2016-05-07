#pragma once
#include "stdafx.h"
#include "Player.h"

/* TODO Lab2:
Make Human a child of Player
*/

class Human : public Player
{

public:
	// Default ctor
	// In:	_name		The name of the human player
	//		_line		What line in Console will Show() execute
	Human(const char* _name = "Human", int _line = 0, ConsoleColor _color = Green);

	// Display the computer's name and card backs
	void Show()const;
	void AddToScore(int _add);
	void Sort();

	// Needed for unit tests
	// DO NOT REMOVE
	friend class CTestManager;
};

