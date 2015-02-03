//===============================================================================================
// in-file.c -- This file contains the class implementations for managing input files.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-29  ADCL  Initial  This is the first version
//
//===============================================================================================

#include "in-file.h"
#include "parser.h"

#include <iostream>
#include <cstdlib>

//-----------------------------------------------------------------------------------------------
// InputFile::fileStack is a stack implementation.  It is a static member of InputFile and will
// be used to access the current file (i.e. read a line from the current file).  When fileStack
// is NULL, there are no open files from which to read.  We need to initialize it to NULL.
//-----------------------------------------------------------------------------------------------
InputFile *InputFile::fileStack = NULL;

//-----------------------------------------------------------------------------------------------
// InputFile::InputFile(std::string) is the standard consutructor for the InputFile class.  It is
// also responsible for pushing itself onto the stack as the current open file.
//-----------------------------------------------------------------------------------------------
InputFile::InputFile(std::string name) : next(NULL), fileName(name), lineNum(0)
{
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        file.open(name.c_str());
        next = fileStack;
        fileStack = this;
    } catch(std::ifstream::failure e) {
        std::cerr << "ERROR: Unable to open file " << name << " (" << e.what() << ")" << std::endl;
        exit(1);
    }
}

//-----------------------------------------------------------------------------------------------
// InputFile::NextLine is the interface to reading a line of code from the input file.  When
// there are nested files, it will make sure that the deepest file is read.  This function is a
// static member function, meaning that it can be called without an instance.
//-----------------------------------------------------------------------------------------------
std::string InputFile::NextLine(void)
{
    if (fileStack) {
        try {
            return fileStack->ReadLine();
        } catch(std::ifstream::failure e) {
            if (e.what() == std::string("EOF")) {
                fileStack->~InputFile();
                return NextLine();
            } else throw e;
        }
    } else throw new std::ifstream::failure("No open file");
}

//-----------------------------------------------------------------------------------------------
// InputFile::READLine is the worker function to read a line from the associated file.  In the
// even an end of file is reached, the function throws an EOF exception.
//-----------------------------------------------------------------------------------------------
std::string InputFile::ReadLine(void)
{
    std::string line;

    if (getline(file, line)) {
        lineNum ++;
        line = rtrim(line, "\x1a");
        if (line == "") throw new EOFException();
    } else throw new EOFException();

    return line;
}

//===============================================================================================
