//
//      Implementation of the Assembler class.
//
#include "stdafx.h"
#include "Assembler.h"
#include "Errors.h"


/**/
/*
 Assembler::Assembler( int argc, char *argv[] ): m_facc( argc, argv )
NAME

    Assembler::Assembler - Assembler constructor

SYNOPSIS

    Assembler::Assembler( int argc, char *argv[] ): m_facc( argc, argv );
    argc -> Determines the amount of arguements that were read in from the CMD
    argv -> Filename

DESCRIPTION

    Assembler constructor is used to pass argc and argv to the file access constructor (see main for more info).
    It also checks the file contents to make sure that there are no numbers more then 9 digits long. Finally it resets 
    the font to the clasic visual studio default if there were previously errors

RETURNS

    nothing since it is a constructor
*/
/**/

Assembler::Assembler( int argc, char *argv[] ): m_facc( argc, argv )
{
    Font_reset();
    m_facc.File_num_check();
}  

/**/
/*
 Assembler::PassI()
NAME

    Assembler::PassI - establish the locations of the symbols

SYNOPSIS

    void Assembler::PassI();

DESCRIPTION

    This function will get the locations of the symbols and add them to the symbol table 

RETURNS

    nothing since it is a void
*/
/**/

void Assembler::PassI( )                                                               
{
    // Tracks the location of the instructions to be generated.
    int loc = 0;        
    // Successively process each line of source code.
    for (; ; ) {

    // Read the next line from the source file.
        string line;
        if (!m_facc.GetNextLine(line)) {

    // If there are no more lines, we are missing an end statement.
    // We will let this error be reported by Pass II.
            return;
        }
    // Parse the line and get the instruction type.
        Instruction::InstructionType st = m_inst.ParseInstruction(line);

    // If this is an end statement, there is nothing left to do in pass I.
    // Pass II will determine if the end is the last statement and report an error if it isn't.
        if (st == Instruction::InstructionType::ST_End) return;

    // Labels can only be on machine language and assembler language
    // instructions.  So, skip comments.
        if (st == Instruction::InstructionType::ST_Comment)
        {
            continue;
        }
    // Handle the case where there is an error.

    // If the instruction has a label, record it and its location in the
    // symbol table.
        if (m_inst.isLabel()) {

            m_symtab.AddSymbol(m_inst.GetLabel(), loc);
        }
    // Compute the location of the next instruction.
        loc = m_inst.LocationNextInstruction(loc);
    }
}

/**/
/*
 Assembler::PassII()
NAME

    Assembler::PassII - generate a translation

SYNOPSIS

    void Assembler::PassII();

DESCRIPTION

    This function will generate a translation of the symbols in addition, errors will be recorded

RETURNS

    nothing since it is a void
*/
/**/

void Assembler::PassII()
{ 
    int loc = 0;                                                                      
    // start at the begining of the file
    m_facc.rewind();                                                                   
    Errors::InitErrorReporting();                                                      
    string trans_word;                                                                  
    cout << "Translation of Program:" << endl;
    cout << endl;
    cout << "Location" << setw(15) << "Contents" << setw(25) << "Original Statement" << endl;
    // fun fact same thing as while true
    for (; ; )                                                                         
    {
        string line;
    // Read the next line from the source file.
        if (m_facc.GetNextLine(line) == false)                                          
        {
    // if there is no end statement record error and stop loop
            Errors::RecordError("ERROR:MISSING_END_STATEMENT_OR_THE_END_STATEMENT_IS_NOT_THE_LAST_ONE_IN_THE_PROGRAM");
            return;                                                                     
        }
    // st is used to get the instruction type
        Instruction::InstructionType st = m_inst.ParseInstruction(line);                                                                        
        string opc = m_inst.Grab_OpCode();                                          
        string cpu = m_inst.Grab_Instructions();                                    
        string first_operand = m_inst.Grab_oper1();                                
        string second_operand = m_inst.Grab_oper2();                    
        if (st == Instruction::InstructionType::ST_MachineLanguage)      
        {
            m_is_assembly = false;
    // based on the op_code we add the first 2 digits to trans_opc string (contents) (don't mind the style change did this to meet the 100 line thresh hold)                                                                      
            trans_word += Op_code_digits(opc, trans_word,cpu, loc);
            int symloc = m_symtab.Grab_symbol_location(first_operand, second_operand);          
            int register_number = m_symtab.Grab_reg_num(cpu);
            trans_word += Translation_converter(register_number);
            Machine_error_check(Op_code_digits(opc, trans_word, first_operand, loc), cpu, symloc, first_operand, second_operand, trans_word);
    // add in the extra zeros so that the size is 9 becuase opcode has 9 digits in machine (don't mind the style change did this to meet the 100 line thresh hold)
            Machine_zero_filler(trans_word, register_number);
            cout << Translation_converter(loc) << Line_alignment(trans_word, loc) << Line_alignment(cpu, loc) << endl;
            Display_location_errors(loc);
            Display_machine_errors(Op_code_digits(opc, trans_word, first_operand, loc), cpu, symloc, first_operand, second_operand, trans_word);
    // Convert the string into a long long
            long long contents = Contents_converter(trans_word);
    // Now that we are done with converting, load this info to the emulator
            m_emul.insertMemory(loc, contents); 
    // Since this is technically a "while true" loop empty the strings and start fresh again
            trans_word.clear();                                                         
        }
        else if (st == Instruction::InstructionType::ST_AssemblerInstr)       
        {
    // Since we are now working with Assembly instructions set is assembly to true
            m_is_assembly = true;
    // Assembly instructions IGNORE second operand (only the first one is used) so we clear it
            second_operand.clear();                                                     
            int symloc = m_symtab.Grab_symbol_location(cpu, second_operand);            
            trans_word += Op_code_digits(opc, trans_word, first_operand, symloc);
    // Insert extra zeros if the string size is NOT 9 (only insert if necessary)
            Zero_filler(trans_word, 0);
            cout << Translation_converter(loc) << Line_alignment(trans_word, symloc) << Line_alignment(cpu, symloc) << endl;
            Display_location_errors(loc);
            Display_assembly_errors(Op_code_digits(opc, trans_word, first_operand, symloc), cpu, symloc, first_operand, trans_word);
            if (opc != "DS")
            {
                long long contents = Contents_converter(trans_word);
                m_emul.insertMemory(symloc, contents);
            }
            trans_word.clear();                                                          
        }
        else if (st == Instruction::InstructionType::ST_End)
        {   
            cout << Line_alignment(cpu, loc) << endl;
            m_facc.File_error_reset();
            return;                                                                     
        }
        else if(st == Instruction::InstructionType::ST_Comment)
        {            
            cout << Line_alignment(cpu, loc) << endl;
            loc--;                                                                      
        }
        else                                                                           
        {
    // if(st == Instruction::InstructionType::ST_Error)
            cout << loc << Line_alignment("?????????", loc) << Line_alignment(cpu, loc) << endl;
            Errors::RecordError("ERROR: ILLEGAL_OPERATION_CODE");
        }
    // Get the next location
        loc = m_inst.LocationNextInstruction(loc);                       
    }
}

