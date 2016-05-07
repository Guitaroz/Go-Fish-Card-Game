#include "Deck.h"
#include "Card.h"

// Default ctor
Deck::Deck()
{
	Initialize();
}

// Sets up the array of cards to be a unique deck
void Deck::Initialize()
{
	size_t count = 0;
	char suit = 3;
	for (int i = 0; i < m_numCards; i++)
	{
		m_cards[i].SetFace(count % 13 + 2);
		++count;
		m_cards[i].SetSuit(suit);
		if (count == 13 || count == 26 || count == 39 || count == 52)
			suit++;
	}
}

// Clears any cards that are already in the stack, shuffles the array, and then pushes
// all of the shuffled cards onto the stack.
void Deck::Shuffle()
{
	m_Stack.Clear();
	for (size_t i = 200; i; i--)
	{
		swap(m_cards[rand() % m_numCards], m_cards[rand() % m_numCards]);
	}

	for (int i = 0; i < m_numCards; i++)
	{
		m_Stack.Push(m_cards[i]);
	}
}

// Draw the top-most card off of the stack
// In:	_card			A reference to store the drawn card
//
// Return: True if a card was removed
//		   False if the deck was empty
bool Deck::Draw(Card& _card)
{
	if (IsEmpty())
		return false;
	else
	{
		m_Stack.Pop(_card);
		return true;
	}
}

// Clear the stack for re-use
void Deck::Clear()
{
	m_Stack.Clear();
}

// Check to see if there are any cards left in the stack
//
// Return: True if the stack is empty
//		   False if there are any cards left in the stack
bool Deck::IsEmpty() const
{
	if (m_Stack.GetCurrSize() == 0)
		return true;
	else
		return false;
}