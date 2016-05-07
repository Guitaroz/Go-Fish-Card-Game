#include "Computer.h"

// Default ctor
Computer::Computer(const char* _name, int _line, ConsoleColor _color)
{
	Player::SetLine(_line);
	Player::SetName(_name);
	Player::SetColor(_color);
}

// Display the computer's name and card backs
void Computer::Show() const
{

	Console::ForegroundColor(GetColor());
	Console::SetCursorPosition(0, GetLine() - 1);
	cout << "\b\b" << Player::GetName();
	Console::SetCursorPosition(30, Player::GetLine() - 1);
	cout << "Score: " << GetScore() << endl;

	bool temp = false;
	if (IsCheatActive(0))
	{
		int intArr[50] = { 1, };
		temp = intArr[rand() % 50];
	}

	for (int i = 0; i < Player::GetNumCards(); i++)
	{
		Console::ForegroundColor(Black);
		Console::BackgroundColor(White);

		if (temp || IsCheatActive(4))
		{
			Card temp;
			GetCard(i, temp);
			cout << temp;
		}
		else
			cout << "\xB2\xB2\xB2";
		Console::BackgroundColor(Black);
		cout << " ";
	}



	cout << '\t';
	Console::ResetColor();


}

