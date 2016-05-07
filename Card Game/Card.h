#pragma once
#include "stdafx.h"


class Card
{
	// Needed for unit tests
	// DO NOT REMOVE OR RENAME
	int m_face;		// 2-14, with 14 being the Ace
	char m_suit;	// 3-6 (3 = Hearts, 4 = Diamonds, 5 = Clubs, 6 = Spades)

public:

	// Default constructor
	Card(int _face = 2, char _suit = 3);	// In:	_face			The face value of the card
	//		_suit			The suit of the card
	// Destructor
	~Card();

	/* Accessors */
	int GetFace()  const { return m_face; }
	char GetSuit() const { return m_suit; }

	/* Mutators */
	void SetFace(int _face)  { m_face = _face; }
	void SetSuit(char _suit) { m_suit = _suit; }

	// Display a card to the screen
	void Show() const;

	// Relational operators	
#define CARD_REL_OP(op)\
	bool operator op(const Card &_rhs) const\
	{\
	return (m_face op _rhs.m_face); \
	}
	CARD_REL_OP(>)
		CARD_REL_OP(>= )
		CARD_REL_OP(<)
		CARD_REL_OP(<= )
		CARD_REL_OP(== )
		CARD_REL_OP(!= )

		// Insertion operator to display Cards with cout (or any other ostream object)
		friend ostream &operator<<(ostream &_os, const Card &_rhs);


	friend class CTestManager; // Needed for unit tests // DO NOT REMOVE


};

