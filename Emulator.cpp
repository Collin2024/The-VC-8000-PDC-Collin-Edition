#include "stdafx.h"
#include "Emulator.h"
#include "Errors.h"

/**/
/*
 emulator::emulator()
NAME

	emulator::emulator - Emulator constructor

SYNOPSIS

	emulator::emulator();

DESCRIPTION

	Emulator constructor is used to resize the m_memory to 0

RETURNS

	nothing since it is a constructor
*/
/**/

emulator::emulator()
{
	m_memory.resize(MEMSZ, 0);
}
/**/
/*
emulator::insertMemory(int a_location, long long a_contents)
NAME

	emulator::insertMemory - Inserts the location and the contents in the memory

SYNOPSIS

	bool emulator::insertMemory(int a_location, long long a_contents);

DESCRIPTION

	This function will insert the location and the contents in the approiate location.
	If the location is greater than MEMSZ (1,000,000) then record the error and return false.
	ELSE if the insert the contents at the specified location and return true.
	a_location -> The location in memory 
	a_contents -> The stuff to be inserted at the specified memory location


RETURNS

	Return false if the location is greater than MEMSZ (1,000,000).
	ELSE return true.
*/
/**/

bool emulator::insertMemory(int a_location, long long a_contents)
{
	
	// if the location specified is more than 1,000,000
	if (a_location > MEMSZ)
	{
		Errors::RecordError("ERROR:CONSTANT_TOO_LARGE_FOR_VC8000_MEMORY");
		return false;
	}
	else if (a_location > 0 && a_contents > 0)
	{
		m_memory.at(a_location) = a_contents;
	}
	return true;
}

/**/
/*
emulator::runProgram()
NAME

	emulator::runProgram - Executes the emulation

SYNOPSIS

	bool emulator::runProgram();

DESCRIPTION

	This function will execute the emulator AKA emulating the translation
	by ultizling the opcode and address information 


RETURNS

	Return true if it runs successfully and there are NO errors.
	ELSE return false if opc < 1 OR opc > 17.
*/
/**/

bool emulator::runProgram()
{
	// i acts as our counter
	long long i = 100;
	// terminate_emulation is used to determine if opc is 17 (HALT which is our stop) condition
	bool terminate_emulation = false;
	
	m_register.assign(9, 0);
	// run the emulator until terminate_emulation is true AKA opc = 17
	while (terminate_emulation == false)
	{	
	// opc_digit_cutter is used to get the FIRST 2 digits these are used for the opcode 
		long long opc_digit_cutter = long long(pow(10, 7));
	// addr_digit_cutter is used to get the LAST 3 digits these are used for the address 
		long long addr_digit_cutter = long long(pow(10, 6));
		long long addr = m_memory.at(i) % addr_digit_cutter;
		long long opc = m_memory.at(i) / opc_digit_cutter;
	// emulate the instructions
		if (opc >= 1 && opc <= 6)
		{
			Single_reg_instruction(opc, addr, i);
		}
		else if (opc >= 7 && opc <= 10)
		{
			Multi_reg_instruction(opc, i);
		}
		else if (opc >= 11 && opc <= 12)
		{
			Read_write_instruction(opc, addr);
		}
		else if (opc >= 13 && opc <= 16)
		{
			Branch_instruction(opc, addr, i);
		}
		else if (opc == 17)
		{
	// Stop condition (Emulation)
			terminate_emulation = true;
		}
		i++;
	}

	return true;
}

/**/
/*
emulator::Set_main_reg(long long& a_index)
NAME

	emulator::Set_main_reg - Finds the specified register number

SYNOPSIS

	long long emulator::Find_reg_num(long long a_contents, long long a_opc);
	a_index -> The specified position 

DESCRIPTION

	This function will set the main register variable
	equal to the value of the register container number
	at the specified index


RETURNS

	Nothing since it is a void (SETTER).
*/
/**/

void emulator::Set_main_reg(long long& a_index)
{
	m_main_reg = m_register.at(a_index);
}

/**/
/*
emulator::Find_reg_num(long long a_contents, long long a_opc)
NAME

	emulator::Find_reg_num - Finds the specified register number

SYNOPSIS

	long long emulator::Find_reg_num(long long a_contents, long long a_opc);
	a_contents -> The contents is the 9 digit number (translation) used in determining the register number
	a_opc	   -> The op code

DESCRIPTION

	This function will find the register number (the second set of 2 digits) 
	in the contents and if its opcode is between 1 and 6 make it only 1 digit
	since 1-6 is single register instructions


RETURNS

	Return the register number.
*/
/**/

