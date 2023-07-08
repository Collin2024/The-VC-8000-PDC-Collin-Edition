//
//		Implementation of the symbol table class.  This is the format I want for commenting functions.
//
#include "stdafx.h"
#include "SymTab.h"

/**/
/*
SymbolTable::AddSymbol( const string &a_symbol, int a_loc )

NAME

    SymbolTable::AddSymbol - adds a new symbol to the symbol table.

SYNOPSIS

    void SymbolTable::AddSymbol( const string &a_symbol, int a_loc );
    a_symbol	-> The name of the symbol to be added to the symbol table.
    a_loc		-> the location to be associated with the symbol.

DESCRIPTION

    This function will place the symbol "a_symbol" and its location "a_loc"
    in the symbol table.

RETURNS

       nothing since it is a void
*/
/**/

void SymbolTable::AddSymbol( const string &a_symbol, int a_loc )
{
    // If the symbol is already in the symbol table, record it as multiply defined.
    map<string, int>::iterator st = m_symbolTable.find(a_symbol);
    if (st != m_symbolTable.end()) {
        
        st->second = multiplyDefinedSymbol;
        return;
    }
    // Record a the  location in the symbol table.
    m_symbolTable[a_symbol] = a_loc;
}

/**/
/*
SymbolTable::LookupSymbol(const string& a_symbol, int& a_loc)

NAME

    SymbolTable::LookupSymbol - looks for a specified symbol in the symbol table.

SYNOPSIS

    bool SymbolTable::LookupSymbol(const string& a_symbol, int& a_loc);
    a_symbol	-> The name of the symbol to be searched for in the symbol table.
    a_loc		-> the location to be associated with the symbol.

DESCRIPTION

    Looks for the specified symbol and if it finds it (if its location is > 0)
    then it returns true since it finds it
    else then it would return false

RETURNS

       Returns false if the symboltable can't find the specified symbol. 
       If it does find it then it returns true.
*/
/**/

bool SymbolTable::LookupSymbol(const string& a_symbol, int& a_loc)
{
    map<string, int>::iterator specified_symbol;
    specified_symbol = m_symbolTable.find(a_symbol);
    if (specified_symbol == m_symbolTable.end())
    {
        return false;                                                                          
    }
    return true;                                                                              
}

/**/
/*
SymbolTable::DisplaySymbolTable()

NAME

    SymbolTable::DisplaySymbolTable - will get the number of the register

SYNOPSIS

    void SymbolTable::DisplaySymbolTable();

DESCRIPTION

    Displays the contents of the Symbol Table

RETURNS

      nothing since it is a void
*/
/**/

void SymbolTable::DisplaySymbolTable()
{ 
    cout << "Your VC8000 is starting up please wait..." << endl;
    // Play classical Windows XP startup chime
    PlaySound(TEXT("Startup.wav"), NULL, SND_SYNC);
    // Clear the message above
    system("cls");
    cout << "Symbol Table:" << endl;
    cout << endl;
    cout << "Symbol#" << setw(13) << "Symbol" << setw(15) << "Location" << endl;
    int counter = 0;                                                                            
    for (map<string, int>::iterator i = m_symbolTable.begin(); i != m_symbolTable.end(); i++)   
    {
        int x = i->second;
        //cout << setw(2) << counter << setw(16) << i->first << setw(13) << i->second << endl;  
        cout << Line_alignment(counter, i->first, i->second) << endl;
        counter++;
    }
    cout << "__________________________________________________________" << endl;
    cout << endl;
    cout << "Press Enter to continue..." << endl;
    // allows for the enter key to be pressed it ignores any input
    cin.ignore();                                                                               
}

/**/
/*
SymbolTable::Grab_symbol_location(string a_op_one, string optwo)

NAME

    SymbolTable::Grab_symbol_location - will get the number of the location of the specified symbol

SYNOPSIS

    int SymbolTable::Grab_symbol_location(string a_op_one, string a_op_two);
    a_op_one -> The first operand
    a_op_two -> The second operand

DESCRIPTION

    It takes in both operands and if operand two is NOT empty then we see which one has more letters
    and if the 2nd one has more letters (A-z) we then set operand one = operand two. ELSE if the second one 
    is empty then we take operand one and delete any extra characters (spaces, commas, and semicolons) 
    and any other characters that come after those extra characters so in the end we should be left with 
    one of the symbols. After that we look for that symbol in the symbol table and return its specified
    location. ELSE if the location is NOT found then we simply just return 0. 

RETURNS

      It returns the location of the specified symbol
*/
/**/