/**/
/*
 Assembler::Is_letter(string a_operand)
NAME

    Assembler::Is_letter - does the operand contain letters

SYNOPSIS

    bool Assembler::Is_letter(string a_operand);
    a_operand -> the operand

DESCRIPTION

    This function will determine if the specified operand is a letter
    if so return true. ELSE return false (USED FOR ASSEMBLY INSTRUCTIONS)

RETURNS

    Returns true if the specified operand is a letter
    if so return true. ELSE return false
*/
/**/

bool Assembler::Is_letter(string a_operand)
{
    for (int i = 0; i < a_operand.size(); i++)
    {
        if (a_operand[i] >= 'A' && a_operand[i] <= 'z')
        {
            return true;
        }
    }
    return false;
}

/**/
/*
 Assembler::Is_error_check(string a_trans_word, string a_error)
NAME

    Assembler::Is_error_check - does the translated word contain question marks

SYNOPSIS

    void Assembler::Is_error_check(string a_trans_word, string a_error);
    a_trans_word -> The translated word
    a_error -> The spiecified error

DESCRIPTION

    This function will determine if the translated word contains 
    question marks if so record the specified error

RETURNS

    Nothing since it is a void
*/
/**/

void Assembler::Is_error_check(string a_trans_word, string a_error)
{
    bool check = false;
    for (int i = 0; i < a_trans_word.size(); i++)
    {
        if (a_trans_word[i] == '?')
        {
            check = true;
            break;
        }
    }
    // if there are question marks in the string OR if there are assembly errors 
    if (m_assembly_errors == true || check == true)
    {
    // record and display the error
        Errors::RecordError(a_error);
    }
}

/**/
/*
 Assembler::Is_numeric(string a_instruction, string a_opc)
NAME

    Assembler::Is_numeric - does the code contain numbers (where applicable)

SYNOPSIS

    bool Assembler::Is_numeric(string a_instruction, string a_opc);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the instructions contain numerical values
    if so (or where NOT applicable based on opcode) return true. ELSE return false

RETURNS

    Returns true if the instructions contain numerical values
    if so (or where NOT applicable based on opcode)
    ELSE return false
*/
/**/

bool Assembler::Is_numeric(string a_opc, string a_instruction)
{
    int opc = Op_code_converter(a_opc);
    if ((opc >= 1 && opc <= 6) || (opc >= 11 && opc <= 16))
    {
        return true;
    }
    else if (opc == 17 || (opc >= 7 && opc <= 10))
    {
        if (opc == 17)
        {
            return true;
        }
        else if (opc >= 7 && opc <= 10)
        {
            a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ' '), a_instruction.end());
            a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ','), a_instruction.end());
            for (char i = 'A'; i <= 'z'; i++)
            {
                a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), i), a_instruction.end());
            }
            int number = Op_code_converter(a_instruction);
            return true;
        }
    }
    return false;
}

/**/
/*
 Assembler::Multi_label(string a_instruction, string a_opc)
NAME

    Assembler::Multi_label - does the code contain multiple labels

SYNOPSIS

    bool Assembler::Multi_label(string a_instruction, string a_opc);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the instructions contain multiple labels
    if so return true. ELSE return false

RETURNS

    Returns true if the instructions contain multiple labels
    ELSE return false
*/
/**/

bool Assembler::Multi_label(string a_instruction, string a_opc)
{
    int counter = 0;

    for (int i = 0; i < a_instruction.size(); i++)
    {
        if (a_instruction[i] == ',')
        {
            counter++;
        }
    }
    if (counter > 1)
    {
        return true;
    }    
    return false;
}

/**/
/*
 Assembler::Is_symbolic(string a_opc, string a_instruction, string a_opone, string a_optwo, int a_loc)
NAME

    Assembler::Is_symbolic - does the code contain a symbol when approiate

SYNOPSIS

    bool Assembler::Is_symbolic(string a_opc, string a_instruction, string a_opone, string a_optwo, int a_loc);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions
    a_opone       -> first operand
    a_optwo       -> second operand
    a_loc         -> the location

DESCRIPTION

    This function will determine if the instructions contain a symbol (where applicable)
    if so (or if a symbol is NOT required) return true. ELSE return false

RETURNS

    Returns true if the instructions contain a symbol (where applicable or if a symbol is NOT required)
    ELSE return false
*/
/**/

