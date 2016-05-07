#include "Game.h"
#include "Human.h"
#include "Computer.h"
#include "stdafx.h"
#include <fstream>
#include "FmodSound.h"


// Default ctor
void Game::SetCoutLine()const
{
	Console::SetCursorPosition(0, m_players[m_numPlayers - 1]->GetLine() + 5);
	cout << "                                              ";
	Console::SetCursorPosition(0, m_players[m_numPlayers - 1]->GetLine() + 6);
	cout << "                                       ";
	Console::SetCursorPosition(0, m_players[m_numPlayers - 1]->GetLine() + 5);
}
Game::Game()
{
	m_state = GAME_MENU;
	int m_currPlayer = 0;
	srand(static_cast<unsigned int>(time(nullptr)));
	m_color = DarkGreen;
	m_paused = false;
	init = false;
}

// Dtor
Game::~Game()
{
	for (int i = 0; i < m_numPlayers; i++)
	{
		delete m_players[i];
	}

}

void Game::Run()
{
	bool replayMode = false;
	bool bRun = true;  // Bool to control whether the game should continue to run.
	LookNice();		   // Resize Console;

	Music.Init(2);
	Music.Load("ExpandMind.mp3");
	Beep = new SoundEffect("Bell.mp3");

	// Loop while our bool remains true.
	while (bRun)

	{
		switch (m_state)
		{
		case GAME_MENU:
			HomeMenu();
			break;

		case GAME_INSTRUCTIONS:
			DisplayInstructionScreen();
			m_state = GAME_MENU;
			break;

		case GAME_TOPSCORES:
			DisplayTopScores();
			m_state = GAME_MENU;
			break;

		case GAME_CHEATS:
			CheatsMenu();
			m_state = GAME_MENU;
			break;

		case GAME_INIT:
			InitializeGoFish();
			break;

		case GAME_PLAY:
			if (m_paused)
				m_paused = false;
			if (CheckGameOverAndPlayerHand())	//Check If hands are empty or if game is over
				break;
			clear();
			Sort();
			ShowAll();	//Display All Players

			/* HUMAN PLAY*/
			if (dynamic_cast<Human*>(m_players[m_currPlayer]))
			{
				PlayerInput();
				break;
			}
			/*COMPUTER PLAY*/
			else if (dynamic_cast<Computer*>(m_players[m_currPlayer]))
			{
				ComputerInput();
				break;
			}

		case GAME_END:
		{
			char * playername = new char[strlen(m_players[0]->GetName()) + 1];
			strcpy_s(playername, strlen(m_players[0]->GetName()) + 1, m_players[0]->GetName()); //Hold name before sorting in case we want to replay
			int HoldScores[4] = {};
			for (int i = 0; i < m_numPlayers; i++)	//Hold the scores before sorting in case we want to 
			{								// replay the game and carry the scores over. (cummulative scores)
			 HoldScores[i] = m_players[i]->GetScore();
			}
			
			CheckWhoWins();	//Sorts and Displays Winner
			Pause();
			DisplayTopScores();

			//Play Again??
			if (AskToPlayAgain())
			{
				 for (int i = 0; i < m_numPlayers; i++)
				 {
					 delete m_players[i];	//Delete and Create new players
				 }
				 CreatePlayers(playername);
				 delete[] playername;
				 Sleep(200);
				 //Keep Scores??
				 if (AskToKeepScores())
				 {				//Set scores in the new players
					 for (int i = 0; i < m_numPlayers; i++)
					 {
						 m_players[i]->AddToScore(HoldScores[i]);
					 }
				 }
				 m_state = GAME_MENU;
				 replayMode = true;
			 }
			 else
			 {
				 delete[] playername;
				 bRun = false;

			 }
			 break;
		}
		case GAME_QUIT:
			bRun = false;
			
			delete Beep;
			break;

		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::PlayerInput()
{
	/*If player has no cards, populate the hand*/
	CheckAndPopulate();

	if (m_players[m_currPlayer]->GetNumCards() == 0)
	{	//This Checks True if There were no cards left in Deck(stack)
		m_currPlayer++;
		return;
	}

	/*Ask Player Input*/
	Console::ForegroundColor(Red);
	Card temp, temp2;
	int cardChoice;
	int playerChoice;
	Sleep(150);
	if (m_players[0]->IsCheatActive(1))
	{
		int tempNum = rand() % 3;
		if (!tempNum)
		{
			Card temp;
			for (int i = 1; i < m_numPlayers; i++)
			{
				m_Deck.Draw(temp);
				m_players[i]->AddCard(temp);
			}
			Sort();
			ShowAll();
		}
	}
	if (!AskTheComputer(cardChoice, playerChoice))
		return;

	m_players[m_currPlayer]->GetCard(cardChoice, temp);
	ShowAll();
	SetCoutLine();
	Console::ForegroundColor(Yellow);
	cout << " You asked "; DisplayName(playerChoice);
	Console::ForegroundColor(Yellow);
	cout << "for a " << temp << "\b\b             ";
	Pause();
	if (m_paused)
		return;

	if (SearchCards(playerChoice, cardChoice))	//If Human guessed Correctly return.
	{
		SetCoutLine();
		Console::ForegroundColor(Green);
		Beep->Play();
		cout << " You Guessed it! Your Turn Continues!!";
		Pause();
		if (m_paused)
			return;
	}
	else
	{	Sort();
		ShowAll();
		SetCoutLine();
		DisplayName(playerChoice);
		Console::ForegroundColor(Yellow);
		cout << "did not have a card to pair.\n Go Fish!";
		Pause();
		if (m_paused)
			return;

		/*IF Search wasn't Successful, Draw From Deck and Check Again*/
		m_players[m_currPlayer]->GetCard(cardChoice, temp);

		if (!m_Deck.Draw(temp2))
		{
			SetCoutLine();
			cout << " Deck is Empty";
			Pause();
			if (m_paused)
				return;
			m_currPlayer++;
			return;
		}
		m_players[m_currPlayer]->AddCard(temp2);
		int tempScore = Score(m_players[m_currPlayer]);
		SetCoutLine();
		Console::ForegroundColor(Yellow);
		cout << " You Drew a " << temp2;
		Console::ForegroundColor(Yellow);
		if (temp == temp2)
		{
			Beep->Play();
			cout << " | Your Turn Continues!!!";
		}
		else
		{
			cout << " Your Turn Has Ended";
		}
		Pause();
		if (m_paused)
			return;

		if (tempScore > 0)
		{
			m_players[m_currPlayer]->AddToScore(tempScore); //Check For Pairs
			if (temp == temp2)			//If Card Drawn matched Card Asked, Player's Turn Continues
			{
				return;
			}
		}
		m_currPlayer++;
		return;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::ComputerInput()
{
	/*If Hand is empty, Repopulate.*/
	CheckAndPopulate();

	/*Double Check we dont have an empty hand*/
	if (m_players[m_currPlayer]->GetNumCards() == 0)
	{
		m_currPlayer++;
		if (m_currPlayer == m_numPlayers)
			m_currPlayer = 0;
		return;
	}
	/*Randomize Card Choice*/
	int _cardIndex = (rand() % m_players[m_currPlayer]->GetNumCards());

	/*Randomize Player Choice*/
	int _playerChoice;
	do 
	{ 
		_playerChoice = rand() % m_numPlayers; 
	}
	while (_playerChoice == m_currPlayer && m_players[_playerChoice]->GetNumCards() != 0);

	/*Ask Human for Card.*/
	Card temp;
	m_players[m_currPlayer]->GetCard(_cardIndex, temp);
	SetCoutLine();
	DisplayName(m_currPlayer);
	cout << "asks"; DisplayName(_playerChoice);
	Console::ForegroundColor(m_players[m_currPlayer]->GetColor());
	cout << "for\n a pair to "; temp.Show();
	Pause();
	if (m_paused)
		return;

	/*Search Cards*/
	if (SearchCards(_playerChoice, _cardIndex))
	{
		SetCoutLine();
		DisplayName(m_currPlayer); cout << "Guessed Right!\n His Turn Continues!";
		Pause();
		if (m_paused)
			return;
		return;
	}

	Card temp2 = temp;

	/*If Search wasnt Successful, Draw Card & Recheck pairs*/
	if (m_Deck.Draw(temp))
	{
		m_players[m_currPlayer]->AddCard(temp);
		int tempScore = Score(m_players[m_currPlayer]);
		if (tempScore > 0)
		{
			m_players[m_currPlayer]->AddToScore(tempScore);
			Sort();
			ShowAll();
			if (temp2 == temp)
			{
				SetCoutLine();
				DisplayName(m_currPlayer); cout << "drew the " << temp << '\n';
				DisplayName(m_currPlayer); cout << "Continues His Turn!!!";
				Pause();
				return;
			}	//Computer Continues Turn ONLY If It Drew a Card that It was Asking for.
			SetCoutLine();
			Console::ForegroundColor(DarkCyan);
			DisplayName(m_currPlayer); cout << "drew the " << temp;
			Console::ForegroundColor(DarkCyan);
			cout << " His turn ends. ";
			m_currPlayer++;
			if (m_currPlayer == m_numPlayers)
				m_currPlayer = 0;
			Pause();
			return;

		}
		Console::ForegroundColor(m_players[m_currPlayer]->GetColor());
		SetCoutLine();
		DisplayName(m_currPlayer); cout << "Turn Ended";
		Pause();
		m_currPlayer++;
		if (m_currPlayer == m_numPlayers)
			m_currPlayer = 0;
		return;
	}
	else
	{
		m_currPlayer++;
		if (m_currPlayer == m_numPlayers)
			m_currPlayer = 0;
		return;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
int Game::Score(Player* _player)
{
	int score = 0;

	Card temp1;
	Card temp2;
	int counter = 0;
	for (int i = 0; i < _player->GetNumCards(); i++)
	{
		for (int t = i + 1; t < _player->GetNumCards(); t++)
		{
			_player->GetCard(i, temp1);
			_player->GetCard(t, temp2);
			if (temp1 == temp2)
			{
				_player->Discard(t, temp2);
				_player->Discard(i, temp1);
				if (counter == 0)
				{
					//_player->ClearLine();
					Console::SetCursorPosition(Console::WindowWidth() / 2 - 4, _player->GetLine());
					Console::ForegroundColor(_player->GetColor());
					cout << _player->GetName() << " Discarded";
				}
				cout << " " << temp1 << " " << temp2 << " ";
				++score;
				++counter;
				i--;
			}
		}
	}
	cout << endl;
	return (score);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::AskTheComputer(int& _cardChoice, int& _playerChoice)
{
	cin.clear();
	Console::ForegroundColor(White);
	SetCoutLine();
	cout << " Pick a Card ";
	int y = m_players[m_currPlayer]->GetLine() + 1;
	Console::ForegroundColor(Green);
	int width = 0;
	int choice = 0;
	Card temp;
	Console::SetCursorPosition(0, y);
	m_players[m_currPlayer]->GetCard(choice, temp);
	ConsoleColor Arr[] = { White, Gray, DarkGray, DarkCyan, Green, DarkYellow, Cyan, Green, Magenta, White,
		Yellow, Red, DarkGray, Cyan, Magenta, DarkMagenta, DarkGreen, DarkYellow };
	char Gofish[4][40] =
	{
		"   ___    ___      ___ .. .__. ..   ..",
		" ||     ||   ||  ||    || ||   ||   ||",
		" || --| ||   ||  ||--- || '--. ||---||",
		" ||___| ||___||  ||    || .__| ||   ||"
	};


	int x = 0;
	int tempwidth = 45;
	int y2;
	if (temp.GetFace() == 10)
	{
		cout << "----";
		width++;
	}
	else
		cout << "---";
	Sleep(99);
	while (true)
	{
		y2 = 14;

		for (x = 0; x < 4; x++, y2++)
		{
			Console::ForegroundColor(Arr[rand() % _countof(Arr)]);
			Console::SetCursorPosition(tempwidth, y2);
			cout << Gofish[x];
			Console::ForegroundColor(Green);
			Sleep(120);

			if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
			{
				//m_players[m_currPlayer]->GetCard(choice, temp);
				if (temp.GetFace() == 10)
				{
					Console::SetCursorPosition(width - 1, y);
					cout << " ";
				}
				Console::SetCursorPosition(width, y);
				cout << "     ";
				if (choice == m_players[m_currPlayer]->GetNumCards() - 1)
				{
					width = 0;
					choice = 0;
				}
				else
				{
					width += 4;
					choice++;
				}
				Console::SetCursorPosition(width, y);
				m_players[m_currPlayer]->GetCard(choice, temp);
				if (temp.GetFace() == 10)
				{
					cout << "----";
					width++;
				}
				else
					cout << "---";

			}

			else if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)
			{

				if (temp.GetFace() == 10)
				{
					Console::SetCursorPosition(width - 1, y);
					cout << " ";
					width--;

				}
				Console::SetCursorPosition(width, y);
				cout << "     ";
				if (choice == 0)
				{
					width += m_players[m_currPlayer]->GetNumCards() * 4 - 4;
					choice = m_players[m_currPlayer]->GetNumCards() - 1;

					for (int i = 0; i < m_players[m_currPlayer]->GetNumCards(); i++)
					{
						Card temp2;
						m_players[m_currPlayer]->GetCard(i, temp2);	//Gotta INcrease Width if there is a 10 in the hand.
						if (temp2.GetFace() == 10)
							width++;
					}
				}
				else
				{
					width -= 4;
					choice--;
				}

				m_players[m_currPlayer]->GetCard(choice, temp);
				if (temp.GetFace() == 10)
				{

					Console::SetCursorPosition(width, y);
					cout << "----";

				}
				else
				{
					Console::SetCursorPosition(width, y);
					cout << "---";
				}

			}
			if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{

				SetCoutLine();
				Console::ForegroundColor(White);
				cout << "Pick a Player                       ";
				Console::ResetColor();
				Sleep(100);
				cin.clear();
				if (!PickAComputer(_playerChoice))
					return false;
				//clear();
				_cardChoice = choice;
				return true;
			}
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				m_state = GAME_MENU;
				m_paused = true;
				return false;
			}
		}
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::SearchCards(int _playerChoice, int _cardIndex)
{
	Card temp;
	m_players[m_currPlayer]->GetCard(_cardIndex, temp); // Store player Choice into Temp Card
	size_t numCardsToCompare = m_players[_playerChoice]->GetNumCards();
	Card compare;
	for (size_t i = 0; i < numCardsToCompare; i++)
	{
		m_players[_playerChoice]->GetCard(i, compare);
		if (compare == temp)
		{
			m_players[_playerChoice]->Discard(i, compare);
			m_players[m_currPlayer]->AddCard(compare);
			clear();
			m_players[m_currPlayer]->AddToScore(Score(m_players[m_currPlayer]));
			ShowAll();

			return true;
		}
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::ShowAll() const
{
	for (int i = 0; i < m_numPlayers; i++)
	{
		m_players[i]->Show();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::Sort()
{
	for (int i = 0; i < m_numPlayers; i++)
	{
		m_players[i]->Sort();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::Pause()
{
	cin.clear();
	ConsoleColor Arr[] = { White, Gray, DarkGray, DarkCyan, Green, DarkYellow, Cyan, Green, Magenta, White,
		Yellow, Red, DarkGray, Cyan, Magenta, DarkMagenta, DarkGreen, DarkYellow };
	//Sleep(150);
	char Gofish[4][40] =
	{
		"   ___    ___      ___ .. .__. ..   ..",
		" ||     ||   ||  ||    || ||   ||   ||",
		" || --| ||   ||  ||--- || '--. ||---||",
		" ||___| ||___||  ||    || .__| ||   ||"

	};
	int x = 0;
	int tempwidth = 45;
	int y;
	while (true)
	{
		y = 14;

		for (x = 0; x < 4; x++, y++)
		{
			Console::ForegroundColor(Arr[rand() % _countof(Arr)]);
			Console::SetCursorPosition(tempwidth, y);
			cout << Gofish[x];

			Sleep(120);
			if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{
				while (true)
				{
					if (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
						return;
				}
			}
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				m_paused = true;
				m_state = GAME_MENU;
				return;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::LookNice() const
{

	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFOEX info;
	info.cbSize = sizeof(info);
	GetConsoleScreenBufferInfoEx(consoleHandle, &info);
	COORD winSize;
	winSize.X = Console::WindowWidth() + 4;
	winSize.Y = 20;
	info.dwSize.X = info.dwMaximumWindowSize.X = info.srWindow.Right = winSize.X;
	info.dwSize.Y = info.dwMaximumWindowSize.Y = info.srWindow.Bottom = winSize.Y;
	SetConsoleScreenBufferInfoEx(consoleHandle, &info);
	SetConsoleScreenBufferSize(consoleHandle, winSize);
	SetConsoleTitle(L"THIS IS GO FISH"); // Unicode string!
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::PopulateHand(Player& _player)
{
	for (int i = 0; i < _player.GetDrawSize(); i++)
	{
		Card temp;
		if (m_Deck.Draw(temp))
			_player.AddCard(temp);

	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::IsPlayersHandEmpty() const
{
	for (int i = 0; i < m_numPlayers; i++)
	{
		if (m_players[i]->GetNumCards() != 0)
			return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CheckWhoWins()
{

	Player *temp = new Computer;
	char tempName[40];
	strcpy_s(tempName, 40, m_players[0]->GetName());
	SoundEffect* Applause = new SoundEffect("Applause.mp3");
	Applause->Play();
	
	UpdateTopScores();	//Dont Update Scores if Cheats were enabled.
	
	/*Bubble Sorting*/
	for (int q = 0; q < m_numPlayers; q++)
	{
		for (int r = 1; r < m_numPlayers; r++)
		{
			if (m_players[r]->GetScore() > m_players[r - 1]->GetScore())
			{
				*temp = *m_players[r];
				*m_players[r] = *m_players[r - 1];
				*m_players[r - 1] = *temp;
			}
		}
	}
	for (int i = 0; i < m_numPlayers; i++)
	{
		if (!_strcmpi(m_players[i]->GetName(), tempName))
		{
			m_currPlayer = i; //Need this For OverwriteScore Function. (keeps track of the human when game has ended)
		}
	}
	int x = 3;
	Console::Clear();
	Console::SetCursorPosition(Console::WindowWidth() / 2 - 9, x);
	cout << "______Results______"; x++;

	/*Display Results*/
	for (int i = 0; i < m_numPlayers; x++, i++)
	{
		Console::SetCursorPosition(Console::WindowWidth() / 2 - 9, x);
		Console::ForegroundColor(m_players[i]->GetColor());
		cout << (i + 1) << ") " << m_players[i]->GetName() << " Score: " << m_players[i]->GetScore();
	}
	++x;
	/*Fancy Win Display*/
	for (int i = 0; i <= 50; i++)
	{
		Console::ForegroundColor(i % 15 + 1);	// loop and change colors.
		Console::SetCursorPosition(Console::WindowWidth() / 2 - 9, x);
		cout << "Winner is " << m_players[0]->GetName() << endl;

		Sleep(50);
	}
	Console::SetCursorPosition(Console::WindowWidth() / 2 - 5, ++x);
	cout << "Game Over";
	delete temp;

	delete Applause;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CreatePlayers()
{
	char name[50];
	while (true)
	{
		Console::SetCursorPosition(0, 17);
		Console::ForegroundColor(DarkCyan);
		cout << "\t         Enter your name: ____________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
		Console::CursorVisible(false);

		cin.get(name, 50);
		if (cin.good())
		{
			break;
		}
		cin.clear();
		cin.ignore(LLONG_MAX, '\n');
	}

	for (;;)
	{
		Console::SetCursorPosition(60, 17);
		cout << "                ";
		Console::SetCursorPosition(0, 17);
		Console::ForegroundColor(DarkCyan);
		cout << "\t How many payers would you like to play against? (Max 4 players): _\b";
		if (cin >> m_numPlayers && m_numPlayers > 0 && m_numPlayers <= 4)
		{
			cin.clear();
			cin.ignore(LLONG_MAX, '\n');
			break;
		}
		cin.clear();
		cin.ignore(LLONG_MAX, '\n');
	}
	int x = 4; //Console use
	char * ComputerNames[3] = { "Z-cardo", "Bender", "Chuck Norris" };
	ConsoleColor compColors[3] = { Cyan, Red, Magenta };

	for (int i = 0; i < m_numPlayers; i++)
	{
		if (!i)
		{
			m_players[i] = new Human(name, 1);

		}
		else
		{
			m_players[i] = new Computer(ComputerNames[i - 1], x, compColors[i - 1]);
			x += 3;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CreatePlayers(char * name)
{
	int x = 4; //Console use
	char * ComputerNames[3] = { "Z-cardo", "Bender", "Chuck Norris" };
	ConsoleColor compColors[3] = { Cyan, Red, Magenta };

	for (int i = 0; i < m_numPlayers; i++)
	{
		if (!i)
		{
			m_players[i] = new Human(name, 1);

		}
		else
		{
			m_players[i] = new Computer(ComputerNames[i - 1], x, compColors[i - 1]);
			x += 3;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::DisplayName(int i) const
{
	Console::ForegroundColor(m_players[i]->GetColor());
	cout << " " << m_players[i]->GetName() << " ";

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::PickAComputer(int& _playerChoice)
{

	int choice = 1;
	int y = m_players[choice]->GetLine() - 1;
	Console::ForegroundColor(Cyan);
	const int width = 13;
	Console::SetCursorPosition(width, y);
	cout << "\xAE\xAE";
	ConsoleColor Arr[] = { White, Gray, DarkGray, DarkCyan, Green, DarkYellow, Cyan, Green, Magenta, White,
		Yellow, Red, DarkGray, Cyan, Magenta, DarkMagenta, DarkGreen, DarkYellow };
	char Gofish[4][40] =
	{
		"   ___    ___      ___ .. .__. ..   ..",
		" ||     ||   ||  ||    || ||   ||   ||",
		" || --| ||   ||  ||--- || '--. ||---||",
		" ||___| ||___||  ||    || .__| ||   ||"
	};


	int x = 0;
	int tempwidth = 45;
	int y2;
	while (true)
	{
		y2 = 14;
		for (x = 0; x < 4; x++, y2++)
		{
			Console::ForegroundColor(Arr[rand() % _countof(Arr)]);
			Console::SetCursorPosition(tempwidth, y2);
			cout << Gofish[x];

			Sleep(120);
			if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000)
			{

				Console::SetCursorPosition(width, y);
				cout << "     ";
				if (choice <= 1)
				{
					choice = m_numPlayers - 1;
					y = m_players[choice]->GetLine() - 1;
				}
				else
				{
					choice--;
					y = m_players[choice]->GetLine() - 1;
				}
				Console::ForegroundColor(m_players[choice]->GetColor());
				Console::SetCursorPosition(width, y);
				cout << "\xAE\xAE";
				Sleep(299);
			}

			else if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)
			{
				Console::SetCursorPosition(width, y);
				cout << "     ";
				if (choice >= m_numPlayers - 1)
				{
					choice = 1;
					y = m_players[choice]->GetLine() - 1;
				}
				else
				{
					choice++;
					y = m_players[choice]->GetLine() - 1;
				}
				Console::ForegroundColor(m_players[choice]->GetColor());
				Console::SetCursorPosition(width, y);
				cout << "\xAE\xAE";

				Sleep(300);
			}
			if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{

				while (true)
				{
					if (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
					{
						_playerChoice = choice;
						cin.ignore(LLONG_MAX, '\n');
						return true;
					}
				}
			}
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			{
				m_state = GAME_MENU;
				m_paused = true;
				return false;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CheckAndPopulate()
{
	while (m_players[m_currPlayer]->GetNumCards() == 0)
	{
		if (m_Deck.IsEmpty())
		{
			return; //return out of this while loop
		}
		PopulateHand(*m_players[m_currPlayer]);
		m_players[m_currPlayer]->AddToScore(Score(m_players[m_currPlayer]));
		Sort();
		ShowAll();

	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::CheckGameOverAndPlayerHand()
{
	if (m_Deck.IsEmpty() && IsPlayersHandEmpty()) // If Deck is empty and AND All player's hands empty.
	{
		m_state = GAME_END;
		return true;
	}

	if (m_Deck.IsEmpty() && m_players[m_currPlayer]->GetNumCards() == 0) //If Deck is Empty and current Players Hand is empty:: currplayer++
	{
		m_currPlayer++;
		if (m_currPlayer == m_numPlayers)
			m_currPlayer = 0;
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::DisplayInstructionScreen()
{
	Console::ForegroundColor(Yellow);
	{

		cin.ignore(LLONG_MAX, '\n');
		Console::Clear();
		cout << "\n"
			<< "\t    _______________________This is Go Fish_____________________\n"
			<< "\t   |                                                           |\n"
			<< "\t   |                                                           |\n"
			<< "\t   | The objective of this game is to score the most points by |\n"
			<< "\t   | making pairs. Pick a card on your hand you want to form a |\n"
			<< "\t   | pair with. Then select which player you wish to ask.      |\n"
			<< "\t   | If the player asked doesn't have the card you want, then  |\n"
			<< "\t   | you must go fish a card from the deck.                    |\n"
			<< "\t   | Your turn will only continue if you drew the same card you|\n"
			<< "\t   | asked for                                                 |\n"
			<< "\t   |                                                           |\n"
			<< "\t   |     The game ends once all the cards have been paired.    |\n"
			<< "\t   |                                                           |\n"

			<< "\t   |     "; Console::ForegroundColor(DarkCyan);
		cout << "Controls: ";
		Console::ForegroundColor(DarkCyan);
		cout << "ARROW"; Console::ForegroundColor(Yellow); cout <<
			" keys or "; Console::ForegroundColor(DarkCyan); cout << "'AWDS'"; Console::ForegroundColor(Yellow);
		cout << "keys to navigate.       | \n";
		cout << "\t   |                      "; Console::ForegroundColor(DarkCyan); cout << "'ENTER'";
		Console::ForegroundColor(Yellow); cout << " to Proceed.                  | \n";
		cout << "\t   |                                                           |\n" 
			 << "\t   |_________________________Good Luck!________________________|\n"
			<< "\n";
	}
	while (true)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		while (true)
		{
			if (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
			{
				return;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::InitializeGoFish()
{
	Console::Clear();
	m_currPlayer = 0;
	if (m_players[0]->IsCheatActive(3))
		m_currPlayer = rand() % m_numPlayers;

	m_Deck.Initialize();
	m_Deck.Shuffle();
	for (int i = 0; i < m_numPlayers; i++)
	{
		PopulateHand(*m_players[i]);	//Deal Cards
	}
	Sort();
	ShowAll();
	cin.ignore(); //Show Players Hands
	Pause();

	 clear();

	for (int i = 0; i < m_numPlayers; i++)	//Discard Original Pairs
	{
		m_players[i]->AddToScore(Score(m_players[i]));
	}
	Sort();
	ShowAll();			//Show Players Again
	Pause();

	m_state = GAME_PLAY;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::AskToPlayAgain() const
{
	Sleep(200);
	bool temp = true;
	int y = 19;
	int width = Console::WindowWidth() / 2 - 10;
	Console::SetCursorPosition(width - 5, y - 3);
	Console::ForegroundColor(DarkCyan);
	cout << "Would you like to play again? ";
	Console::SetCursorPosition(width + 3, y - 1);
	cout << "Yes";
	Console::SetCursorPosition(width + 14, y - 1);
	cout << "No";
	Console::SetCursorPosition(width + 3, y);
	cout << "---";
	while (true)
	{
		Console::SetCursorPosition(width - 3, 9);

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			Console::SetCursorPosition(width + 3, y);
			cout << "   ";
			Console::SetCursorPosition(width + 14, y);
			cout << "--";
			temp = false;

		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			Console::SetCursorPosition(width + 14, y);
			cout << "  ";
			Console::SetCursorPosition(width + 3, y);
			cout << "---";
			temp = true;
		}

		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			break;
	}
	return temp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::AskToKeepScores() const
{
	Sleep(150);
	bool temp = true;
	int y = 19;
	int width = Console::WindowWidth() / 2 - 10;
	Console::SetCursorPosition(width - 5, y - 3);
	Console::ForegroundColor(DarkCyan);
	cout << "Would you like to carry the scores over? ";
	Console::SetCursorPosition(width + 3, y - 1);
	cout << "Yes";
	Console::SetCursorPosition(width + 14, y - 1);
	cout << "No";
	Console::SetCursorPosition(width + 3, y);
	cout << "---";
	while (true)
	{
		Console::SetCursorPosition(width - 3, 9);

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			Console::SetCursorPosition(width + 3, y);
			cout << "   ";
			Console::SetCursorPosition(width + 14, y);
			cout << "--";
			temp = false;

		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			Console::SetCursorPosition(width + 14, y);
			cout << "  ";
			Console::SetCursorPosition(width + 3, y);
			cout << "---";
			temp = true;
		}

		else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			break;
	}
	return temp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateTopScores()
{
	if (!(m_players[0]->IsCheatActive(0) || m_players[0]->IsCheatActive(1) || m_players[0]->IsCheatActive(2) || m_players[0]->IsCheatActive(3)
		|| m_players[0]->IsCheatActive(4) || m_players[0]->IsCheatActive(5) || m_players[0]->IsCheatActive(6) || m_players[0]->IsCheatActive(7)))
	{
			ofstream WriteScores("TopScores.txt.", std::ios_base::app);

			WriteScores << m_players[m_currPlayer]->GetName() << '\n' << m_players[m_currPlayer]->GetScore();
			WriteScores.close();
		
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::DisplayTopScores()
{
	char HoldMeString[40]; int HoldMeInt, x = 10;
	ifstream GetTopScores("TopScores.txt", std::ios_base::in);
	GetTopScores.seekg(0, ios::beg);

	char FullNamesList[11][40];
	int Scores[20] = {};
	if (m_state == GAME_END)
	{
		strcpy(FullNamesList[0], m_players[m_currPlayer]->GetName());
		Scores[0] = m_players[m_currPlayer]->GetScore();
	}
	else
	{
		strcpy(FullNamesList[0], m_players[0]->GetName());
		Scores[0] = m_players[0]->GetScore();
	}
	for (int i = 1; i <= x; i++)
	{
		GetTopScores.getline(FullNamesList[i], 25);
		GetTopScores >> Scores[i];
		GetTopScores.getline(HoldMeString, 25);
	}
	GetTopScores.close();
	/*Bubble Sorting Scores*/
	for (int q = 0; q <=x; q++)
	{
		for (int r = 1; r <= x; r++)
		{
			if (Scores[r] > Scores[r - 1])
			{
				HoldMeInt = Scores[r]; strcpy_s(HoldMeString, 40, FullNamesList[r]);
				Scores[r] = Scores[r - 1];  strcpy_s(FullNamesList[r], 40, FullNamesList[r - 1]);
				Scores[r - 1] = HoldMeInt; strcpy_s(FullNamesList[r - 1], 40, HoldMeString);
			}
		}
	}
	
	/*Print Scores*/
	Console::Clear();
	int width = Console::WindowWidth() / 2 - 10;
	Console::SetCursorPosition(width + 5, 1);
	Console::ForegroundColor(DarkCyan);
	int y = 4;

	cout << "Top Scores:";
	for (int i = 0; i < x; i++)	//We only save the top 10 scores.
	{
		Console::SetCursorPosition(width - 4, i + y);
		Console::ForegroundColor(White);
		if (m_state == GAME_END)
		{
			if (!strcmpi(FullNamesList[i], m_players[m_currPlayer]->GetName()))
				Console::ForegroundColor(Green);
		}
		else
		{
			if (!strcmpi(FullNamesList[i], m_players[0]->GetName()))
				Console::ForegroundColor(Green);
		}
		cout << FullNamesList[i];
		Console::SetCursorPosition(width + 24, i + y);
		Console::ForegroundColor(Yellow);
		cout << Scores[i];
	}

	if (m_state == GAME_END)
		OverWriteScores(FullNamesList, Scores);
	
	while (true)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		while (true)
		{
			if (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
			{
				return;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::OverWriteScores(const char _Arr[11][40], int _scores[20]) const
{
	if (!(m_players[0]->IsCheatActive(0) || m_players[0]->IsCheatActive(1) || m_players[0]->IsCheatActive(2) || m_players[0]->IsCheatActive(3)
		|| m_players[0]->IsCheatActive(4) || m_players[0]->IsCheatActive(5) || m_players[0]->IsCheatActive(6) || m_players[0]->IsCheatActive(7)))
	{
		ofstream OverWriteScores("TopScores.txt", std::ios_base::out);
		for (int i = 0; i < 10; i++)
		{
			OverWriteScores << _Arr[i] << '\n' << _scores[i] << '\n';
		}

		OverWriteScores.close();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::CheatsMenu()
{
	const char* cheatsDescription[5] =
	{
		"- Slight Chance to display Computer's Card",
		"- Chance for Computers to draw at the begginning of your turn",
		"- Each pair you score is worth 2 points",
		"- Randomize who starts",
		"- Debug Mode (Show All Cards)"
	};
	Console::Clear();
	Console::ForegroundColor(Magenta);
	cout << "\n"
		<< "\t    _________________________Cheats Menu_____________________________\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |                                                                 |\n"
		<< "\t   |______________________Because we can_____________________________|\n"
		<< "\n";
	int y = 4;
	int width;
	bool escape = false;
	Console::ForegroundColor(DarkCyan);
	Console::SetCursorPosition(18, 16);
	cout << "Activate Cheat Codes:";
	Console::ForegroundColor(Gray);
	cout << "Hold the 'G', 'F' keys, \n\tthen Press"
		<< "'A','B','C','D' 'E', respectively to activate the cheat codes";
	Console::ForegroundColor(Red);
	cout << "\n\t       Activating any of these will disable storing your score";

	for (;;)
	{
		if (escape)
			break;
		y = 4;
		for (size_t i = 0; i < _countof(cheatsDescription); i++, y++)
		{
			width = 56 - (strlen(cheatsDescription[i]) / 2) - 12;
			if (m_players[0]->IsCheatActive(i))
				Console::ForegroundColor(Green);
			else
				Console::ResetColor();
			Console::SetCursorPosition(width, y++);
			cout << cheatsDescription[i];
		}
		Console::CursorVisible(false);
		Sleep(500);
		while (true)
		{

			if (GetAsyncKeyState('A') & 0X8000 && GetAsyncKeyState('G') & 0x8000
				&& GetAsyncKeyState('F') & 0x8000)
			{
				m_players[0]->ToggleCheat(0);
				break;
			}
			else if (GetAsyncKeyState('B') & 0X8000 && GetAsyncKeyState('G') & 0x8000
				&& GetAsyncKeyState('F') & 0x8000)
			{
				m_players[0]->ToggleCheat(1);
				break;
			}
			else if (GetAsyncKeyState('C') & 0X8000 && GetAsyncKeyState('G') & 0x8000
				&& GetAsyncKeyState('F') & 0x8000)
			{
				m_players[0]->ToggleCheat(2);
				break;
			}
			else if (GetAsyncKeyState('D') & 0X8000 && GetAsyncKeyState('G') & 0x8000
				&& GetAsyncKeyState('F') & 0x8000)
			{
				m_players[0]->ToggleCheat(3);
				break;
			}

			else if (GetAsyncKeyState('E') & 0X8000 && GetAsyncKeyState('G') & 0x8000
				&& GetAsyncKeyState('F') & 0x8000)
			{
				m_players[0]->ToggleCheat(4);
				break;
			}
			else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{
				escape = true;
				break;
			}
		}
	}
	m_state = GAME_MENU;
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::HomeMenu()
{
	Console::Clear();
	Console::ForegroundColor(Magenta);

	//     0         1         2         3         4         5         6         7         8  3
	cout << "                      __________________________________________                   "  //1
		<< "                     ||                                          ||                 "	//2
		<< "                     ||                                          ||                 "	//3
		<< "                     ||                                          ||                 "	//4
		<< "                     ||                                          ||                 "	//5
		<< "                     ||__________________________________________||                 ";	//6
	Console::ForegroundColor(Magenta);
	cout << "                     ---------------------------------------------                  "; Console::ForegroundColor(Yellow);
	cout << "                                      _________                                     "//8
		<< "                                     |PLAY GAME|                                    "	//9
		<< "                                      ¯¯¯¯¯¯¯¯¯                                     "	//0
		<< "                                                                                    "	//1
		<< "                                                                                    "	//2
		<< "                                                                                    "	//3
		<< "           ____________              __________                    ______           "	//4
		<< "          |INSTRUCTIONS|            |TOP SCORES|                  |CHEATS|          "	//5
		<< "           ¯¯¯¯¯¯¯¯¯¯¯¯              ¯¯¯¯¯¯¯¯¯¯                    ¯¯¯¯¯¯           "	//6
		<< "                                                                                    "	//7
		<< "                                                                                    "	//8
		<< "                                                                                    ";	//9
	Console::ForegroundColor(Cyan);
	Console::SetCursorPosition(0, 0);
	int tempwidth = 23;
	int x = 0;
	char Gofish[4][42] =
	{
		"   ___    ___      ___ .. .__. ..   ..  ",
		" ||     ||   ||  ||    || ||   ||   ||  ",
		" || --| ||   ||  ||--- || '--. ||---||  ",
		" ||___| ||___||  ||    || .__| ||   ||  "

	};
	for (size_t i = 1; x < 4; i++)
	{
		Console::SetCursorPosition(tempwidth, i);
		cout << Gofish[x++];

	}

	if (!init)
	{
		Music.Play(false);
		CreatePlayers();
		init = true;
	}

	int choice = 1;
	int y = 14;
	Console::SetCursorPosition(0, 17);
	cout << "                                                                                    "	//8
		<< "                                                                                     ";	//9
	Console::ForegroundColor(DarkCyan);
	cout << "                              Press Q to quit the game";
	Console::ForegroundColor(Red);
	int width = 36;
	Console::SetCursorPosition(width, y - 7);
	cout << " _________";
	Console::SetCursorPosition(width, y - 6);
	cout << "|PLAY GAME|";
	Console::SetCursorPosition(width, y - 5);
	cout << " ¯¯¯¯¯¯¯¯¯";
	choice = 0;
	Sleep(250);

	int j[4] = { 36, 9, 35, 65 };
#pragma region HomeInput

	ConsoleColor Arr[] = { White, Gray, DarkGray, DarkCyan, Green, DarkYellow, Cyan, Green, Magenta, White,
		Yellow, Red, DarkGray, Cyan, Magenta, DarkMagenta, DarkGreen, DarkYellow };
	float vol = 1.0f;
	while (true)
	{
		Music.SetVolume(vol);
		for (x = 0; x < 4; x++)
		{
			Console::ForegroundColor(Arr[rand() % _countof(Arr)]);
			Console::SetCursorPosition(tempwidth, x + 1);
			cout << Gofish[x];

			Sleep(120);
			Console::CursorVisible(false);
			if (GetAsyncKeyState('=') & 0x800)
			{
				if (vol < 1)
					vol += .1f;
				continue;
			}
			if (GetAsyncKeyState('-') & 0x800)
			{
				if (vol > 0)
					vol -= .1f;
				continue;
			}
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
			{
				if (width == 36)
					continue;
				Console::ForegroundColor(Yellow);
				Console::SetCursorPosition(j[0], y - 7);
				cout << " _________";
				Console::SetCursorPosition(j[0], y - 6);
				cout << "|PLAY GAME|";
				Console::SetCursorPosition(j[0], y - 5);
				cout << " ¯¯¯¯¯¯¯¯¯";
				if (width == j[2])
				{
					Console::ForegroundColor(Yellow);
					Console::SetCursorPosition(width, y - 1);
					cout << " __________ ";
					Console::SetCursorPosition(width, y);
					cout << "|TOP SCORES|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯";
					width = j[3];

					Console::ForegroundColor(Red);
					Console::SetCursorPosition(width, y - 1);
					cout << " ______";
					Console::SetCursorPosition(width, y);
					cout << "|CHEATS|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯";
					choice = 3;
					Sleep(190);//;////300);
				}
				else if (width == j[1])
				{
					Console::ForegroundColor(Yellow);
					Console::SetCursorPosition(width, y - 1);
					cout << " ____________ ";
					Console::SetCursorPosition(width, y);
					cout << "|INSTRUCTIONS|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯¯¯";

					Console::SetCursorPosition(width, y);
					width = j[2];

					Console::ForegroundColor(Red);
					Console::SetCursorPosition(width, y - 1);
					cout << " __________ ";
					Console::SetCursorPosition(width, y);
					cout << "|TOP SCORES|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯";
					choice = 2;
					Sleep(190);//;////200);
				}
				else if (width == j[3])
				{
					Console::ForegroundColor(Yellow);
					Console::SetCursorPosition(width, y - 1);
					cout << " ______";
					Console::SetCursorPosition(width, y);
					cout << "|CHEATS|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯";

					width = j[1];
					Console::ForegroundColor(Red);
					Console::SetCursorPosition(width, y - 1);
					cout << " ____________ ";
					Console::SetCursorPosition(width, y);
					cout << "|INSTRUCTIONS|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯¯¯";
					choice = 1;
					Sleep(190);//;////300);
				}
			}

			else if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)
			{
				if (width == j[0])
					continue;
				Console::ForegroundColor(Yellow);
				Console::SetCursorPosition(j[0], 7);
				cout << " _________";
				Console::SetCursorPosition(j[0], 8);
				cout << "|PLAY GAME|";
				Console::SetCursorPosition(j[0], 9);
				cout << " ¯¯¯¯¯¯¯¯¯";
				if (width == j[3])
				{
					Console::ForegroundColor(Yellow);
					Console::SetCursorPosition(width, y - 1);
					cout << " ______";
					Console::SetCursorPosition(width, y);
					cout << "|CHEATS|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯";

					width = j[2];
					Console::ForegroundColor(Red);
					Console::SetCursorPosition(width, y - 1);
					cout << " __________ ";
					Console::SetCursorPosition(width, y);
					cout << "|TOP SCORES|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯";
					choice = 2;
					Sleep(190);//;////300);


				}
				else if (width == j[2])
				{
					Console::ForegroundColor(Yellow);
					Console::SetCursorPosition(width, y - 1);
					cout << " __________ ";
					Console::SetCursorPosition(width, y);
					cout << "|TOP SCORES|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯";

					width = j[1];
					Console::ForegroundColor(Red);
					Console::SetCursorPosition(width, y - 1);
					cout << " ____________ ";
					Console::SetCursorPosition(width, y);
					cout << "|INSTRUCTIONS|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯¯¯";
					choice = 1;
					Sleep(190);//;////200);
				}
				else if (width == j[1])
				{

					Console::ForegroundColor(Yellow);
					Console::SetCursorPosition(width, y - 1);
					cout << " ____________ ";
					Console::SetCursorPosition(width, y);
					cout << "|INSTRUCTIONS|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯¯¯";

					width = j[3];
					Console::ForegroundColor(Red);
					Console::SetCursorPosition(width, y - 1);
					cout << " ______";
					Console::SetCursorPosition(width, y);
					cout << "|CHEATS|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯";
					choice = 3;
					Sleep(190);//;////300);

				}
			}
			else if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000)
			{
				if (width != j[0])
				{
					Console::SetCursorPosition(j[1], y - 1);
					Console::ForegroundColor(Yellow);
					cout << " ____________              __________                    ______            ";
					Console::SetCursorPosition(j[1], y);
					cout << "|INSTRUCTIONS|            |TOP SCORES|                  |CHEATS|           ";
					Console::SetCursorPosition(j[1], y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯¯¯              ¯¯¯¯¯¯¯¯¯¯                    ¯¯¯¯¯¯            ";	//6
					Console::ForegroundColor(Red);
					width = j[0];
					Console::SetCursorPosition(width, y - 7);
					cout << " _________";
					Console::SetCursorPosition(width, y - 6);
					cout << "|PLAY GAME|";
					Console::SetCursorPosition(width, y - 5);
					cout << " ¯¯¯¯¯¯¯¯¯";
					choice = 0;
				}

			}
			else if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)
			{
				if (width == j[0])
				{
					Console::ForegroundColor(Yellow);
					Console::SetCursorPosition(j[0], y - 7);
					cout << " _________";
					Console::SetCursorPosition(j[0], y - 6);
					cout << "|PLAY GAME|";
					Console::SetCursorPosition(j[0], y - 5);
					cout << " ¯¯¯¯¯¯¯¯¯";
					width = j[2];

					Console::ForegroundColor(Red);
					Console::SetCursorPosition(width, y - 1);
					cout << " __________ ";
					Console::SetCursorPosition(width, y);
					cout << "|TOP SCORES|";
					Console::SetCursorPosition(width, y + 1);
					cout << " ¯¯¯¯¯¯¯¯¯¯";
					choice = 2;
					Sleep(190);//;////200);
				}
				else
					continue;

			}
#pragma endregion
			if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			{
				while (true)
				{
					if (!(GetAsyncKeyState(VK_RETURN) & 0x8000))
					{
						switch (choice)
						{
						case 0:
							if (!m_paused)
								m_state = GAME_INIT;
							else
								m_state = GAME_PLAY;

							return;
						case 1:
							m_state = GAME_INSTRUCTIONS;
							return;
						case 2:
							m_state = GAME_TOPSCORES;
							return;
						case 3:
							m_state = GAME_CHEATS;
							return;
						}
					}
				}
			}
			if (GetAsyncKeyState('Q') & 0x8000)
			{
				m_state = GAME_QUIT;
				return;
			}


		}

	}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::clear() const
{
	Console::SetCursorPosition(0, 0);
	cout << "                                                                                   "  //1
		<< "                                                                                    "	//2
		<< "                                                                                    "	//3
		<< "                                                                                    "	//4
		<< "                                                                                    "	//5
		<< "                                                                                    "	//6
		<< "                                                                                    "
		<< "                                                                                    "//8
		<< "                                                                                    "	//9
		<< "                                                                                    "	//0
		<< "                                                                                    "	//1
		<< "                                                                                    "	//2
		<< "                                                                                    "	//3
		<< "                                                                                    "//4
		<< "                                               \n"//5
		<< "                                              \n"	//6
		<< "                                              \n"	//7
		<< "                                              \n"	//8
		<< "                                                                                    "
		<< "                                                                             ";//9
}