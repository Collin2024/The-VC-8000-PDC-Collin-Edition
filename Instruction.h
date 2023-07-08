//
// Class to parse and provide information about instructions.  Note: you will be adding more functionality.
//
#pragma once

// The elements of an instruction.
class Instruction 
{

public:

    // Codes to indicate the type of instruction we are processing.  Why is this inside the
    // class?  We should make this an enum class.  We will do this during a lecture.
    enum class InstructionType 
    {

        // A machine language instruction.
        ST_MachineLanguage, 

        // Assembler Language instruction.
        ST_AssemblerInstr,

        // Comment or blank line
        ST_Comment, 

        // end instruction.
        ST_End,     

        // Statement has an error.
        ST_Error                
    };

    // Parse the Instruction.
    InstructionType ParseInstruction(string a_line);
    

    // Compute the location of the next instruction.
    int LocationNextInstruction(int a_loc);

    // To access the label
    inline string &GetLabel() 
    {
        return m_Label;
    };

    // To determine if a label is blank.
    inline bool isLabel()
    {
        return !m_Label.empty();
    };

    // the grabbers "getters"
    string Grab_OpCode();
    string Grab_Instructions();
    string Grab_oper1();
    string Grab_oper2();



private:


    // The elemements of a instruction
  
    // The label.
    string m_Label;        
    string m_OpCode;       // The symbolic op code.
    string m_Operand1;     // The first operand. 
    string m_Operand2;     // The second operand.

    // The original instruction.
    string m_instruction;   

    // Derived values.
   
    // The numerical value of the op code for machine language equivalents.
    int m_NumOpCode = 0;     
   
    // The type of instruction.
    InstructionType m_type = InstructionType::ST_Error; 

    // == true if the operand 1 is numeric.
    bool m_IsNumericOperand1 = false;
    int m_Operand1NumericValue = 0;   // The value of the operand 1 if it is numeric.
    
    bool m_IsNumericOperand2 = false;// == true if the operand 2 is numeric.
    int m_Operand2NumericValue = 0;   // The value of the operand 2 if it is numeric.

    // Delete any comments from the statement.
    void DeleteComment(string& a_line);

    // Record the fields of the instructions.
    bool RecordFields(const string &a_line);

    // Get the fields that make up the statement.  This function returns false if there are extra fields.
    bool ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode, string& a_Operand1, string& a_Operand2);

    // Check if a string contains a number. 
    bool isStrNumber(const string& a_str);
    
};


