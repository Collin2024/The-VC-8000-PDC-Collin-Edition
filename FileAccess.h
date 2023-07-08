//
//		File access to source file.
//
#ifndef _FILEACCESS_H  // This is the way that multiple inclusions are defended against often used in UNIX
#define _FILEACCESS_H  // We use pragmas in Visual Studio and g++.  See other include files

#include <fstream>
#include <stdlib.h>
#include <string>

class FileAccess {

public:

    // Opens the file.
    FileAccess( int argc, char *argv[] );

    // Closes the file.
    ~FileAccess( );

    // Get the next line from the source file.  Returns true if there was one.
    bool GetNextLine( string &a_line );

    // Put the file pointer back to the beginning of the file.
    void rewind( );

    // CHECKING FOR NUMBERS MORE THEN 9 DIGITS TO PREVENT A SYSTEM CRASH
    // AKA AN EXCEPTION THROWN ERROR
    void File_num_check();
    
    // reset the file to its original contents
    void File_error_reset();

    // error checking methologie for digit size
    bool Is_to_big(string a_line);



private:

    // Source file object.
    ifstream m_sfile;

    // ERROR CHECKING METHOLODIES
    string Lower_case(string a_line);
    string Single_reg_num(string& a_line, string a_original, int a_loc);
    string Double_reg(string& a_line);
   
    void Num_in_val_range(string a_read_file, string a_write_file);
    int File_line_count(string a_read_file);
    void Write(string a_main_file, string a_new_file);

    // file identifiers
    const string NO_ERROR_CHECK = "TEMPORARYTest.tmp";
    const string ERROR_CHECK = "MODIFIEDTest.tmp";
    const string MAIN_FILE = "Test.txt";

};
#endif

