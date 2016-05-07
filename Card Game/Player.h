#pragma once
#include "stdafx.h"
#include "Card.h"



class Player
{

private:

	// Needed for unit tests
	// DO NOT REMOVE OR RENAME

	char * m_name;				// Can change size of array if needed
	Card * m_hand;				// Can change size of array if needed

	int m_numCards;				// The number of cards the player is currently holding


	int m_score;				// For "Go Fish," this will represent the number of pairs.  For "UNO," it will be the player's accumulated score
	int m_line;					// Used for Console cursor position.
	const int m_maxCards;		// The number of cards the player can store (the number of elements in Hand)
	const int m_DrawSize;		// Number of cards Players Draw when hand is empty.
	ConsoleColor m_color;		//Color to be displayed in.


	static unsigned char m_cheats;

public:

	Player(const char* _name = "Player", int _maxCards = 13, int _line = 1, int _drawSize = 7);


	Player(const Player& _cpy);

	virtual ~Player() = 0;

	void Sort();
	Player& operator=(const Player& _assign);
	/* Accessors */
	int GetDrawSize()const { return m_DrawSize; }

	const char* GetName() const { return m_name; }
	int GetNumCards() const		{ return m_numCards; }
	int GetMaxCards() const		{ return m_maxCards; }
	int GetScore() const		{ return m_score; }
	int GetLine() const { return m_line; }
	ConsoleColor GetColor() const{ return m_color; }

	bool GetCard(int _index, Card& _card) const;
	bool IsCheatActive(short i) const;
	/* Mutators */

	void SetColor(ConsoleColor _color) { m_color = _color; }
	void SetName(const char* _name);
	void SetLine(const int _line) { m_line = _line; }
	virtual void AddToScore(int _add);
	void ActivateCheat(short i) { m_cheats |= (1 << i); }
	void DeactivateCheat(short i) { m_cheats &= ~(1 << i); }
	void ToggleCheat(short i) { m_cheats ^= (1 << i); }



	// Add a Card to the player's hand
	// In:	_card		The card being added
	//
	// Return: True if the card was successfully added to their hand
	//		   False if the player's hand was already full
	bool AddCard(const Card& _card);
	// Remove a Card from the player's hand
	// In:	_index		The index of the card to remove from the array
	//		_card		A reference to store that value in
	//
	// Return: True if there was a card actually stored at that index.
	//		   False if the index was "empty"
	bool Discard(int _index, Card& _discard);
	// Clear the player's hand so that they are holding 0 cards
	void Clear();

	// Display method (empty for this class)
	virtual void Show() const = 0;

	// Needed for unit tests
	// DO NOT REMOVE
	friend class CTestManager;

};