//===============================================================================================
// in-file.h -- This file contains the class definition for managing input files.
//
// Eventually, I will need to be able to support nested files.  It it my hope that when I come
// to a directive that includes a file, I will be able to create a new instance of this class
// and then push the "included" file onto a stack of input files.  When a file closes, it's just
// a matter of poping the instance off the stack and resetting a pointer.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-29  ADCL  Initial  This is the first version
//
//===============================================================================================

#ifndef __IN_FILE_H__
#define __IN_FILE_H__

#include <fstream>
#include <string>

class InputFile
{
private:
    static InputFile *fileStack;
    std::ifstream file;
    InputFile *next;
    std::string fileName;
    long lineNum;

private:
    InputFile(std::string name);

public:
    static InputFile &Factory(std::string name) { return *(new InputFile(name)); };
    virtual ~InputFile() { file.close(); fileStack = this->next; delete this; };

protected:
    std::string ReadLine(void);

public:
    static std::string NextLine(void);
    static std::string FileName(void) { return (fileStack?fileStack->fileName:""); };
    static long LineNum(void) { return (fileStack?fileStack->lineNum:0); };
};

class EOFException : public std::ifstream::failure {
public:
    EOFException() : std::ifstream::failure("EOF") {};
};

//===============================================================================================

#endif
