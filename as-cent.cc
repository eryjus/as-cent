//===============================================================================================
// as-cent.cc -- This file contains the main function and additional smaller functions.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-30  ADCL  Initial  This is the first version
//
//===============================================================================================

#include "in-file.h"
#include "parser.h"
#include "symtab.h"

#include <iostream>
#include <iomanip>

//-----------------------------------------------------------------------------------------------
// main() is the main entry point.  It will evolve over time as more compnents of the assembler
// are developed.
//-----------------------------------------------------------------------------------------------
int main(void)
{
    std::string file = "test.asm";
    std::string line;
    int num = 0;

    InputFile::Factory(file);
    Parser parser;

    try {
        while (1) {
            line = InputFile::NextLine();
            std::cout << std::setw(5) << (++ num) << ": " << line << std::endl;

            parser.ParseLine(line);
            std::cout << "\t Clean Line: <" << parser.Line() << ">\n";
            std::cout << std::endl;
        }
    } catch(...) {
    }

    return 0;
}

//===============================================================================================
