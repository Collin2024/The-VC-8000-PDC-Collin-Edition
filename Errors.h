//
// Class to manage error reporting. Note: all members are static so we can access them anywhere.
// What other choices do we have to accomplish the same thing?
//
#ifndef _ERRORS_H
#define _ERRORS_H

#include <string>
#include <vector>

class Errors {

public:
    
    // Initializes error reports.
    static void InitErrorReporting();

    // Records an error message.
    static void RecordError(string a_emsg);

    // Displays the collected error message.
    static void DisplayErrors();

    // Are their errors present 
    bool Is_corrupt();

    // Add the approiate error effects for the Blue Screen Of Death
    static void Visual_error_effects();

    // draws the skull and cross bones
    static void Fail_picture(string a_name);



private:

    // holds the errors
    static vector<string> m_ErrorMsgs;  // This must be declared in the .cpp file.  Why?
};
#endif