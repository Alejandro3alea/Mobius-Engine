#include "ColorConsole.h"

void PrintColoredText(const std::string& text, const ConsoleColor color)
{
#ifdef _WIN32
	int col = 15;
	switch (color)
	{
	case Red:
		col = 12; break;
	case Green:
		col = 10; break;
	case Yellow:
		col = 6; break;
	case Blue:
		col = 3; break;
	case Purple:
		col = 5; break;
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, col);
#endif
	std::cout << text << std::endl;		

#ifdef _WIN32
	SetConsoleTextAttribute(hConsole, 15);	
#endif
}

void PrintSuccess(const std::string& text)
{
	PrintColoredText(text, ConsoleColor::Green);
}

void PrintDebug(const std::string& text)
{
	PrintColoredText("[DEBUG] - " + text, ConsoleColor::Default);
}

void PrintWarning(const std::string& text)
{
	PrintColoredText("[WARNING] - " + text, ConsoleColor::Yellow);
}

void PrintError(const std::string& text)
{
	PrintColoredText("[ERROR] - " + text, ConsoleColor::Red);
}