long long emulator::Find_reg_num(long long a_contents, long long a_opc)
{
	// lets convert it to a string so it is easier to work with
	stringstream convert;
	string contents;
	convert << a_contents;
	convert >> contents;
	// eliminate the opcode (first 2 digits set) since we are passing it in seperately
	if (contents.size() == 8)
	{
		contents.erase(0, 1);
	}
	else
	{
		contents.erase(0, 2);
	}
	// eliminate the address (last 6 digits) so we have 2 digits left 
	contents.erase(2, 8);
	// If the opcode results in emulating a single register instruction (1-6) get rid of the last digit
	if ((contents.size() == 2 && contents[1] == '0') && (a_opc >= 1 && a_opc < 7) )
	{
		contents.pop_back();
	}
	// convert it back to a long long
	long long final;
	stringstream reconvert;
	reconvert << contents;
	reconvert >> final;
	return final;
}

/**/
/*
emulator::Single_reg_instruction(long long a_opc, long long a_addr, long long& a_index)
NAME

	emulator::Single_reg_instruction - Emulates single register instructions

SYNOPSIS

	long long emulator::Single_reg_instruction(long long a_opc, long long a_addr, long long& a_index);
	
	a_opc	   -> The op code
	a_addr	   -> The address
	a_index	   -> The specified position 

DESCRIPTION

	This function will Emulate instructions that only use a singular register
	(opc >= 1) and (opc <= 6)


RETURNS

	Return the MAIN register.
*/
/**/

long long emulator::Single_reg_instruction(long long a_opc, long long a_addr, long long& a_index)
{
	// Determine the register number 
	long long register_number = Find_reg_num(m_memory.at(a_index), a_opc);
	// Emulate the instructions based on the opcode
	if (a_opc == 1)
	{
		m_register.at(register_number) = m_register.at(register_number) + m_memory.at(a_addr);
	}
	else if (a_opc == 2)
	{
		m_register.at(register_number) = m_register.at(register_number) - m_memory.at(a_addr);
	}
	else if (a_opc == 3)
	{
		m_register.at(register_number) = m_register.at(register_number) * m_memory.at(a_addr);
	}
	else if (a_opc == 4)
	{
		Divide_by_zero(a_addr);
		m_register.at(register_number) = m_register.at(register_number) / m_memory.at(a_addr);
	}
	else if (a_opc == 5)
	{
		m_register.at(register_number) = m_memory.at(a_addr);	
	}
	else
	{
		m_memory.at(a_addr) = m_register.at(register_number);
		return m_memory.at(a_addr);
	}
	// we will set the main register equal to the vector register
	Set_main_reg(register_number);
	return m_main_reg;
}

/**/
/*
emulator::Multi_reg_instruction(long long a_opc, long long a_addr, long long& a_index)
NAME

	emulator::Multi_reg_instruction - Emulates the instructions that use 2 (hence the "Multi" in the name) 
	registers

SYNOPSIS

	long long emulator::Multi_reg_instruction(long long a_opc, long long a_addr, long long& a_index);
	a_opc	   -> The op code
	a_index	   -> The specified position

DESCRIPTION

	This function will Emulate instructions that use multiple registers
	(opc >= 7) and (opc <= 10)


RETURNS

	Return the MAIN register.
*/
/**/

long long emulator::Multi_reg_instruction(long long a_opc, long long& a_index)
{
	// Determine the register number
	long long merged_reg_num = Find_reg_num(m_memory.at(a_index), a_opc);
	// Ok so here we are doing something a bit different since we are working with 2 registers
	// there are 2 register numbers combined into one gigantic register so we take this
	// number and break it up into 2 numbers
	// first register number
	long long first = merged_reg_num / 10;
	// second register number
	long long second = merged_reg_num % 10;
	// emulate the instructions
	if (a_opc == 7)
	{
		m_register.at(first) = m_register.at(first) + m_register.at(second);
	}
	else if (a_opc == 8)
	{
		m_register.at(first) = m_register.at(first) - m_register.at(second);
	}
	else if (a_opc == 9)
	{
		m_register.at(first) = m_register.at(first) * m_register.at(second);
	}
	else
	{
		Divide_by_zero(second);
		m_register.at(first) = m_register.at(first) / m_register.at(second);
	}	
	// we will set the main register equal to the vector register
	Set_main_reg(first);
	return m_main_reg;
	
}

/**/
/*
emulator::Read_write_instruction(long long a_opc, long long a_addr)
NAME

	emulator::Read_write_instruction - Emulates the user inpuut and output
	

SYNOPSIS

	void emulator::Read_write_instruction(long long a_opc, long long a_addr);
	a_opc	   -> The op code
	a_addr	   -> The specified address

DESCRIPTION

	This function will Emulate the user input and output instructions
	(opc >= 11) and (opc <= 12)


RETURNS

	Nothing since it is a void
*/
/**/

