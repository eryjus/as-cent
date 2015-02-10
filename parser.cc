//===============================================================================================
// parser.cc -- This file contains the class implementation for parsing a line of assembly code.
//
// So, Since there are several different flavors of operands, the operand part of the code line
// will be considered as a single element.  Depending on the OpCode or directive, the operand
// will be parsed using a context-sensitive parsing function -- specific to the type of operands
// expected.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-30  ADCL  Initial  This is the first version
//
//===============================================================================================

#include "parser.hpp"
#include "utils.hpp"
#include <iostream>

//-----------------------------------------------------------------------------------------------
// Parser::ParseLine(std::string) is likely to be one of the most complicated functions in the
// assembler.  It is responsible for parsing out a line into one of its 6 possible components.
//-----------------------------------------------------------------------------------------------
void Parser::ParseLine(std::string line)
{
#if 0
    try {
        size_t position = 0;

        // -- first, assume we have a blank line
        label = opCode = operand = comment = "";
        if (line == "") return;

        // -- strip the terminating '\n' character
        line = rtrim(line, "\n\r");
        if (line == "") return;

        // -- OK, the easiest thing to do is to identify and strip off any comment
        position = line.find(';');
        if (position != (size_t)-1) {
            comment = line.substr(position);
            line = line.substr(0, position);
        }

        cleanLine = rtrim(line);
        if (line == "") return;

        // -- with comments removed from the equation, if the first position contains whitespace,
        //    then the first thing we have is an OpCode;  Otherwise we have a label.
        if (line.find_first_not_of(" \f\n\r\t\v") == 0) {
            position = line.find_first_of(" \f\n\r\t\v");
            label = line.substr(0, position);
            line = line.substr(position);
        }

        line = ltrim(line);

        if (line == "") return;

        // -- Now, we can look for an opcode  We really should have one at this point in the code
        position = line.find_first_of(" \f\n\r\t\v");
        opCode = line.substr(0, position);
        line = ltrim(line.substr(position));

        if (line == "") return;

        // -- Finally, the rest is the operands
        operand = trim(line);
    } catch (...) {
        return;
    }
#endif
}

//===============================================================================================
