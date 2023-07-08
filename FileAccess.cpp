//
//  Implementation of file access class.
//
#include "stdafx.h"
#include "FileAccess.h"

/**/
/*
FileAccess::FileAccess( int argc, char *argv[] )
NAME

    Instruction::FileAccess - File opener

SYNOPSIS

    FileAccess::FileAccess( int argc, char *argv[] );
    argc -> Determines the amount of arguements that were read in from the CMD
    argv -> Filename

DESCRIPTION

    It opens the file checks to make sure that there is only one runtime parameter
    if their isn't exit(1) also if the file opening process failed exit(1).

RETURNS

    Nothing since its a constructor
*/
/**/

FileAccess::FileAccess( int argc, char *argv[] )
{
    // Check that there is exactly one run time parameter.
    if( argc != 2 ) {
        cerr << "Usage: Assem <FileName>" << endl;
        exit( 1 );
    }
    // Open the file.  One might question if this is the best place to open the file.
    // One might also question whether we need a file access class.
    m_sfile.open( argv[1], ios::in );

    // If the open failed, report the error and terminate.
    if( ! m_sfile ) {
        cerr << "Source file could not be opened, assembler terminated."
            << endl;
        exit( 1 ); 
    }
}

/**/
/*
FileAccess::~FileAccess( )
NAME

    FileAccess::~FileAccess - File closer

SYNOPSIS

    FileAccess::~FileAccess( );


DESCRIPTION

    It closes the file and deletes the created temporary files

RETURNS

    Nothing since its a destructor
*/
/**/

FileAccess::~FileAccess()
{
    // Not that necessary in that the file will be closed when the program terminates, but good form.
    m_sfile.close(); 
}

/**/
/*
FileAccess::GetNextLine( string &a_line )
NAME

    Instruction::GetNextLine - gets the next line

SYNOPSIS

    FileAccess::GetNextLine( string &a_line );


DESCRIPTION

    Get the next line from the source file. 

RETURNS

    Returns true if there is a next line. ELSE it returns false
*/
/**/

bool FileAccess::GetNextLine( string &a_line )
{
    // Get the next line from the file.
    // If there is no more data, return false.
    if(m_sfile.eof()) 
    {
        return false;
    }
    getline( m_sfile, a_line );
    // Return indicating success.
    return true;
}

/**/
/*
FileAccess::rewind( )
NAME

    FileAccess::rewind - start at the begining of the file


SYNOPSIS

   void FileAccess::rewind();


DESCRIPTION

    Clean all file flags and go back to the 
    beginning of the file.


RETURNS

    Returns nothing since its void
*/
/**/

void FileAccess::rewind()
{
    // Clean all file flags and go back to the beginning of the file.
    m_sfile.clear();
    m_sfile.seekg( 0, ios::beg );
}
 
/**/
/*
FileAccess::Lower_case(string a_line)
NAME

    FileAccess::Lower_case - make everything lower case

SYNOPSIS

   string FileAccess::Lower_case(string a_line);

DESCRIPTION

    make all characters of a line lower case
    a_line -> the line to make everything lower case

RETURNS

    Returns the line all lower cased
*/
/**/

string FileAccess::Lower_case(string a_line)
{
    for (int i = 0; i < a_line.size(); i++)
    {
        a_line[i] = tolower(a_line[i]);
    }
    return a_line;
}

/**/
/*
FileAccess::Single_reg_num(string& a_line, string a_original, int a_loc)
NAME

    FileAccess::Single_reg_num - inserts question marks where applicable for single register or assembly languagelines

SYNOPSIS

   string FileAccess::Single_reg_num(string& a_line, string a_original, int a_loc);
   a_line     -> the line to add ?'s
   a_original -> the original line
   a_loc      -> the location

DESCRIPTION

    This function will determine if necessary (execution based on the num in range functions response) to insert question marks based on if a number 
    is more then 9 digits for single register lines

RETURNS

    Returns the line with the question marks if there is a 
    numerical value with more then 9 digits
*/
/**/