bool Assembler::Is_symbolic(string a_opc, string a_instruction, string a_opone, string a_optwo, int a_loc)
{
    int opc = Op_code_converter(a_opc);
    // Delete any spaces     
    a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ' '), a_instruction.end());
    if ((opc == 17) || (opc >= 7 && opc <= 13))
    {
        if (opc >= 11 && opc <= 13 && (m_symtab.LookupSymbol(a_opone, a_loc) == false && m_symtab.LookupSymbol(a_optwo, a_loc) == false) && a_instruction.back() != ' ')
        {
            return false;
        }
        return true;
    }
    else if (((opc >= 1 && opc <= 6) || (opc >= 14 && opc <= 16)) && (m_symtab.LookupSymbol(a_opone, a_loc) == true || m_symtab.LookupSymbol(a_optwo, a_loc) == true))
    {
        for (int i = 0; i < a_instruction.size(); i++)
        {
            if (a_instruction[i] == ',' && (a_instruction[i - 1] >= '0' && a_instruction[i - 1] <= '9'))
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

/**/
/*
 Assembler::Is_multi_reg_num(string a_opc, string a_instruction)
NAME

    Assembler::Is_symbolic - does the code contain a symbol when approiate

SYNOPSIS

    bool Assembler::Is_multi_reg_num(string a_opc, string a_instruction);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the instructions contain multiple register numbers (where applicable)
    if so (or if a symbol is NOT required) return true. ELSE return false

RETURNS

    Returns true if the instructions contain a symbol (where applicable or if a symbol is NOT required)
    ELSE return false
*/
/**/

bool Assembler::Is_multi_reg_num(string a_opc, string a_instruction)
{
    int opc = Op_code_converter(a_opc);
    a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ' '), a_instruction.end());
    a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ','), a_instruction.end());
    if (opc >= 7 && opc <= 10)
    {
        for (char i = 'A'; i <= 'z'; i++)
        {
            a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), i), a_instruction.end());
        }
    // Being lazy here and reusing a function I already wrote 
        if (Has_comma(a_opc, a_instruction) == false)
        {
            a_instruction.pop_back();
        }
        int reg_num = Op_code_converter(a_instruction);
        if (a_instruction.empty() == false && (reg_num < 10 || reg_num > 100))
        {
            return true;
        }
    }
    return false;
}

/**/
/*
 Assembler::Machine_error_check(string a_opc, string a_instruction, int a_loc, string a_opone, string a_optwo, string &a_trans_word)
NAME

    Assembler::Machine_error_check - checks for machine language error

SYNOPSIS

    string Assembler::Machine_error_check(string a_opc, string a_instruction, int a_loc, string a_opone, string a_optwo, string &a_trans_word);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions
    a_loc         -> the location
    a_opone       -> first operand
    a_optwo       -> second operand
    a_trans_word  -> the translated word (contents)

DESCRIPTION

    This function will determine if there are any errors based on the returning value of
    the error checking metholodigies and if there are errors fill in the translated word string
    with question marks (where applicable based on the error) ELSE fill in the address portion
    as normal.

RETURNS

    Returns the translated word
*/
/**/

string Assembler::Machine_error_check(string a_opc, string a_instruction, int a_loc, string a_opone, string a_optwo, string& a_trans_word)
{
    m_is_assembly = false;
    const int MAX = 9;
    string error;
    string temp_con = a_trans_word;
    int opc = Op_code_converter(a_opc);
    // if there are no errors up to this point return the address ELSE add in the ?'s where applicable
    if ((m_facc.Is_to_big(a_instruction) == false && Reg_in_range(a_opc, a_instruction) == true && Missing_label(a_opc, a_instruction) == false && Has_comma(a_opc, a_instruction) == true && Multi_label(a_instruction, a_opc) == false) && (Is_numeric(a_opc, a_instruction) == true || (m_symtab.LookupSymbol(a_opone, a_loc) == true || m_symtab.LookupSymbol(a_optwo, a_loc) == true)) && (a_loc >= 0 && a_loc < 1'000'000) && Is_symbolic(a_opc, a_instruction, a_opone, a_optwo, a_loc) == true && Reg_num_missing(a_opc, a_instruction) == false && Has_comma(a_opc, a_instruction) == true)
    {
        a_trans_word += Translation_converter(a_loc);
        return a_trans_word;
    }
    else if (m_facc.Is_to_big(a_instruction) == true && (((opc >= 1 && opc <= 6) || (opc >= 14 && opc <= 16))))
    {
        a_trans_word.pop_back();
        a_trans_word.push_back('?');
    }
    else if (Reg_in_range(a_opc, a_instruction) == false)
    {
        temp_con.erase(0, 2);
        for (int i = 0; i < temp_con.size(); i++)
        {
            a_trans_word.erase(2,i);
        }
        error.assign((9 - a_trans_word.size()), '?');
        a_trans_word += error;
        return a_trans_word;
    }
    else
    {
        char reg_val = a_trans_word.back(); 
        if (((opc >= 7 && opc <= 10) && (Is_multi_reg_num(a_opc, a_instruction) == false)) || (((opc >= 1 && opc <= 6) || (opc >= 14 && opc <= 16)) && ((Reg_num_missing(a_opc, a_instruction) == true)||(Is_numeric(a_opc, a_instruction) == false) ||((Is_symbolic(a_opc, a_instruction, a_opone, a_optwo, a_loc) == false) && (Missing_label(a_opc, a_instruction) == true)))))
        {
            if (((opc >= 1 && opc <= 6) || (opc >= 14 && opc <= 16)) && (m_symtab.LookupSymbol(a_opone, a_loc) == true || m_symtab.LookupSymbol(a_optwo, a_loc) == true) && (Reg_num_missing(a_opc, a_instruction) == true))
            {
                a_trans_word.pop_back();
                a_trans_word.push_back('?');
                a_trans_word += Translation_converter(a_loc);
                return a_trans_word;
            }
            else if (((opc >= 7 && opc <= 10) && (Is_multi_reg_num(a_opc, a_instruction) == false)) || Keep_random_reg(a_instruction) == false)
            {
                a_trans_word.pop_back(); 
                a_trans_word.push_back('?');
                a_trans_word.push_back('?');
            }
        }
        else if ((opc >= 7 && opc <= 10) && Is_numeric(a_opc, a_instruction) == false)
        {
            for (int i = 0; i < a_instruction.size(); i++)
            {
                if (a_instruction[i] == ',' && ((a_instruction[i + 1] >= 'A' && a_instruction[i + 1] <= 'z') && (a_instruction[i - 1] >= 'A' && a_instruction[i - 1] <= 'z')))
                {
                    a_trans_word.pop_back();
                    a_trans_word.push_back('?');
                    a_trans_word.push_back('?');
                    return a_trans_word;
                }
                else if (a_instruction[i] == ',' && (a_instruction[i + 1] >= 'A' && a_instruction[i + 1] <= 'z'))
                {
                    a_trans_word.push_back('?');
                    return a_trans_word;
                }
                else if (a_instruction[i] == ',' && (a_instruction[i - 1] >= 'A' && a_instruction[i - 1] <= 'z'))
                {
                    a_trans_word.pop_back();
                    a_trans_word.push_back('?');
                    a_trans_word.push_back(reg_val);
                    return a_trans_word;
                }
            }
        }
        error.assign((9 - a_trans_word.size()), '?');
        a_trans_word += error;
    }
    return a_trans_word;
}

