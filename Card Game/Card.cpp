#include "Card.h"

Card::Card(int _face, char _suit)
{
	m_face = _face;
	m_suit = _suit;
}

Card::~Card()
{
}

// Display a card to the screen
void Card::Show() const
{
	Console::BackgroundColor(White);
	if (m_suit == 3 || m_suit == 4)
		Console::ForegroundColor(Red);
	else
		Console::ForegroundColor(Black);
	if (m_face <= 10)
		cout << m_face;
	else if (m_face == 11)
		cout << 'J';
	else if (m_face == 12)
		cout << 'Q';
	else if (m_face == 13)
		cout << 'K';
	else if (m_face == 14)
		cout << 'A';

	cout << ' ' << m_suit << " ";
	Console::ResetColor();
}

// Insertion operator to display Cards with cout (or any other ostream object)
ostream &operator<<(ostream &_os, const Card &_rhs)
{
	Console::BackgroundColor(White);
	if (_rhs.GetSuit() > 4)
		Console::ForegroundColor(Black);
	else if (_rhs.GetSuit() < 5)
		Console::ForegroundColor(Red);

	if (_rhs.GetFace() <= 10)
		_os << _rhs.GetFace();
	else if (_rhs.GetFace() == 11)
		_os << 'J';
	else if (_rhs.GetFace() == 12)
		_os << 'Q';
	else if (_rhs.GetFace() == 13)
		_os << 'K';
	else if (_rhs.GetFace() == 14)
		_os << 'A';

	_os << ' ' << _rhs.GetSuit();
	Console::ResetColor();

	return _os;
}
