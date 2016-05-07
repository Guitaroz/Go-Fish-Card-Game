#include "Human.h"



// Default ctor
Human::Human(const char* _name, int _line, ConsoleColor _color)
{
	Player::SetName(_name);
	Player::SetColor(_color);
}

// Display the computer's name and card backs
void Human::Show() const
{
	Console::ForegroundColor(GetColor());
	Console::SetCursorPosition(0, GetLine() - 1);
	cout << "\b\b" << Player::GetName();
	Console::SetCursorPosition(30, Player::GetLine() - 1);
	cout << "Score: " << GetScore() << endl;
	Console::ResetColor();
	Card temp;
	for (int i = 0; i < Player::GetNumCards(); i++)
	{
		Player::GetCard(i, temp);
		cout << temp << " ";
	}
	cout << '\t';
	Console::SetCursorPosition(Console::WindowWidth() / 2 - 10, Console::WindowHeight());
	Console::ForegroundColor(DarkCyan);
	cout << "Press Escape to Pause";

}


void Human::AddToScore(int _add)
{
	Player::AddToScore(_add);

	if (IsCheatActive(2))
		Player::AddToScore(_add);
}