/**/
/*
 Assembler::Reg_num_missing(string a_opc, string a_instruction)
NAME

    Assembler::Reg_num_missing - are the register number present?

SYNOPSIS

    bool Assembler::Reg_num_missing(string a_opc, string a_instruction);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the instructions are missing the register numbers (where applicable)
    if so return true. ELSE return false

RETURNS

    Returns true if the instructions are missing the register
    numbers (where applicable). ELSE return false
*/
/**/

bool Assembler::Reg_num_missing(string a_opc, string a_instruction)
{
    int opc = Op_code_converter(a_opc);
    if ((opc >= 1 && opc <= 6) || (opc >= 14 && opc <= 16))
    {
        for (int i = 0; i < a_instruction.size(); i++)
        {
            if (a_instruction[i] >= '0' && a_instruction[i] <= '9')
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

/**/
/*
 Assembler::Missing_label(string a_instruction, string a_opc)
NAME

    Assembler::Missing_label - does the code not have labels

SYNOPSIS

    bool Assembler::Multi_label(string a_instruction, string a_opc);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the instructions are missing labels
    if so return true. ELSE return false

RETURNS

    Returns true if the instructions are missing labels
    ELSE return false
*/
/**/

bool Assembler::Missing_label(string a_opc, string a_instruction)
{
    int opc = Op_code_converter(a_opc);
    if (opc >= 7 && opc <= 10)
    {
        return false;
    }
    else if (((opc >= 1 && opc <=6) || (opc >= 14 && opc <= 16)) && (a_instruction.find(',') == string::npos) || ((opc >= 1 && opc <= 16) && (a_instruction.back() == ',' || a_instruction.back() == ' ')))
    {
        return true;
    }
    return false;
}

/**/
/*
 Assembler::Display_machine_errors(string a_opc, string a_instruction, int a_loc, string a_opone, string a_optwo, string a_trans_word)
NAME

    Assembler::Display_machine_errors - Records and displays the machine (not assembly) errors 

SYNOPSIS

    void Assembler::Display_machine_errors(string a_opc, string a_instruction, int a_loc, string a_opone, string a_optwo, string a_trans_word);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions
    a_loc         -> the location 
    a_opone       -> first operand
    a_optwo       -> second operand
    a_trans_word  -> the translated word (contents)

DESCRIPTION

    This function will determine if there are any Machine errors based on the returning value of
    the error checking metholodigies and if there are errors record and display them

RETURNS

    Nothing since it is a void
*/
/**/

void Assembler::Display_machine_errors(string a_opc, string a_instruction, int a_loc, string a_opone, string a_optwo, string a_trans_word)
{
    if (Missing_label(a_opc, a_instruction) == true && Reg_num_missing(a_opc, a_instruction) == true)
    {
        Is_error_check(a_trans_word, "ERROR: MISSING_REGISTER_NUMBER_AND_LABEL");
    }
    else if (Missing_label(a_opc, a_instruction) == true)
    { 
        Is_error_check(a_trans_word, "ERROR: MISSING_LABEL");
    }
    else if (Multi_label(a_instruction, a_opc) == true)
    {
        Is_error_check(a_trans_word, "ERROR: MULTIPLE_LABELS");
    }
    else if (Is_numeric(a_opc, a_instruction) == false)
    {
        Is_error_check(a_trans_word, "SYNTAX_ERROR: INSTRUCTION_MUST_BE_NUMERIC"); 
    }
    else if (Reg_num_missing(a_opc, a_instruction) == true)
    {
        Is_error_check(a_trans_word, "ERROR: MISSING_REGISTER_NUMBER");
    }
    else if (Is_symbolic(a_opc, a_instruction, a_opone, a_optwo, a_loc) == false)
    {
        Is_error_check(a_trans_word, "SYNTAX_ERROR: INSTRUCTION_MUST_BE_SYMBOLIC");
    }
    else if (a_loc < 0)
    {
        Is_error_check(a_trans_word, "ERROR: MULTIPLY_DEFINED_SYMBOLS");
    }
    else if (a_loc >= 1'000'000)
    {
        Is_error_check(a_trans_word, "ERROR: INSUFFICIENT_MEMORY_FOR_THE_TRANSLATION");
    }
    else if (Is_multi_reg_num(a_opc, a_instruction) == true)
    {
        Is_error_check(a_trans_word, "ERROR: MUST_HAVE_TWO_REGISTER_NUMBERS");
    }
    else if (Reg_in_range(a_opc, a_instruction) == false)
    {
        Is_error_check(a_trans_word, "ERROR: REGISTER_NUMBERS_MUST_BE_BETWEEN_0_AND_9");
    }
    if (Has_comma(a_opc, a_instruction) == false)
    {
        Is_error_check(a_trans_word, "SYNTAX_ERROR: MISSING_COMMA");
    }
    else if (m_facc.Is_to_big(a_instruction) == true)
    {
        Is_error_check(a_instruction, "ERROR: NUMBER_CAN'T_BE_MORE_THEN_NINE_DIGITS");
    }
}

/**/
/*
 Assembler::Display_assembly_errors(string a_opc, string a_instruction, int a_loc, string a_operand, string a_trans_word)

    Assembler::Display_assembly_errors - Records and displays the assembly (not machine) errors

SYNOPSIS

    void Assembler::Display_assembly_errors(string a_opc, string a_instruction, int a_loc, string a_opone, string a_optwo, string a_trans_word);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions
    a_loc         -> the location
    a_operand     -> first operand
    a_trans_word  -> the translated word (contents)

DESCRIPTION

    This function will determine if there are any Assembly errors based on the returning value of
    the error checking metholodigies and if there are errors record and display them

RETURNS

    Nothing since it is a void
*/
/**/

void Assembler::Display_assembly_errors(string a_opc, string a_instruction, int a_loc, string a_operand, string a_trans_word)
{ 
    if (Org_in_range(m_inst.Grab_OpCode(), a_instruction) == false)
    {
        Is_error_check(a_trans_word, "ERROR: ILLEGAL_VALUE_FOR_ORG");
    }
    else if (a_loc >= 1'000'000)
    {
        Is_error_check(a_trans_word, "ERROR: INSUFFICIENT_MEMORY_FOR_THE_TRANSLATION");
    }
    else if (Is_letter(a_operand) == true)
    {
        Is_error_check(a_trans_word, "SYNTAX_ERROR: ASSEMBLY_INSTRUCTION_MUST_BE_NUMERIC");
    }
    else if (a_loc < 0)
    {
        Is_error_check(a_trans_word, "ERROR: MULTIPLY_DEFINED_SYMBOLS");
    }
    if (m_facc.Is_to_big(a_instruction) == true)
    {
        Is_error_check(a_instruction, "ERROR: NUMBER_CAN'T_BE_MORE_THEN_NINE_DIGITS");
    }
}

/**/
/*
 Assembler::Op_code_converter(string a_opc)
NAME

    Assembler::Op_code_converter - converts the opcode to a number

SYNOPSIS

    int Assembler::Op_code_converter(string a_opc);
    a_opc -> the opcode to be converted

DESCRIPTION

    This function will convert the opcode from a string to an int
    for ease of comparsion in other function (helper function)
RETURNS

    Returns the opcode in integer form
*/
/**/

int Assembler::Op_code_converter(string a_opc)
{
    stringstream convert;
    int opc = 0;
    convert << a_opc;
    convert >> opc;
    return opc;
}

/**/
/*
 Assembler::RunProgramInEmulator()
NAME

    Assembler::RunProgramInEmulator - Run emulator on the translation.

SYNOPSIS

    void Assembler::RunProgramInEmulator();

DESCRIPTION

    This function will execute the emulator after the code is translated (assuming there are no errors)

RETURNS

    nothing since it is a void
*/
/**/

void Assembler::RunProgramInEmulator()
{
  
    Errors check;                                                                       
    bool problems = check.Is_corrupt();
    // if errors are present (problems == true) then display the errors in a BSOD format                                              
    if (problems == true)                                                               
    {
        cout << "__________________________________________________________" << endl;
        cout << endl;
        cout << "Press Enter to continue..." << endl;  
        cin.ignore();
        check.DisplayErrors();
    }
    else                                  
    {
    // Since no errors were reported, then we can run the emulator
        cout << "__________________________________________________________" << endl;
        cout << endl;
        cout << "Press Enter to continue..." << endl;
        cin.ignore();
        cout << endl;
        cout << "Results from emulating program:" << endl;
        cout << endl;
        m_emul.runProgram();   
    // Play a Quack sound twice
        PlaySound(TEXT("Duck.wav"), NULL, SND_SYNC);
        PlaySound(TEXT("Duck.wav"), NULL, SND_SYNC);
    // Draw my instructors favorite animal a duck
        m_emul.Success_Picture("Duck.txt");
    // Play a Quack sound 3 times
        PlaySound(TEXT("Duck.wav"), NULL, SND_SYNC);
        PlaySound(TEXT("Duck.wav"), NULL, SND_SYNC);
        PlaySound(TEXT("Duck.wav"), NULL, SND_SYNC);
        cout << endl;
    // Play the Windows 95 Shutdown sound
        PlaySound(TEXT("Shutdown.wav"), NULL, SND_SYNC);
    }
}

/**/
/*
 Assembler::DisplaySymbolTable()
NAME

    Assembler::DisplaySymbolTable - Display the symbols in the symbol table

SYNOPSIS

    void Assembler::DisplaySymbolTable();

DESCRIPTION

    This function will call DisplaySymbolTable() from the symtab class ultizling the m_symtab object

RETURNS

    nothing since it is a void
*/
/**/

void Assembler::DisplaySymbolTable()
{
    m_symtab.DisplaySymbolTable();
}

/**/
/*
 Assembler::Op_code_digits(string a_opc, string a_trans_word, string a_operand, int a_loc)
NAME

    Assembler::Op_code_digits - Determines the Opcode's digits

SYNOPSIS

    string Assembler::Op_code_digits(string a_opc, string a_trans_word, string a_operand, int a_loc);
    a_opc        -> the Opcode
    a_trans_word -> The word (contents) that will get the op code digits
    a_operand    -> The operand used in the assembler instructions
    a_loc        -> the location of the line
DESCRIPTION

    This function will determine the first 2 digits of the contents 
    (opcode) if it is machine language. ELSE (if it's assembly) will determine
    the last 5 digits of the contents.

RETURNS

    Returns the the first 2 digits of the contents 
    (opcode) if it is machine language. ELSE (if it's assembly) will return
    the last 5 digits of the contents.
*/
/**/

string Assembler::Op_code_digits(string a_opc, string a_trans_word, string a_operand, int a_loc)
{
    string trans_opc;
    if (m_is_assembly == false)
    {
        if (a_opc == "ADD")
        {
            trans_opc += "01";
        }
        else if (a_opc == "SUB")
        {
            trans_opc += "02";
        }
        else if (a_opc == "MULT")
        {
            trans_opc += "03";
        }
        else if (a_opc == "DIV")
        {
            trans_opc += "04";
        }
        else if (a_opc == "LOAD")
        {
            trans_opc += "05";
        }
        else if (a_opc == "STORE")
        {
            trans_opc += "06";
        }
        else if (a_opc == "ADDR")
        {
            trans_opc += "07";
        }
        else if (a_opc == "SUBR")
        {
            trans_opc += "08";
        }
        else if (a_opc == "MULTR")
        {
            trans_opc += "09";
        }
        else if (a_opc == "DIVR")
        {
            trans_opc += "10";
        }
        else if (a_opc == "READ")
        {
            trans_opc += "11";
        }
        else if (a_opc == "WRITE")
        {
            trans_opc += "12";
        }
        else if (a_opc == "B")
        {
            trans_opc += "13";
        }
        else if (a_opc == "BM")
        {
            trans_opc += "14";
        }
        else if (a_opc == "BZ")
        {
            trans_opc += "15";
        }
        else if (a_opc == "BP")
        {
            trans_opc += "16";
        }
        else if (a_opc == "HALT")
        {
            trans_opc += "17";
        }
        else
        {
            trans_opc += "??";
        }
    }
    else
    {
        trans_opc = Assembly_error_check(a_opc, trans_opc, a_operand, a_loc);
    }
    return trans_opc;
}

/**/
/*
 Assembler::Assembly_error_check(string a_opc, string& a_trans_opc, string& a_operand, int a_loc)
NAME

    Assembler::Assembly_error_check - Determines if their are errors in Assembly instructions

SYNOPSIS

    string Assembler::Assembly_error_check(string a_opc, string& a_trans_opc, string& a_operand, int a_loc);
    a_opc        -> the Opcode
    a_trans_word -> The word (contents) that will get the op code digits
    a_operand    -> The operand used in the assembler instructions
    a_loc        -> the location of the line

DESCRIPTION

    This function will determine if the assembly instructions have errors.
    If their are errors add in the right amount of question marks to the translanted word

RETURNS

    Returns the translated word with the amount of question marks it need (if their are assemblyerrors present) 
*/
/**/

string Assembler::Assembly_error_check(string a_opc, string& a_trans_opc, string& a_operand, int a_loc)
{
    m_assembly_errors = false;
    m_is_assembly = true;
    if (Is_letter(a_operand) == false && Keep_random_reg(m_inst.Grab_Instructions())== true && ((a_opc != "DS" && a_opc != "ORG") && a_loc >= 0))
    {
        a_trans_opc += a_operand;
    }
    else if ((Is_letter(a_operand) == true || Keep_random_reg(m_inst.Grab_Instructions()) == false) && ((a_opc != "DS" && a_opc != "ORG") || (a_loc < 0)))
    {
        a_trans_opc.assign(9, '?');
    }
    else
    {
    // I did this since in DS their is NO specified contents 9 NULL characters to keep spacing even and perfectly lined up  
        if (Org_in_range(a_opc, m_inst.Grab_Instructions()) == false || (a_loc >= 1'000'000 || a_loc < 0)  || Is_letter(a_operand) == true || Keep_random_reg(m_inst.Grab_Instructions()) == false)
        {
            m_assembly_errors = true;
        }
        a_trans_opc.assign(9, char(0));  
    }
    return a_trans_opc;
}

/**/
/*
 Assembler::Org_in_range(string a_opc, string a_instruction)
NAME

    Assembler::Org_in_range - is the ORG instruction in the approiate range

SYNOPSIS

    bool Assembler::Org_in_range(string a_opc, string a_instruction);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the ORG instruction is in tha approiate range (ORG >= 0 AND ORG < 1,000,000) 
    if so return true. ELSE return false

RETURNS

    Returns true if the ORG instruction is in tha approiate range 
    ELSE return false
*/
/**/

bool Assembler::Org_in_range(string a_opc, string a_instruction)
{
    if (a_opc == "ORG")
    {
        a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ' '), a_instruction.end());
        for (char i = 'A'; i <= 'z'; i++)
        {
            a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), i), a_instruction.end());
        }
        int number = Op_code_converter(a_instruction);
        if(number < 0 || number >= 1'000'000)
        {
            m_assembly_errors = true;
            return false;
        }
    }
    return true;
}

