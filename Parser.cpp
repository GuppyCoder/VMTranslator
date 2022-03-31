//
// Created by Brian Chaidez on 3/15/22.
//

#include "Parser.h"

int Parser::getCommandType(std::string& command){
    if(command.find("call") != std::string::npos)
        return C_CALL;
    else if(command.find("function") != std::string::npos)
        return C_FUNCTION;
    for(auto it:arithmeticCommand) {
        if ((command.find(it)) != std::string::npos)
            return C_ARITHMETIC;
    }
    if(command.find("push") != std::string::npos)
        return C_PUSH;
    else if(command.find("pop") != std::string::npos)
        return C_POP;
    else if(command.find("label") != std::string::npos)
        return C_LABEL;
    else if(command.find("if") != std::string::npos)
        return C_IF;
    else if(command.find("goto") != std::string::npos)
        return C_GOTO;
    else if(command.find("return") != std::string::npos)
        return C_RETURN;
}

std::string Parser::getArg1(std::string &command) {
    std::string arg1;
    size_t pos = 0;

    for (auto it = command.cbegin() ; it != command.cend(); ++it) {
        if(isdigit(*it)) {
            size_t pos;
            if(isspace(*(it-1))) {
                arg1 = command.substr(0, std::distance(command.cbegin(), it));
                command.erase(0, std::distance(command.cbegin(), it));
                return arg1;
            }
        }
    }
    return command;
}

int Parser::getArg2(std::string &command) {
    return std::stoi(command);
}

