//
//		Emulator class - supports the emulation of VC8000 programs
//
#ifndef _EMULATOR_H      
// UNIX way of preventing multiple inclusions.
#define _EMULATOR_H

class emulator 
{

public:

    // The size of the memory of the VC8000.
    const static int MEMSZ = 1'000'000;

    // constructor 
    emulator();

    // Records instructions and data into simulated memory.
    bool insertMemory(int a_location, long long a_contents);
    
    // Runs the program recorded in memory.
    bool runProgram( );

    // draw a duck 
    void Success_Picture(string type);



private:

    // Memory for the VC8000
    vector<long long> m_memory;

    // Instruction types
    long long Single_reg_instruction(long long a_opc, long long a_addr, long long& a_index);
    long long Multi_reg_instruction(long long a_opc, long long& a_index);
    void Read_write_instruction(long long a_opc, long long a_addr);
    long long Branch_instruction(long long a_opc, long long a_addr, long long &a_index);

    // holds the registers for the two of them to access
    vector<long long> m_register;

    // determine the reg number for the emulation process
    long long Find_reg_num(long long a_contents, long long a_opc);

    // Set the main register equal to the specified register container location
    void Set_main_reg(long long& a_index);

    // The main register
    long long m_main_reg = 0;

    // take care of the divide by 0 error
    void Divide_by_zero(long long a_denominator);
};

#endif