void emulator::Read_write_instruction(long long a_opc, long long a_addr)
{
	if (a_opc == 11)
	{
		long long vcinput = 0;
		cout << "? ";
		cin >> vcinput;
		m_memory.at(a_addr) = vcinput;
	}
	else
	{
		cout << m_memory.at(a_addr) << endl;
	}
}

/**/
/*
emulator::Branch_instruction(long long a_opc, long long a_addr, long long& a_index)
NAME

	emulator::Branch_instruction - Emulates branching instructions

SYNOPSIS

	long long emulator::Single_reg_instruction(long long a_opc, long long a_addr, long long& a_index);

	a_opc	   -> The op code
	a_addr	   -> The address
	a_index	   -> The specified position

DESCRIPTION

	This function will Emulate branching instructions 
	(opc >= 13) and (opc <= 16)


RETURNS

	Return the index.
*/
/**/

long long emulator::Branch_instruction(long long a_opc, long long a_addr, long long &a_index)
{
		if (a_opc == 13)
		{
			a_index = a_addr;
		}
		else if (a_opc == 14)
		{
			if (m_main_reg < 0)
			{
				a_index = --a_addr;
			}
		}
		else if (a_opc == 15)
		{
			if (m_main_reg == 0)
			{
				a_index = --a_addr;
			}
		}
		else
		{
			if (m_main_reg > 0)
			{
				a_index = --a_addr;
			}
		}
		return a_index;
}
/**/
/*
emulator::Success_Picture(string a_name)
NAME

	emulator::Success_Picture - Draws a duck

SYNOPSIS

	void emulator::Success_Picture(string name);
	a_name -> The name of the file to be opened

DESCRIPTION

	Draws a duck made out of ASCII characters (my instructors favorite animal)
	
RETURNS

	Nothing since it is a void.
*/
/**/

void emulator::Success_Picture(string a_name)
{
	// I declared these color based symbolic constants for ease of debugging
	const int RED = 12;
	const int LIGHT_GREEN = 10;
	const int YELLOW = 14;
	const int WHITE = 15;
	const int LIGHT_BLUE = 11;
	// counter is used to determine what line we are on
	int counter = 0;
	const int MAX = 6000;
	// I declared a cstring since they allow for spaces to be read in i did +1 for room for a delimiter
	char PicLine[MAX + 1];
	ifstream Picture;
	Picture.open(a_name);
	Picture.getline(PicLine, MAX + 1, '\n');
	while (counter != 21)
	{
		counter++;
	// This for loop we used to read each character of the cstring and assign it the approiate color
		for (int i = 0; i < strlen(PicLine); i++)
		{
	// x is used to draw the beak only and after line 15 we have the legs and feet
			if (PicLine[i] == 'x' || counter > 15)
			{
				HANDLE console_color;
				console_color = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(console_color, RED);

			}
	// CAPITAL letters are used to emphize the end of emulation text shown in the sample output
			else if (PicLine[i] >= 'A' && PicLine[i] <= 'Z')
			{
				HANDLE console_color;
				console_color = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(console_color, LIGHT_GREEN);
			}
	// o is used for the duck's eye
			else if (PicLine[i] != 'o')
			{
				HANDLE console_color;
				console_color = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(console_color, YELLOW);
			}
	// ALL other characters not specified are yellow
			else
			{
				HANDLE console_color;
				console_color = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(console_color, LIGHT_BLUE);
			}
			cout << PicLine[i];
		}
		Picture.getline(PicLine, MAX + 1, '\n');
		cout << endl;
	// Set the color back to the Visual Studio DEFAULT
		HANDLE console_color;
		console_color = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console_color, WHITE);
	}
	Picture.close();
}

/**/
/*
emulator::Divide_by_zero(long long a_pos)
NAME

	emulator::Divide_by_zero - oh no don't get an exception thrown

SYNOPSIS

	void emulator::Divide_by_zero(long long a_pos);
	a_pos -> The position in the vector
DESCRIPTION

	Prevents exception thrown error when the user tries to
	divide by 0


RETURNS

	Nothing since it is a void.
*/
/**/

void emulator::Divide_by_zero(long long a_pos)
{
	if (m_register.at(a_pos) == 0)
	{
		PlaySound(TEXT("Error.wav"), NULL, SND_SYNC);
		cout << endl;
		cerr << "ERROR: CAN'T_DIVIDE_BY_ZERO_SYSTEM_ABORT";
		cout << endl;
		exit(1);
	}
}