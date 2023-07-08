#include "stdafx.h"

#include "Instruction.h"

/**/
/*
Instruction::ParseLineIntoFields(string &a_line)

NAME

    Instruction::ParseLineIntoFields - delete the comments

SYNOPSIS

    bool Instruction::ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode, string& a_Operand1, string& a_Operand2);
    a_line -> The instructions

DESCRIPTION

    This function will delete the comments in an instruction (everything after ; )

RETURNS

    Nothing since it is a void
*/
/**/

void Instruction::DeleteComment(string& a_line)
{
    size_t isemi1 = a_line.find(';');
    if (isemi1 != string::npos)
    {
        a_line.erase(isemi1);
    }
}

/**/
/*
Instruction::ParseInstruction(string a_line)

NAME

    Instruction::ParseInstruction - Reads the instruction and will return its instruction type

SYNOPSIS

    Instruction::InstructionType Instruction::ParseInstruction(string a_line);
    a_line -> The instructions
    

DESCRIPTION

    Reads the instruction (a_line) and will return its instruction type

RETURNS

       The instruction type
*/
/**/
    
Instruction::InstructionType Instruction::ParseInstruction(string a_line)
{
    // parse an instruction and return its type.  Yes, I want a call by value.
    // Record the original statement.  This will be needed in the sceond pass.
    m_instruction = a_line;

    // Delete any comment from the line.
    DeleteComment( a_line );

    // Record label, opcode, and operands.  Up to you to deal with formatting errors.
    bool isFormatError = RecordFields( a_line );
    
    // Check if this is a comment.
    if (m_Label.empty() && m_OpCode.empty())
    {
        return InstructionType::ST_Comment;
    }
    else if ((m_OpCode == "DS") || (m_OpCode == "DC") || (m_OpCode == "ORG"))
    {
        return InstructionType::ST_AssemblerInstr;
    }
    else if (m_OpCode == "END")
    {
        return InstructionType::ST_End;
    }
    else if((m_OpCode == "ADD") || (m_OpCode == "SUB") || (m_OpCode == "MULT")||(m_OpCode == "DIV") || (m_OpCode == "LOAD") || (m_OpCode == "STORE")||(m_OpCode == "ADDR") || (m_OpCode == "SUBR") || (m_OpCode == "MULTR")||(m_OpCode == "DIVR") || (m_OpCode == "READ") || (m_OpCode == "WRITE")||(m_OpCode == "B") || (m_OpCode == "BM") || (m_OpCode == "BZ")||(m_OpCode == "BP") || (m_OpCode == "HALT"))
    {
        return InstructionType::ST_MachineLanguage;
    }
    else
    {
        return InstructionType::ST_Error;
    }
    // Return the instruction type.
    
    return m_type;
}

/**/
/*
Instruction::RecordFields(const string &a_line)

NAME

    Instruction::RecordFields - Record the fields that make up the instructions.

SYNOPSIS

    bool Instruction::RecordFields( const string &a_line );
    a_line -> The instructions


DESCRIPTION

    Records and gets the fields that makes up the instructions

RETURNS

       true
*/
/**/

bool Instruction::RecordFields( const string &a_line )
{
    // Record the fields that make up the instructions.
    // Get the fields that make up the instruction.
    bool isFormatError = ParseLineIntoFields( a_line, m_Label, m_OpCode, m_Operand1, m_Operand2);

    // if code was a comment, there is nothing to do.
    if (m_OpCode.empty() && m_Label.empty()) return isFormatError;

    // Record whether the operands are numeric and their value if they are.
    m_IsNumericOperand1 = isStrNumber(m_Operand1);
    if (m_IsNumericOperand1) m_Operand1NumericValue = stoi(m_Operand1);

    m_IsNumericOperand2 = isStrNumber(m_Operand2);
    if (m_IsNumericOperand2) m_Operand1NumericValue = stoi(m_Operand2);

    // For the sake of comparing, convert the op code to upper case.
    for (char& c : m_OpCode)
    {
        c = toupper(c);
    }
    // I leave to the class the tasks:
    // - Determine and record the instruction type from the op code.
    // - Recording the numberic Op code for machine lanuage equivalents.
    return true;
}

