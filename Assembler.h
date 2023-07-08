//
//		Assembler class.  This is a container for all the components
//		that make up the assembler.
//
#pragma once 

#include "SymTab.h"
#include "Instruction.h"
#include "FileAccess.h"
#include "Emulator.h"


class Assembler 
{

public:

    // Constructor
    Assembler( int argc, char *argv[] );

    // Pass I - establish the locations of the symbols
    void PassI( );

    // Pass II - generate a translation
    void PassII();

    // Display the symbols in the symbol table.
    void DisplaySymbolTable(); 
    
    // Run emulator on the translation.
    void RunProgramInEmulator();
    
    

private:

    // File Access object
    FileAccess m_facc;

    // Symbol table object	    
    SymbolTable m_symtab;

    // Instruction object   
    Instruction m_inst;

    // Emulator object	    
    emulator m_emul;

    // is it assembly instructions
    bool m_is_assembly = false;

    // error checking assistants are used in the ACTUAL error checking methodogies
    bool m_assembly_errors = false;
    bool Is_letter(string a_line);
    string Op_code_digits(string a_opc, string a_trans_word, string a_instruction, int a_loc);
    string Zero_filler(string& a_trans_opc, int a_pos);
    string Machine_zero_filler(string& a_trans_opc, int a_reg_num);
    string Line_alignment(string a_original_statement, int a_loc);
    bool Keep_random_reg(string a_instruction);

    // data converters
    long long Contents_converter(string a_contents);
    int Op_code_converter(string a_opc);
    string Translation_converter(int a_value);

    // error checking methodogies
    bool Multi_label(string a_instruction, string a_opc);
    bool Missing_label(string a_opc, string a_instruction);
    bool Is_numeric(string a_opc, string a_instruction);
    bool Is_symbolic(string a_opc, string a_instruction, string a_opone, string a_optwo, int a_loc);
    bool Reg_num_missing(string a_opc, string a_instruction);
    bool Is_multi_reg_num(string a_opc, string a_instruction);
    bool Has_comma(string a_opc, string a_instruction);
    bool Location_is_negative(int a_loc);
    bool Location_beyond_mem(int a_loc);
    bool Org_in_range(string a_opc, string a_instruction);
    bool Reg_in_range(string a_opc, string a_instruction);

    // checks for all errors using the error checking methodogies 
    string Machine_error_check(string a_opc, string a_instruction, int a_loc,string a_opone, string a_optwo, string &a_trans_opc);
    string Assembly_error_check(string a_opc, string & a_trans_opc, string &a_operand, int a_loc);
    
    // display errors
    void Display_machine_errors(string a_opc, string a_instruction, int a_loc, string a_opone, string a_optwo, string a_trans_opc);
    void Display_assembly_errors(string a_opc, string a_instruction, int a_loc, string a_optwo, string a_trans_opc);
    void Display_location_errors(int a_loc);

    // error checker and recorder
    void Is_error_check(string a_trans_word, string a_error);

    // reset error effects
    void Font_reset();
};