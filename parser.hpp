//===============================================================================================
// parser.hpp -- This file contains the class definition for parsing a line of assembly code.
//
// Theoretically, parsing a line is simple -- it is generally in the form:
//
// [label `:`] [OpCode [Operand1 [`,` Operand2 [`,` Operand3]]]] [`;` Comment]
//
// Though the colon after the label is not necessary for most assemblers, I will require it.
// Note also that the termination of a comment is the end of the line.  Comments do not span
// multiple lines.  However, multiple lines can have comments.
//
// Labels MUST begin in the first position of the line.  Comments and directives can also begin
// in the first position (I am not yet working on directives).  However, if the first position
// is whitespace, there cannot be a label on that line.
//
//    Date     PGMR  Tracker  Description
// ----------  ----  -------  -------------------------------------------------------------------
// 2015-01-30  ADCL  Initial  This is the first version
// 2015-02-09  ADCL   #262    The Parser class is misnamed.  It needs to be modified to operate
//                            as a parser really does -- reading and processing the while file
//                            in a single method call.
// 2015-02-09  ADCL   #263    The functions ltrim(), rtrim(), and trim () are moving to their
//                            own file -- utils.hpp.
// 2015-02-09  ADCL   #257    This file is renamed to parser.hpp.
//
//===============================================================================================

#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#ifndef __cplusplus
#error The file 'parser.hpp' is not being compiled by a C++ compiler.
#endif

#include "filestack.hpp"

#include <string>

//-----------------------------------------------------------------------------------------------
// The Parser class is setup to take a file and parse it into its individual lines.  The
// constructor receives as a parameter the name of the file to parse; defaulting to "" if no file
// is specified.  It then sets up it file stack and opens the initial file.
//
// The function Parser::Parse() is then called to do the work of parsing.
//-----------------------------------------------------------------------------------------------
class Parser {
public:
    Parser(const std::string &f = "");
    virtual ~Parser() {};

public:
    void Parse(void);                       // eventually will return the AST

private:
    FileStack &fStack;

private:
    void ParseLine(std::string line);
};

//===============================================================================================

#endif