/**/
/*
 Assembler::Keep_random_reg(string a_instruction)
NAME

    Assembler::Keep_random_reg - should the random register value be kept if there is an error

SYNOPSIS

    bool Assembler::Keep_random_reg(string a_instruction);
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the random register value assigned
    should be kept in contents (when the register number is NOT SPECIFIED (where applicable))
    AND when there is an error if both conditions are true (keep random register) return true. 
    ELSE return false (delete random register).

RETURNS

    Returns true if the random register value should be kept
    ELSE return false
*/
/**/

bool Assembler::Keep_random_reg(string a_instruction)
{
    // if instructions are machine instructions
    if (m_is_assembly == false)
    {   
        while ((a_instruction.back() >= 'A' && a_instruction.back() <= 'z') || a_instruction.back() == ',' || a_instruction.back() == ' ')
        {
            a_instruction.pop_back();
        }
    }
    // delete comments
    for (int i = 0; i < a_instruction.size(); i++)
    {
        while (a_instruction[i] == ';')
        {
            a_instruction.pop_back();
        }
    }
    // delete spaces
    a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ' '), a_instruction.end());
    // if there is a numerical value present
    if (a_instruction.back() >= '0' && a_instruction.back() <= '9' )
    {
        return true;
    }
    return false;
}

/**/
/*
 Assembler::Has_comma(string a_opc, string a_instruction)
NAME

    Assembler::Has_comma - is there a comma

SYNOPSIS

    bool Assembler::Has_comma(string a_opc, string a_instruction);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the instruction contains a comma (where applicable)
    if so return true. ELSE return false

RETURNS

    Returns true if the ORG instruction contains a comma (where applicable)
    ELSE return false
*/
/**/

