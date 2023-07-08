//
//		Symbol table class.
//
#pragma once



// This class is our symbol table.
class SymbolTable 
{

public:

    const int multiplyDefinedSymbol = -999;

    // Add a new symbol to the symbol table.
    void AddSymbol( const string &a_symbol, int a_loc );

    // Display the symbol table.
    void DisplaySymbolTable();

    // Lookup a symbol in the symbol table.
    bool LookupSymbol( const string &a_symbol, int &a_loc );

    // Grab the symbol location
    int Grab_symbol_location(string a_op_one, string a_op_two);

    // Grab the register number
    int Grab_reg_num(string a_instruction);
   
   

private:

    // This is the actual symbol table. The symbol is the key to the map. The value is the location.
    map<string, int> m_symbolTable;

    // random number generator
    vector<int>m_rng;

    string Line_alignment(int a_sym_num, string a_symbol, int a_loc);
	
    string Convert(int a_value);
};