/**/
/*
Instruction::ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode, string& a_Operand1, string& a_Operand2)

NAME

    Instruction::ParseLineIntoFields - Record the fields that make up the instructions.

SYNOPSIS

    bool Instruction::ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode, string& a_Operand1, string& a_Operand2);
    a_line     -> The instructions
    a_label    -> The label
    a_OpCode   -> The symbolic opcode
    a_Operand1 -> The first operand
    a_Operand2 -> The second operand

DESCRIPTION

    Omits commas and Parses the instructions into the approiate fields

RETURNS

    Returns true if the string (endStr) is empty
    ELSE if there is extra data return false
*/
/**/

bool Instruction::ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode, string& a_Operand1, string& a_Operand2)
{
    // Get rid of any commas from the line.
    // Parse the intruction into fields.
    replace(a_line.begin(), a_line.end(), ',', ' ');

    // Get the elements of the line.  That is the label, op code, operand1, and operand2.
    string endStr;
    a_label = a_OpCode = a_Operand1 = a_Operand2 = "";
    istringstream ins(a_line);
    if (a_line[0] == ' ' || a_line[0] == '\t')
    {
        a_label = "";
        ins >> a_OpCode >> a_Operand1 >> a_Operand2 >> endStr;
    }
    else
    {
        ins >> a_label >> a_OpCode >> a_Operand1 >> a_Operand2 >> endStr;
    }
    // If there is extra data, return false.
    return endStr.empty() ? true : false;
}

/**/
/*
Instruction::isStrNumber(const string& a_str)

NAME

    Instruction::isStrNumber - Determines if the string is a number

SYNOPSIS

    bool Instruction::isStrNumber(const string& a_str);
    a_str -> The instructions


DESCRIPTION

    Determines if the string is a number by checking for digits returns false if
    it contains - or + operators OR if it does NOT contains digits (0-9) 
    ELSE it is a number and it returns true

RETURNS

    Returns true if the string is a number
    ELSE return false
*/
/**/

bool Instruction::isStrNumber(const string& a_str)
{
    if (a_str.empty()) return false;

    // If there is a - or a plus, make sure there are more characters.
    int ichar = 0;
    if (a_str[0] == '-' || a_str[0] == '+')
    {
        ichar++;
        if (a_str.length() < 2) return false;
    }
    // Make sure that the remaining characters are all digits
    for (; ichar < a_str.length(); ichar++)
    {
        if (!isdigit(a_str[ichar])) return false;
    }
    return true;
}

/**/
/*
Instruction::LocationNextInstruction(int a_loc)
NAME

    Instruction::LocationNextInstruction - Determines the next location

SYNOPSIS

    int Instruction::LocationNextInstruction(int a_loc);
    a_loc -> The location


DESCRIPTION

    Determines the next location of the next instruction by 
    checking if opcode is "DS" OR "ORG" and if it is then you
    to return a_loc + m_Operand1NumericValue ELSE return loc incremented by 1

RETURNS

    the location of the next instruction 
*/
/**/

int Instruction::LocationNextInstruction(int a_loc)
{
    
    if (m_OpCode == "DS" || m_OpCode == "ORG")
    {
        return a_loc + m_Operand1NumericValue;
    }
    // since this is the next instruction increase a_loc by 1
    return a_loc+1;
}

/**/
/*
Instruction::Grab_OpCode()
NAME

    Instruction::Grab_OpCode - OpCode grabber

SYNOPSIS

    string Instruction::Grab_OpCode();


DESCRIPTION

    It gets the opcode

RETURNS

    the m_OpCode
*/
/**/

string Instruction::Grab_OpCode()
{
    return m_OpCode;
}

/**/
/*
Instruction::Grab_Instructions()
NAME

    Instruction::Grab_Instructions - Instructions grabber

SYNOPSIS

    string Instruction::Grab_Instructions();


DESCRIPTION

    It gets the Instructions

RETURNS

    the m_instruction
*/
/**/

string Instruction::Grab_Instructions()
{
    return m_instruction;
}

/**/
/*
Instruction::Grab_oper1()
NAME

    Instruction::Grab_oper1 - 1st operand grabber

SYNOPSIS

    string Instruction::Grab_oper1();


DESCRIPTION

    It gets the 1st operand

RETURNS

    the m_Operand1
*/
/**/

string Instruction::Grab_oper1()
{
    return m_Operand1;
}

/**/
/*
Instruction::Grab_oper2()
NAME

    Instruction::Grab_oper2 - 2nd operand grabber

SYNOPSIS

    string Instruction::Grab_oper2();


DESCRIPTION

    It gets the 2nd operand

RETURNS

    the m_Operand2
*/
/**/

string Instruction::Grab_oper2()
{
    return m_Operand2;
}