string FileAccess::Single_reg_num(string& a_line, string a_original, int a_loc)
{
    // find the key assembly language phrases ELSE its machine language
    if (Lower_case(a_original).find("ds") != std::string::npos)
    {
        a_line += "?????????";
    }
    else if (Lower_case(a_original).find("dc") != std::string::npos)
    {
        a_line += "?????????";
    }
    else if (Lower_case(a_original).find("org") != std::string::npos)
    {
        a_line += "?????????";
    }
    else
    {
        int counter = 0;
        for (int i = 0; i <= a_line.size() + counter; i++)
        {
            if (a_line[i] == ',' && (a_line[i + 1] >= '0' && a_line[i + 1] <= '9'))
            {
                a_line.pop_back();
                counter++;
            }
        }
        a_line.insert(a_loc, "?");
    }
    return a_line;
}

/**/
/*
FileAccess::Double_reg(string& a_line)
NAME

    FileAccess::Double_reg - inserts question marks where applicable in multi register lines

SYNOPSIS

   string FileAccess::Double_reg(string& a_line);
   a_line -> the line to add ?'s

DESCRIPTION

    This function will determine if necessary (based on the num in range functions response) to insert question marks based on if a number
    is more then 9 digits for multi register lines

RETURNS

    Returns the line with the question marks
*/
/**/

string FileAccess::Double_reg(string& a_line)
{
    int counter = 0;
    int start = 0;
    int sec_reg = 0;
    string second;
    bool has_error = false;
    int next_pos = int(Lower_case(a_line).find(",") + 1);
    if (Lower_case(a_line).find(",") != std::string::npos && a_line[0] != ';' && (a_line[next_pos] >= '0' && a_line[next_pos] <= '9'))
    {
        for (int i = 0; i < a_line.size(); i++)
        {
            if (a_line[i] >= '0' && a_line[i] <= '9')
            {
                counter++;
            }
    // only alter if we need to this is for reduncy
            else if (a_line[i] == ',' && counter > 9)
            {
                start = i;
                second.push_back(',');
                for (int x = i; x <= a_line.size(); x++)
                {
                    if (a_line[x] >= '0' && a_line[x] <= '9')
                    {
                        sec_reg++;
                        second.push_back(a_line[x]);
                    }
                }
                break;
            }
        }
    }
    if (counter > 9)
    {
        a_line.erase(counter - 2, start);
     
        for (char i = '0'; i <= '9'; i++)
        {
            a_line.erase(remove(a_line.begin(), a_line.end(), i), a_line.end());
        }
        if(sec_reg > 9)
        {
            for (char i = '0'; i <= '9'; i++)
            {
                a_line.erase(remove(a_line.begin(), a_line.end(), i), a_line.end());
            }
            second.clear();
            second.push_back(',');
            second.push_back('?');
        }
        a_line += ("?" + second);
    }   
    return a_line;
}

/**/
/*
FileAccess::Is_to_big(string a_line)
NAME

    FileAccess::Single_reg_num - is the numerical values too big


SYNOPSIS

   bool FileAccess::Is_to_big(string a_line);
   a_line -> the line that is read in 

DESCRIPTION

    This function will determine if a number is more then 9 digits based on the presence of question marks 
    in the line if they are there then it is more then 9 digits if this is the case return true. ELSE return false.

RETURNS

    Returns true if its more then 9 digits. ELSE return false.
*/
/**/

bool FileAccess::Is_to_big(string a_line)
{
    if (Lower_case(a_line).find('?') != std::string::npos && a_line[0] != ';')
    {
        return true;
    }
    return false;
}

/**/
/*
FileAccess::Write(string a_read_file, string a_write_file)
NAME

    FileAccess::Write - writes the data to the specified file

SYNOPSIS

   void FileAccess::Write(string a_read_file, string a_write_file);
   a_read_file -> the file that is read in
   a_write_file  -> the file that is written

DESCRIPTION

    This function will read in data from a file and write that data to a file 

RETURNS

    Nothing since it is a void
*/
/**/

void FileAccess::Write(string a_read_file, string a_write_file)
{
    ofstream write;
    ifstream read;
    int counter = 0;
    const int MAX = 5000;
    char word[MAX + 1];
    read.open(a_read_file);
    write.open(a_write_file);
    string data;
    read.getline(word, MAX + 1, '\n');
    while (counter != File_line_count(a_read_file))
    {
        write << word << endl;
        counter++;
        read.getline(word, MAX + 1, '\n');   
    }
    read.close();
    write.close();
}

