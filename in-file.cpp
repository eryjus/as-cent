//===============================================================================================
// in-file.cpp -- This file contains the class implementations for managing input files.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-29  ADCL  Initial  This is the first version
//
//===============================================================================================

#include "in-file.hpp"
#include "parser.hpp"
#include "utils.hpp"

#include <iostream>
#include <cstdlib>

//-----------------------------------------------------------------------------------------------
// InputFile::InputFile(const std::string&) is the standard consutructor for the InputFile class.
// It is also responsible for pushing itself onto the stack as the current open file.
//-----------------------------------------------------------------------------------------------
InputFile::InputFile(const std::string &name) : file(NULL), next(NULL), fileName(name),
        lineNum(0)
{
    if (name != "") {
        file = new std::ifstream();
        file->exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file->open(name.c_str());
    }
}

//-----------------------------------------------------------------------------------------------
// ~InputFile::InputFile() is used to clean up an InputFile.  Remember that next is a pointer to
// an instance that will persist after this instance is cleaned up, so we will not delete it.
//-----------------------------------------------------------------------------------------------
InputFile::~InputFile()
{
    if (file) {
        file->close();
        delete file;
    }
}

//-----------------------------------------------------------------------------------------------
// InputFile::ReadLine is the worker function to read a line from the associated file.  eof()
// must be checked before calling this function for the stack to work.
//-----------------------------------------------------------------------------------------------
std::string InputFile::ReadLine(void)
{
    std::string line;

    if (file) {
        if (getline(*file, line)) {
            lineNum ++;
            line = rtrim(line, "\x1a");
        }
    } else {
        if (getline(std::cin, line)) {
            lineNum ++;
            line = rtrim(line, "\x1a");
        }
    }

    return line;
}

//===============================================================================================
