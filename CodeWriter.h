//
// Created by Brian Chaidez on 3/15/22.
//

#ifndef VMTRANSLATOR_CODEWRITER_H
#define VMTRANSLATOR_CODEWRITER_H
#include <fstream>
#include <string>
#include <map>
#include "Parser.h"
class CodeWriter {
public:
    CodeWriter(std::ofstream& fileOut, std::string& fileName);
    static void ChangeFileName(std::string& fileName);
    static void writeArithmetic(std::string& command);
    void WritePushPop(int commandType, std::string& command, int index, std::string& fileName);

    std::map<std::string, std::string> segmentsASM = {{"local", "LCL"},{"argument", "ARG"},{"this","THIS"},{"that","THAT"}};
    std::string writeInit();
    static void writeLabel(std::string& command);
    static void writeGoto(std::string& command);
    static void writeIf(std::string& command);
    static char getFirstUpper(const std::string& str);
    void writeCall(std::string& functionName, int numArgs);
    static void writeFunction(std::string& functionName, int numLocal);
    std::string writeReturn();
};


#endif //VMTRANSLATOR_CODEWRITER_H
