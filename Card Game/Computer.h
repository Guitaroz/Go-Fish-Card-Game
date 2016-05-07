#pragma once
#include "stdafx.h"
#include "Player.h"


class Computer : public Player
{

public:
	// Default ctor
	// In:	_name		The name of the computer player

	Computer(const char* _name = "Computer", int _line = 0, ConsoleColor _color = DarkCyan);


	// Display the computer's name and card backs
	void Show()const;
	void Sort();
	/*Accessor*/

	// Needed for unit tests
	// DO NOT REMOVE
	friend class CTestManager;
};