bool Assembler::Has_comma(string a_opc, string a_instruction)
{
    int opc = Op_code_converter(a_opc);
    bool num = false;
    bool comma = false;
    for (int i = 0; i < a_instruction.size(); i++)
    {
        while (a_instruction[i] == ';')
        {
            a_instruction.pop_back();
        }
    } 
    a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), ' '), a_instruction.end());
    for (char i = 'A'; i <= 'z'; i++)
    {  
        a_instruction.erase(remove(a_instruction.begin(), a_instruction.end(), i), a_instruction.end());
    }
    if (a_instruction.find(',') != string::npos)
    {
        comma = true;
    }
    if ((opc == 17) || (((opc >= 1 && opc <= 10) || (opc >= 14 && opc <= 16)) && (comma == true)))
    {
        return true;
    }
    else if (((opc >= 1 && opc <= 10) || (opc >= 14 && opc <= 16)) && (comma == false))
    { 
        return false;
    }
    return true;
}

/**/
/*
 Assembler::Reg_in_range(string a_opc, string a_instruction)
NAME

    Assembler::Reg_in_range - is registerr number in the approiate range

SYNOPSIS

    bool Assembler::Reg_in_range(string a_opc, string a_instruction);
    a_opc         -> The opcode
    a_instruction -> The spiecified instructions

DESCRIPTION

    This function will determine if the instruction's register is in the approiate
    range (where applicable) if so return true. ELSE return false.

RETURNS

    Returns true if the instruction's register is in the approiate
    range (where applicable). ELSE return false
*/
/**/

