#include "stdafx.h"

#include "Errors.h"

vector<string> Errors::m_ErrorMsgs;

/**/
/*
Errors::InitErrorReporting()
NAME

	Errors::InitErrorReporting - emptys the error messages

SYNOPSIS

	void Errors::InitErrorReporting();

DESCRIPTION

	This function will clear m_ErrorMsgs vector if it is not already empty.

RETURNS

	nothing since it is a void
*/
/**/

void Errors::InitErrorReporting() 
{
	if (m_ErrorMsgs.empty() == false)
	{
		m_ErrorMsgs.clear();
	}
}

/**/
/*
Errors::RecordError(string a_emsg)
NAME

	Errors::RecordError - records the error messages

SYNOPSIS

	void Errors::RecordError(string a_emsg);
	a_emsg -> the message being added to the errors vector
DESCRIPTION

	This function will add error messages to the m_ErrorMsgs vector.

RETURNS

	nothing since it is a void
*/
/**/

void Errors::RecordError(string a_emsg)
{
	m_ErrorMsgs.push_back(a_emsg);
	cerr << a_emsg << endl;
	// Play Windows XP error sound after adding error to the list
	PlaySound(TEXT("Error.wav"), NULL, SND_SYNC);
}

/**/
/*
Errors::DisplayErrors()
NAME

	Errors::DisplayErrors - Displays ALL the error messages

SYNOPSIS

	void Errors::DisplayErrors();

DESCRIPTION

	This function will Displays ALL the error messages from the m_ErrorMsgs vector.

RETURNS

	nothing since it is a void
*/
/**/

void Errors::DisplayErrors()
{
	// full_size is used to modify the properties of the console window
	HWND full_size = GetConsoleWindow();
	// make the window full screen
	//ShowWindow(full_size, SW_MAXIMIZE);
	// change the screen color and font details
	Visual_error_effects();
	cout << setw(8) << "YOUR VC8000 HAS RUN INTO A PROBLEM AND NEEDS TO RESTART" << endl;
	cout << endl;
	// draw a skull and cross bones
	Fail_picture("Skull.txt");
	// Display ALL errors
	for (int i = 0; i < m_ErrorMsgs.size(); i++)
	{
		cerr << m_ErrorMsgs.at(i) << endl;
	}
	// Play an angry Donald Duck file since errors are frustrating
	PlaySound(TEXT("Angry.wav"), NULL, SND_SYNC);
	cout << endl;
	cout << "PRESS ANY KEY TO RESTART YOUR VC8000" << endl;
	// terminate program
	exit(1);
}

/**/
/*
Errors::Is_corrupt()
NAME

	Errors::Is_corrupt - Deterines if there are errors

SYNOPSIS

	bool Errors::Is_corrupt();

DESCRIPTION

	This function will determine if errors are present based on the m_ErrorMsgs vector's size. 
	IF the size is 0 (empty) return false. ELSE return true

RETURNS

	False if there are NO errors. ELSE return true
*/
/**/

bool Errors::Is_corrupt()
{
	if (m_ErrorMsgs.empty() == true)
	{
		return false;
	}
	return true;
}

/**/
/*
Errors::Visual_error_effects()
NAME

	Errors::Visual_error_effects - Adds some cool visual effects when displaying ALL errors

SYNOPSIS

	void Errors::Visual_error_effects();

DESCRIPTION

	This function will add some cool visual effects when displaying ALL errors. 
	Plus it will also clear the console screen before adding the visual effects 
	just like a real Blue Screen Of Death in the Windows OS.

RETURNS

	nothing since it is a void
*/
/**/

void Errors::Visual_error_effects()
{
	// The determined font size before declaration
	int Letter_Size = 70;
	const int WHITE_BLUE = 159;
	HANDLE console_color;
	// Modify the output styling properties
	console_color = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console_color, WHITE_BLUE);
	// get rid of other text
	system("cls");
	CONSOLE_FONT_INFOEX font;
	font.cbSize = sizeof(font);
	// declare the size of the font
	font.dwFontSize.Y = Letter_Size;
	// Choose your font                  
	wcscpy(font.FaceName, L"Impact"); 
	// Now with everything we got we now set the font
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &font);
}

/**/
/*
Errors::Fail_picture(string a_name)
NAME

	Errors::Fail_picture - Displays a picture 

SYNOPSIS

	void Errors::Fail_picture(string a_name);
	a_name -> The name of the file to be opened

DESCRIPTION

	This function will display a picture of a skull and cross bones 
	made out of ASCII characters

RETURNS

	nothing since it is a void
*/
/**/

void Errors::Fail_picture(string a_name)
{
	const int MAX = 5000;
	char PicLine[MAX + 1];
	ifstream Picture;
	Picture.open(a_name);
	Picture.getline(PicLine, MAX + 1, '\n');
	while (!Picture.eof())
	{
		cout << "		  "<<PicLine << endl;
		Picture.getline(PicLine, MAX + 1, '\n');
	}
	Picture.close();
}