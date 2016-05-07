#include "Player.h"

/* TODO Lab5:
Initialize m_maxCards in all constructors
*/

// Default ctor
// In:	_name			The player's name
//		_maxCards		The maximum number of cards they can store

Player::Player(const char* _name, int _maxCards, int _line, int _DrawsSize)
: m_maxCards(_maxCards), m_DrawSize(_DrawsSize)
{
	m_name = NULL;
	SetName(_name);
	m_score = 0;
	m_color = White;
	m_numCards = 0;
	m_line = _line;
	m_hand = new Card[m_maxCards];
	

	// TODO Lab4: Keep in mind that there are more than two data members.

}


unsigned char Player::m_cheats = 0;

Player::Player(const Player& _cpy) : m_maxCards(_cpy.m_maxCards), m_DrawSize(_cpy.m_DrawSize)
{
	m_name = NULL;
	SetName(_cpy.m_name);
	m_numCards = _cpy.m_numCards;
	m_line = _cpy.m_line;
	m_score = _cpy.m_score;

	m_hand = new Card[m_maxCards];
	for (int i = 0; i < _cpy.m_numCards; i++)
	{
		m_hand[i] = _cpy.m_hand[i];
	}


}

// Dtor
Player::~Player()
{
	delete[] m_name;
	delete[] m_hand;
}


Player& Player::operator=(const Player& _assign)
{
	if (this != &_assign)
	{

		m_name = NULL;
		SetName(_assign.m_name);
		m_numCards = _assign.m_numCards;
		m_color = _assign.m_color;
		m_line = _assign.m_line;
		delete[] m_hand;
		m_hand = new Card[_assign.m_numCards];
		for (int i = 0; i < _assign.m_numCards; i++)
		{
			m_hand[i] = _assign.m_hand[i];
		}
		m_score = _assign.m_score;
	}
	return *this;
}


// Access a Card from the player's hand
// In:	_index		The index of the card being accessed
//		_card		A reference to store that value in
//
// Return: True if there was a card actually stored at that index.
//		   False if the index was "empty"
bool Player::GetCard(int _index, Card& _card) const
{

	if (m_numCards > _index)
	{
		_card = m_hand[_index];
		return true;
	}
	else //(_index >= m_numCards)
		return false;


}

/* Mutators */

// Change the player's name to the value passed in
void Player::SetName(const char* _name)
{
	delete[] m_name;

	m_name = new char[strlen(_name) + 1];
	strcpy_s(m_name, strlen(_name) + 1, _name);

}
// Update the player's score by some amount
void Player::AddToScore(int _add)
{
	m_score += _add;

}

// Add a Card to the player's hand
// In:	_card		The card being added
//
// Return: True if the card was successfully added to their hand
//		   False if the player's hand was already full
bool Player::AddCard(const Card& _card)
{

	if (m_numCards < m_maxCards)
	{
		m_hand[m_numCards] = _card;
		++m_numCards;
		return true;
	}
	else if (m_numCards == m_maxCards)
		return false;

	return true;
}

// Remove a Card from the player's hand
// In:	_index		The index of the card to remove from the array
//		_card		A reference to store that value in
//
// Return: True if there was a card actually stored at that index.
//		   False if the index was "empty"
bool Player::Discard(int _index, Card& _discard)
{


	if (m_numCards > _index)
	{
		_discard = m_hand[_index];

		for (size_t i = _index; i != m_numCards; i++)
		{
			m_hand[i] = m_hand[i + 1];
		}
		m_numCards--;

		return true;
	}
	else
		return false;
}

// Clear the player's hand so that they are holding 0 cards
void Player::Clear()
{
	m_numCards = 0;

}

// Display method (empty for this class)
void Player::Show() const
{

}
bool Player::IsCheatActive(short i) const
{
	if (m_cheats & (1 << i))
		return true;
	else
		return false;
}

void Player::Sort()
{
	Card temp;
	const int numcards = GetNumCards();
	Card* CardArr = new Card[GetNumCards()];
	for (int i = GetNumCards() - 1; GetNumCards(); i--)
	{
		Discard(i, CardArr[i]);
	}
	for (int i = 0; i < numcards; i++)
	{
		for (int j = 1; j < numcards; j++)
		{
			if (CardArr[j].GetSuit() < CardArr[j - 1].GetSuit())
			{
				temp = CardArr[j]; CardArr[j] = CardArr[j - 1];
				CardArr[j - 1] = temp;
			}
		}
	}
	for (int i = 0; i < numcards; i++)
	{
		for (int j = 1; j < numcards; j++)
		{
			if (CardArr[j].GetFace() < CardArr[j - 1].GetFace())
			{
				if (CardArr[j].GetSuit() == CardArr[j - 1].GetSuit())
				{
					temp = CardArr[j]; CardArr[j] = CardArr[j - 1];
					CardArr[j - 1] = temp;
				}
			}
		}
	}
	for (int i = 0; i < numcards; i++)
	{
		AddCard(CardArr[i]);
	}
	delete[] CardArr;
}