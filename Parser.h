//
// Created by Brian Chaidez on 3/15/22.
//

#ifndef VMTRANSLATOR_PARSER_H
#define VMTRANSLATOR_PARSER_H

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iterator>
#include "CodeWriter.h"

class Parser {
#define C_ARITHMETIC 0
#define C_PUSH 1
#define C_POP 2
#define C_LABEL 3
#define C_GOTO 4
#define C_IF 5
#define C_FUNCTION 6
#define C_RETURN 7
#define C_CALL 8

public:
    std::string arg1;
    int arg2;
    std::string advance;
    int getCommandType(std::string &command);
    std::vector<std::string> arithmeticCommand{"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};
    std::string getArg1(std::string&);
    int getArg2(std::string&);
    bool hasMoreCommands;
};


#endif //VMTRANSLATOR_PARSER_H