bool Assembler::Reg_in_range(string a_opc, string a_instruction)
{
    int opc = Op_code_converter(a_opc);
    int reg = m_symtab.Grab_reg_num(a_instruction);
    // single register instructions must have a register number between 0 and 9
    if (((opc >= 1 && opc <= 6) || (opc >= 11 && opc <= 16)) && (reg < 10 && reg >= 0))
    {
        return true;
    }
    else if (((opc >= 1 && opc <= 6) || (opc >= 11 && opc <= 16)) && (reg >= 10 || reg < 0))
    {
        return false; 
    }
    else if ((opc >= 7 && opc <= 10) && (reg >= 10 && reg <= 99))
    {
        return true;
    }
    else if ((opc >= 7 && opc <= 10) && (reg >= 100 || reg < 10))
    {
        return false;
    }
    return true;
}

/**/
/*
 Assembler::Zero_filler(string &a_trans_word, int a_pos)
NAME

    Assembler::Zero_filler - adds 0's to the translated word

SYNOPSIS

    string Assembler::Zero_filler(string &a_trans_word, int a_pos);
    a_trans_word -> The word to add 0's to
    a_pos        -> The spiecified position (or index) in the translated word to
                    insert the zeros string

DESCRIPTION

    This function will determine and add the amount of 0's needed to the translated
    word to make it 9 digits long

RETURNS

    Returns the translated word with the proper amount of 0's
*/
/**/

string Assembler::Zero_filler(string &a_trans_word, int a_pos)
{
    const int CONTENT_SIZE = 9;
    if (a_trans_word.size() != CONTENT_SIZE)
    {
        string zeros;
    // determine the amount of 0's needed to make the content size 9 (9 digits)
        int differ = int(CONTENT_SIZE - a_trans_word.size());
    // generate the extra 0 amount based on the value of differ
        zeros.assign(differ, '0');
    // insert the generated 0's to the translated word at the specified position 
        a_trans_word.insert(a_pos, zeros);
    }
    return a_trans_word;
}

/**/
/*
 Assembler::Machine_zero_filler(string &a_trans_word, int a_reg_num)
NAME

    Assembler::Machine_zero_filler - adds 0's to the translated word when using machine language

SYNOPSIS

    string Assembler::Zero_filler(string &a_trans_word, int a_reg_num);
    a_trans_word -> The word to add 0's to
    a_reg_num    -> The spiecified register number

DESCRIPTION

    This function will determine and add the amount of 0's needed to the translated
    word to make it 9 digits long when it is machine instructions since machine is a bit ddifferent then 
    assembly so it needs it's own function to determmine where to insert the zeros at the specified position
    (based on the register number)

RETURNS

    Returns the translated word with the proper amount of 0's
*/
/**/

string Assembler::Machine_zero_filler(string& a_trans_word, int a_reg_num)
{
    bool check = false;
    string opcode;
    // Get the first 2 digits
    opcode.push_back(a_trans_word[0]);
    opcode.push_back(a_trans_word[1]);
    int opc = Op_code_converter(opcode);
    for (int i = 0; i < a_trans_word.size(); i++)
    {
        if (a_trans_word[i] == '?')
        {
            check = true;
            break;
        }
    }
    if (a_reg_num < 10 && ((opc >= 1 && opc <= 6) || (opc >= 11 && opc <= 17)))
    {
        Zero_filler(a_trans_word, 3);
    }
    else
    {
        Zero_filler(a_trans_word, 4);
    }
    return a_trans_word;
}

/**/
/*
 Assembler::Translation_converter(int a_value)
NAME

    Assembler::Translation_converter - int to string

SYNOPSIS

    string Assembler::Translation_converter(int a_value);
    a_value -> The spiecified integer to convert to a string

DESCRIPTION

    This function will convert the specified integer to a string
    ultizling stringstream (bi directional) to be able to 
    add the proper information to the translated contents
    string(s).

RETURNS

    Returns the converted value in string form (int to string)
*/
/**/

string Assembler::Translation_converter(int a_value)
{
    // Ultize stringstream (bi directional) to convert the int to a string
    stringstream convert;
    string reg;
    convert << a_value;
    convert >> reg;
    if (a_value < 0 || a_value >= 1'000'000)
    {
        reg.assign(3, '?');
    }
    return reg;
}

/**/
/*
 Assembler::Contents_converter(string a_contents)
NAME

    Assembler::Contents_converter - string to long long

SYNOPSIS

    long long Assembler::Contents_converter(string a_contents);
    a_contents -> The spiecified string to convert to a long long

DESCRIPTION

    This function will convert the specified string to a long long
    for the emulation via the insert memory function

RETURNS

    Returns the converted value in long long form (string to long long)
*/
/**/

long long Assembler::Contents_converter(string a_contents)
{
    // Ultize stringstream (bi directional) to convert the string to a long long
    stringstream convert;
    long long trans_contents;
    convert << a_contents;
    convert >> trans_contents;
    return trans_contents;
}

