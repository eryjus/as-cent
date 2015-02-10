//===============================================================================================
// filestack.hpp -- This file contains the class definition for managing a stack of input files.
//
// Eventually, I will need to be able to support nested files.  It it my hope that when I come
// to a directive that includes a file, I will push a new Input File onto the stack and all
// references to read the file are pulled from this new file until it reaches its EOF.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-02-09  ADCL  Initial  This is the first version.  This is completed based on Redmine
//                            #264.
//
//===============================================================================================

#ifndef __FILESTACK_HPP__
#define __FILESTACK_HPP__

#ifndef __cplusplus
#error The file 'parser.hpp' is not being compiled by a C++ compiler.
#endif

#include "in-file.hpp"

#include <string>
#include <fstream>

//-----------------------------------------------------------------------------------------------
// The class FileStack exists to automatically manage and maintain a stack of files.  One asm
// file has the ability to include another through an assembler directive.  When this included
// file reaches its end of file, FileStack needs to automatically close the top file on the
// stack and pop it off the stack.  In the meantime, the original file is not considered to be
// closed until it reaches its own EOF.
//-----------------------------------------------------------------------------------------------
class FileStack {
public:
    FileStack(void) : stack(NULL) {};
    virtual ~FileStack();

public:
    void Push(const std::string &f);
    void Push(std::ifstream &f);
    void Pop(void);

public:
    std::string FileName(void) { return (stack?stack->FileName():""); };
    long LineNum(void) { return (stack?stack->LineNum():0); };

private:
    InputFile *stack;

private:
    std::string ReadLine(void);
};

//===============================================================================================

#endif