int SymbolTable::Grab_symbol_location(string a_op_one, string a_op_two)
{
    int oponecheck = 0;
    int optwocheck = 0;
    int final = 0;
    // If the second operand is NOT empty we determine which operand has more letters (A-z)
    if (a_op_two.empty() == false)
    {
        for (int i = 0; i < a_op_one.size(); i++)
        {
            if (a_op_one[i] >= 'A' && a_op_one[i] <= 'z')
            {
                oponecheck++;
            }
        }
        for (int i = 0; i < a_op_two.size(); i++)
        {
            if (a_op_two[i] >= 'A' && a_op_two[i] <= 'z')
            {
                optwocheck++;
            }
        }
    // If second operand has more letters then the first operand OR if the first operand is NOT found
        if ((LookupSymbol(a_op_one, oponecheck) == false) ||(optwocheck > oponecheck))
        {
            a_op_one.clear();
            a_op_one = a_op_two;
        }
    }
    else                                       
    {
    // delete any extra characters (spaces, commas, and semicolons) and any other characters that come after
        for (int i = 0; i < a_op_one.size(); i++)
        {
            while (a_op_one[i] == ' ' || a_op_one[i] == ';' || a_op_one[i] == ',')
            {
                a_op_one.pop_back();
            }
        }
    }
    // Search for the specified symbol
    for(map<string, int>::iterator i = m_symbolTable.begin(); i != m_symbolTable.end(); i++)   
    {
        if (a_op_one == i->first)
        {
            final = i->second;                                                                  
            return final;                                                                      
        }
    }
    // if the location is NOT found then...
    return 0;                                                                                   
}

/**/
/*
SymbolTable::Grab_reg_num(string a_instruction)

NAME

    SymbolTable::Grab_reg_num - will get the number of the register 

SYNOPSIS

    int SymbolTable::Grab_reg_num(string a_instruction);
    a_instruction -> a_instruction is passed in and is used in determining the register number.
    

DESCRIPTION

    This function will take instruction string and delete extra characters that are NOT numbers
    from there it will be either converted to a int if a number  character is present else it 
    will generate a random number between 0-9 and each time this instance is the case it will return the 
    same random number

RETURNS

       Returns the register number
*/
/**/

int SymbolTable::Grab_reg_num(string a_instruction)
{
    int final = 0;
    // Delete any spaces     
    a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ' '), a_instruction.end());
    //delete any extra characters (commas, and semicolons) and any other characters that come after 
    for (int i = 0; i < a_instruction.size(); i++)
    {
        if ((a_instruction[i + 1] >= '0' && a_instruction[i + 1] <= '9') && a_instruction[i] == ',')
        {
            a_instruction[i] = a_instruction[i + 1];
            a_instruction[i + 1] = ',';
            a_instruction.pop_back();

        }
        while (a_instruction[i] == ',' || a_instruction[i] == ';')
        {
            a_instruction.pop_back();
        }
    }
    reverse(a_instruction.begin(), a_instruction.end());
    // Since we are looking for the register number we will get rid of any letter (A-z) characters
    for (char i = 'A'; i <='z'; i++)
    {
        a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), i), a_instruction.end());
    }
    a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), '?'), a_instruction.end());
    // if there is no numeric characters then the instructions string will be empty 
    if (a_instruction.empty())                                                                    
    {
        m_rng.push_back(rand() % 9);                                                  
    // After the first number is inserted into the Random Number Generator we will return the same random number
        return m_rng.at(0);                                                                       
    }
   //
    // In this case the number is specified so we will convert it to an int and return the the register number (final) 
    else                                                                                        
    {
        reverse(a_instruction.begin(), a_instruction.end());
        stringstream convert;                                                                    
        convert << a_instruction;
        convert >> final;                                                                
    }
    return final;                                                                               
}

/**/
/*
SymbolTable::Line_alignment(int a_sym_num, string a_symbol, int a_loc)

NAME

    SymbolTable::Line_alignment - lines up the symbol table values

SYNOPSIS

    string SymbolTable::Line_alignment(int a_sym_num, string a_symbol, int a_loc);
    a_sym_num -> symbol number
    a_symbol  -> The symbol
    a_loc     -> The location


DESCRIPTION

    This function will take the contents of the symbol table and perfectly align everything

RETURNS

       Returns the aligned string with the approiate amount of spaces
*/
/**/

string SymbolTable::Line_alignment(int a_sym_num, string a_symbol, int a_loc)
{
    string ref_sym_line = "ymbol#       S";
    string ref_loc_line = "ymbol       L";
    string location = Convert(a_loc);
    string symbol_num = Convert(a_sym_num);
    int sym_num_size = int(symbol_num.size());
    int sym_size = int(a_symbol.size());
    // subtract size difference to determine how many spaces it needs to make it aligned 
    for (int i = 0; i < (ref_sym_line.size() - sym_num_size); i++)
    {
        symbol_num.push_back(' ');
    }
    for (int i = 0; i < (ref_loc_line.size() - sym_size); i++)
    {
        a_symbol.push_back(' ');
    }
    return (symbol_num + a_symbol + location);
}

/**/
/*
SymbolTable::Convert(int a_value)

NAME

    SymbolTable::Convert - converts the data type

SYNOPSIS

    string SymbolTable::Convert(int a_value);
    a_value -> The numerical value to convert

DESCRIPTION

    This function will convert the int to a string ultizling the string stream function

RETURNS

       Returns the converted data type
*/
/**/

string SymbolTable::Convert(int a_value)
{
    string final;
    stringstream convert;
    convert << a_value;
    convert >> final;
    return final;
}