/**/
/*
 Assembler::Location_is_negative(int a_loc)
NAME

    Assembler::Location_is_negative - is it negative

SYNOPSIS

    bool Assembler::Location_is_negative(int a_loc);
    a_loc -> The location

DESCRIPTION

    This function will determine if the specified location is negative
    if so return true. ELSE return false.

RETURNS

    Returns true if the specified location is negative. ELSE return false.
*/
/**/

bool Assembler::Location_is_negative(int a_loc)
{
    if (a_loc < 0)
    {
        return true;
    }
    return false;
}

/**/
/*
 Assembler::Location_beyond_mem(int a_loc)
NAME

    Assembler::Location_beyond_mem - is it greater then or equal to 1,000,000

SYNOPSIS

    bool Assembler::Location_beyond_mem(int a_loc);
    a_loc -> The location

DESCRIPTION

    This function will determine if the specified location is greater then or equal to 1,000,000
    if so return true. ELSE return false.

RETURNS

    Returns true if the specified location is greater then or equal to 1,000,000. ELSE return false.
*/
/**/

bool Assembler::Location_beyond_mem(int a_loc)
{
    if (a_loc >= 1'000'000)
    {
        return true;
    }
    return false;
}

/**/
/*
 Assembler::Display_location_errors(int a_loc)
NAME

    Assembler::Display_location_errors - displays the location errors

SYNOPSIS

    void Assembler::Display_location_errors(int a_loc);
    a_loc -> The location

DESCRIPTION

    This function will display location errors if there are any

RETURNS

    nothing since it is a void
*/
/**/

void Assembler::Display_location_errors(int a_loc)
{
    if (Location_is_negative(a_loc) == true)
    {
        Is_error_check("???", "ERROR: LOCATION_ILLEGAL_CAN'T_BE_LESS_THEN_ZERO");
    }
    else if (Location_beyond_mem(a_loc) == true)
    {
        Is_error_check("???", "ERROR: LOCATION_BEYOND_MEMORY_CAPACITY_OF_1,000,000_BYTES");
    }
}

/**/
/*
 Assembler::Line_alignment(string a_operand, int a_loc)
NAME

    Assembler::Line_alignment - indent the lines for better translation output

SYNOPSIS

    string Assembler::Line_alignment(string a_operand, int a_loc);
    a_operand -> the line getting the proper indentation
    a_loc     -> the location of the line

DESCRIPTION

    This function will determine the proper indentation based
    on the type of line it is (comment, end statement ETC)

RETURNS

    Returns the new_line value with the
    updated spacing for the neater output.
*/
/**/

string Assembler::Line_alignment(string a_line, int a_loc)
{
    // The 2 constants are used for reference with the alignment process
    const string STATEMENT_TITLE = "Location       Contents       O";
    const string CONTENT_TITLE = "Location    C";
    bool error = false;
    string spaces;
    string new_line = a_line;

    // for sack of comparsion make characters lowercase
    for (int i = 0; i < new_line.size(); i++)
    {
        new_line[i] = tolower(new_line[i]);
    }
    for (int i = 0; i < new_line.size(); i++)
    {
        if (new_line[i] == '?' || a_loc < 0)
        {
            error = true;
            break;
        }
    }
    if (a_loc != 100)
    {
        a_loc = 100;
    }
    // for ease of comparsion omit extra spaces
    new_line.erase(remove(new_line.begin(), new_line.end(), ' '), new_line.end());
    // If its a comment, end statement, or a line with a TAB
        if (new_line.find("org") != string::npos)
        {
          
            for (int i = 1; i < 16; i++)
            {
                spaces += ' ';
            }
            spaces += a_line;
            return spaces;
        }
    else if (new_line[0] == ';' || new_line[0] == char(9) || new_line.find("end") != string::npos)
    {
        for (int i = 1; i < STATEMENT_TITLE.size(); i++)
        {
            spaces += ' ';
        }
        spaces += a_line;
        return spaces;
    }
    // If the location is 100 or more and is NOT an end statement then its one of the following
    else if (a_loc >= 100 && new_line.find("end") == string::npos)
    {
        // If its a translated word or error
        if ((new_line.size() == 9) && (new_line[0] >= '0' && new_line[0] <= '9') || error == true)
        {
            for (int i = 1; i < CONTENT_TITLE.size(); i++)
            {
                spaces += ' ';
            }
            spaces += a_line;
            return spaces;
        }
        // If the statement does NOT contain the phrase "ds" (define storage)
        else if (new_line.find("ds") == string::npos)
        {
            for (int i = 1; i < 7; i++)
            {
                spaces += ' ';
            }
            spaces += a_line;
            return spaces;
        }
        // If the statement contains the phrase "ds" (define storage) 
        else if (new_line.find("ds") != string::npos)
        {
            for (int i = 1; i < STATEMENT_TITLE.size() - 9; i++)
            {
                spaces += ' ';
            }
            spaces += a_line;
            return spaces;
        }
         
    }
    else
    {
        for (int i = 1; i < 15; i++)
        {
            spaces += ' ';
        }
        spaces += a_line;
        return spaces;
    }
    return a_line;
}

/**/
/*
 Assembler::Font_reset()
NAME

    Assembler::Font_reset - reset the error effects 

SYNOPSIS

    void Assembler::Font_reset();

DESCRIPTION

    This function will reset the error effects back to standard format

RETURNS

    nothing since it is a void
*/
/**/

void Assembler::Font_reset()
{
    // The determined font size before declaration
    int Letter_Size = 16;
    // White is the default color
    const int WHITE = 15;
    HANDLE console_color;
    // Modify the output styling properties
    console_color = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console_color, WHITE);
    CONSOLE_FONT_INFOEX font;
    font.cbSize = sizeof(font);
    // declare the size of the font
    font.dwFontSize.Y = Letter_Size;
    // Choose your font                  
    wcscpy(font.FaceName, L"Consolas");
    // Now with everything we got we now set the font
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &font);
}