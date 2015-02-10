//===============================================================================================
// in-file.hpp -- This file contains the class definition for managing input files.
//
// Eventually, I will need to be able to support nested files.  It it my hope that when I come
// to a directive that includes a file, I will be able to create a new instance of this class
// and then push the "included" file onto a stack of input files.  When a file closes, it's just
// a matter of poping the instance off the stack and resetting a pointer.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-29  ADCL  Initial  This is the first version
// 2015-02-09  ADCL   #264    Removed static members and created an independent file class
// 2015-02-09  ADCL   #261    Removed the EOFException from this file
// 2015-02-09  ADCL   #258    Removed static members
// 2015-02-09  ADCL   #257    Renamed this file to in-file.hpp
//
//===============================================================================================

#ifndef __IN_FILE_HPP__
#define __IN_FILE_HPP__

#ifndef __cplusplus
#error The file 'parser.hpp' is not being compiled by a C++ compiler.
#endif

#include <fstream>
#include <string>

//-----------------------------------------------------------------------------------------------
// The InputFile class is responsbile for reading a line from a file and managing its location.
//-----------------------------------------------------------------------------------------------
class InputFile
{
public:
    InputFile(const std::string &name);
    virtual ~InputFile();

public:
    std::string ReadLine(void);

public:
    std::string FileName(void) const { return fileName; };
    long LineNum(void) const { return lineNum; };
    bool IsEOF(void) const { return file->eof(); };
    InputFile *Next(void) const { return next; };
    void SetNext(InputFile *n) { next = n; };

private:
    std::ifstream *file;
    InputFile *next;
    std::string fileName;
    long lineNum;
};

//===============================================================================================

#endif