/**/
/*
FileAccess::Num_in_val_range(string a_read_file, string a_write_file)
NAME

    FileAccess::Num_in_val_range - are the numbers less then 9 digits

SYNOPSIS

   void FileAccess::Num_in_val_range(string a_read_file, string a_write_file);
   a_read_file   -> the file that is read in
   a_write_file  -> the file that is written

DESCRIPTION

    This function will read in data from a file and will write in question marks if necessary
    (ie a numerical value is more then 9 digits) ultizling other member functions 

RETURNS

    Nothing since it is a void
*/
/**/

void FileAccess::Num_in_val_range(string a_read_file, string a_write_file)
{
    ofstream write;
    ifstream read;
    const int MAX = 5000;
    char word[MAX + 1];
    string number;
    int counter = 0;
	// open the two files
    read.open(a_read_file);
    write.open(a_write_file);
	// read in the data
    read.getline(word, MAX + 1, '\n');
	// I used strings since strings have more built in functionality (set the strings equal to cstrings)
    string original;
    string edited;
    while (counter != File_line_count(a_read_file))
    {
        number = word;
	// original is the string that is unedited from single reg function
        original = word;
        edited = word;
        int size = int(number.size());
	// check if its a multiple numbers and if there in range if they aren't change them to question marks
        original = Double_reg(number);
	// remove spaces and extra characters
        number.erase(remove(number.begin(), number.end(), ' '), number.end());
        for (char i = ' '; i < 'z'; i++)
        {
	// skip numbers
            if (i >= '0' && i <= '9')
            {
                i++;
            }
    // remove other characters
            else
            {
                number.erase(remove(number.begin(), number.end(), i), number.end());
            }
        }
    // if the numerical value is more then 9 digits then write in the question marks
        if (number.size() > 9)
        {
            edited.erase(edited.find(number), number.size());
            Single_reg_num(edited, original, int(original.find(number)));
            write << edited << endl;
            read.getline(word, MAX + 1, '\n');
        }
        else
        {
            write << original << endl;
            read.getline(word, MAX + 1, '\n');
        }
        counter++;
    }
    read.close();
    write.close();
}

/**/
/*
FileAccess::File_num_check()
NAME

    FileAccess::File_num_check - execute the file checking procedures

SYNOPSIS

   void FileAccess::File_num_check();

DESCRIPTION

    This function will execute the file checking procedures (writing unedited file data to a tmp checking number 
	range and writing the error checking file to the main test.txt file
    so that the the assembler constructor can easily and elegantly 
    check for greater then 9 digit numbers.

RETURNS

    Nothing since it is a void
*/
/**/

void FileAccess::File_num_check()
{
    Write(MAIN_FILE, NO_ERROR_CHECK);
    Num_in_val_range(MAIN_FILE, ERROR_CHECK);
    Write(ERROR_CHECK, MAIN_FILE);
}

/**/
/*
FileAccess::File_line_count(string a_read_file)
NAME

    FileAccess::File_line_count - how many lines are present

SYNOPSIS

   void FileAccess::File_line_count(string a_read_file);
   a_read_file -> the file that is read in

DESCRIPTION

    This function will read in data from a file and each line that 
    is read in it will increment a counter

RETURNS

    the number of lines that are read from the file
*/
/**/

int FileAccess::File_line_count(string a_read_file)
{
    int counter = 0;
    ifstream read;
    read.open(a_read_file);
    string data;
	// while we are reading in data increase the counter
    while (getline(read,data))
    {
        counter++;
    }
    return counter;
    read.close();
}

/**/
/*
FileAccess::File_error_reset()
NAME

    FileAccess::File_error_reset - execute the file reset process

SYNOPSIS

   void FileAccess::File_error_reset();

DESCRIPTION

    This function will reset the command line file 

RETURNS

    Nothing since it is a void
*/
/**/

void FileAccess::File_error_reset()
{
	// The remove command requires a const char data type so we declare them and have them point to the const file name strings
	// converted to cstrings
    const char*ALTERED = ERROR_CHECK.c_str();
    const char* UN_ALTERED = NO_ERROR_CHECK.c_str();
    Write(NO_ERROR_CHECK, MAIN_FILE);
	// Time to take out the trash (delete the created tmp files
    remove(ALTERED);
    remove(UN_ALTERED);
}
