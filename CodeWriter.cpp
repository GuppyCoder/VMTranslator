//
// Created by Brian Chaidez on 3/15/22.
//

#include "CodeWriter.h"
#include <algorithm>
CodeWriter::CodeWriter(std::ofstream &fileOut, std::string& fileName) {
    ChangeFileName(fileName);
    fileOut.open(fileName);
}

void CodeWriter::ChangeFileName(std::string &fileName) {
    fileName.append(".asm");
}

void CodeWriter::writeArithmetic(std::string &command) {
    // pop 2 items off the stack
    static int i = 0;
    std::string label = std::to_string(i);
    label.insert(0, "(LABEL");
    label.append(")");

    std::string TRUE = std::to_string(i);
    TRUE.insert(0, "TRUE");

    std::string FALSE = std::to_string(i);
    FALSE.insert(0, "FALSE");

    std::string assembly;
    size_t pos;
    if((pos = command.find("//")) != std::string::npos)
        command.erase(pos+1, std::string::npos);
    command.erase(remove_if(command.begin(), command.end(), isspace), command.end());
    // the following commands need 2 operands
    if(command == "add" || command == "sub" || command == "eq" || command == "gt" || command == "lt" || command == "and" || command == "or") {
        assembly = "@SP\n"     // go to stack pointer, register 0
                   "AM=M-1\n"  // A = value at register 0
                   "D=M\n"     // D = value at register A.
                   "@SP\n"     // go back to register 0 to decrement pointer
                   "AM=M-1\n"; // two items have been popped off the stack
    }else{  // if only one operand is needed do this instead
        assembly = "@SP\n"     // go to stack pointer, register 0
                   "AM=M-1\n"  // A = value at register 0
                   "D=M\n";     // D = value at register A.
    }


    if(command == "add")
        assembly.append("D=M+D\n");
    else if(command == "sub")
        assembly.append("D=M-D\n");
    else if(command == "eq"){
        assembly.append("D=M-D\n"
                        "@" + TRUE +"\n"
                        "D;JEQ\n"
                        "@" + FALSE + "\n"
                        "D;JNE\n");
    }
    else if(command == "gt"){
        assembly.append("D=M-D\n"
                        "@" + TRUE +"\n"
                        "D;JGT\n"
                        "@" + FALSE + "\n"
                        "D;JMP\n");
    }
    else if(command == "lt"){
        assembly.append("D=M-D\n"
                        "@" + TRUE +"\n"
                        "D;JLT\n"
                        "@" + FALSE + "\n"
                        "D;JMP\n");
    }else if(command == "and")
        assembly.append("D=D&M\n");
    else if(command == "or")
        assembly.append("D=D|M\n");
    else if(command == "neg")
        assembly.append("D=-D\n");
    else
        assembly.append("D=!D\n");

    // true false labels
    if(command == "eq" || command == "lt" || command == "gt"){
        std::string labelNoParenthesis = label;
        labelNoParenthesis.erase( std::remove_if(labelNoParenthesis.begin(), labelNoParenthesis.end(),
                                                  [](char ch){ return ch=='(' || ch ==')'; }),labelNoParenthesis.end() );
        assembly.append("(" + TRUE +")\n"
                            "@R13\n"
                            "D=M\n"
                            "@" + labelNoParenthesis +"\n"
                            "0;JMP\n"
                            "(" + FALSE + ")\n"
                            "@R14\n"
                            "D=M\n"
                            "@" +labelNoParenthesis+"\n"
                            "0;JMP\n"+
                            label+'\n');
                            ++i;
    }

    std::string pushResult = "@SP\n"
                             "A=M\n"
                             "M=D\n"
                             "@SP\n"
                             "M=M+1";
    assembly.append(pushResult);
    command = assembly;
}

void CodeWriter::WritePushPop(int commandType, std::string &command, int index, std::string& fileName) {
    std::string i = std::to_string(index);
    std::string assembly;
    size_t pos;
    pos = command.find(' ');
    command.erase(0, pos+1);
    command.erase(std::remove(command.begin(), command.end(), ' '), command.end()); // remove spaces
    if(commandType == C_POP){
        std::string segmentASM;
        auto it = segmentsASM.find(command);
        if(it != segmentsASM.end()) {
            segmentASM = it->second;
            assembly.append("@" + i + '\n' +
                            "D=A\n"
                            "@" + segmentASM + '\n' +
                            "D=M+D\n"
                            "@addr\n"
                            "M=D\n"
                            "@SP\n"     // go to stack pointer, register 0
                            "AM=M-1\n"  // A = value at register 0
                            "D=M\n"
                            "@addr\n"
                            "A=M\n"
                            "M=D");
        }else if(command == "static"){
            assembly.append("@SP\n"
                            "AM=M-1\n"
                            "D=M\n"
                            "@"+fileName+i+'\n'+
                            "M=D\n");
        }else if(command == "temp"){
            std::string address = std::to_string(5+index);
            assembly.append("@SP\n"
                            "AM=M-1\n"
                            "D=M\n"
                            "@"+address+'\n'+
                            "M=D\n");
        }else if(command == "pointer" && index == 0){
            assembly.append("@SP\n"
                            "AM=M-1\n"
                            "D=M\n"
                            "@THIS\n"
                            "M=D\n");
        }else if(command == "pointer" && index == 1){
            assembly.append("@SP\n"
                            "AM=M-1\n"
                            "D=M\n"
                            "@THAT\n"
                            "M=D\n");
        }
    }else if(commandType == C_PUSH){
        if(command != "static" || command != "pointer" || command != "temp") {
            assembly = "@" + i + '\n' +
                       "D=A\n";
        }
        std::string segmentASM;
        auto it = segmentsASM.find(command);
        if(it != segmentsASM.end()) {
            segmentASM = it->second;
            assembly.append("@" + segmentASM + '\n' +
                            "D=M+D\n"
                            "A=D\n"
                            "D=M\n"
                            "@SP\n"     // go to stack pointer, register 0
                            "A=M\n"     // A = value at register 0
                            "M=D\n"
                            "@SP\n"
                            "M=M+1\n");
        }
        if(command == "constant") {
            assembly.append("@SP\n"
                            "A=M\n"
                            "M=D\n"
                            "@SP\n"
                            "M=M+1");
        }else if(command == "static"){
            assembly.append('@'+ fileName+i+'\n'+
                            "D=M\n"
                            "@SP\n"
                            "A=M\n"
                            "M=D\n"
                            "@SP\n"
                            "M=M+1\n");
        }else if(command == "temp"){
            std::string address = std::to_string(5+index);
            assembly.append('@'+address+'\n'+
                            "D=M\n"
                            "@SP\n"
                            "A=M\n"
                            "M=D\n"
                            "@SP\n"
                            "M=M+1\n");
        }else if(command == "pointer" && index == 0){
            assembly.append("@THIS\n"
                            "D=M\n"
                            "@SP\n"
                            "A=M\n"
                            "M=D\n"
                            "@SP\n"
                            "M=M+1\n");
        }else if(command == "pointer" && index == 1){
            assembly.append("@THAT\n"
                            "D=M\n"
                            "@SP\n"
                            "A=M\n"
                            "M=D\n"
                            "@SP\n"
                            "M=M+1\n");
        }
    }
    command = assembly;
}

void CodeWriter::writeLabel(std::string &command) {
    // get the first UpperCase letter and delete everything that comes before it
    char labelStart = getFirstUpper(command);
    size_t pos;
    if((pos = command.find(labelStart)) != std::string::npos)
        command.erase(0, pos);

    // Delete all whitespaces that are after the label
    command.erase(remove_if(command.begin(), command.end(), isspace), command.end());
    std::string assembly = '(' + command + ")";
    command = assembly;
}

void CodeWriter::writeGoto(std::string &command) {
    size_t pos;
    // Delete all whitespaces that are after the label
    command.erase(remove_if(command.begin(), command.end(), isspace), command.end());

    // erase from "//" till end of string
    if((pos = command.find("//"))!= std::string::npos)
        command.erase(pos, std::string::npos);


    // Delete all the characters or whitespace characters that come before the label.
    char labelStart = getFirstUpper(command);
    if((pos = command.find(labelStart)) != std::string::npos)
        command.erase(0, pos);

    std::string assembly = '@' + command+'\n'+
                           "0;JMP";
    command = assembly;
}

 void CodeWriter::writeIf(std::string &command) {
    size_t pos;
    // erase from "//" till end of string
    if((pos = command.find("//"))!= std::string::npos)
        command.erase(pos, std::string::npos);

    // get the first UpperCase letter and delete everything that comes before it
    char labelStart = getFirstUpper(command);
    if((pos = command.find(labelStart)) != std::string::npos)
        command.erase(0, pos);

    // Delete all whitespaces that are after the label
    command.erase(remove_if(command.begin(), command.end(), isspace), command.end());

    // Write if-goto Label in assembly
    std::string assembly = "@SP\n"
                           "AM=M-1\n"
                           "D=M\n"
                           "@" + command+"\n"+
                           "D;JNE";

    command = assembly; // return changed command and output to DirName.asm file
}


//function to return first uppercase character from given string
char CodeWriter::getFirstUpper(const std::string& str)
{
    //loop that will check uppercase character
    //from index 0 to str.length()
    for(char i : str)
    {
        //'isupper() function returns true
        //if given character is in uppercase
        if(isupper(i))
            return i;
    }

    return 0;
}

void CodeWriter::writeCall(std::string &functionName, int numArgs) {
    // Get the function name
    functionName.erase(0, 5);
    size_t pos = functionName.find(' ');
    if(pos != std::string::npos)
        functionName.erase(functionName.find(' '), 1);

    // push return address
    static int k = 0;
    std::string returnAddress = "RETURN"+std::to_string(k);
    ++k;
    std::string assembly = "// push return address\n@" + returnAddress +'\n'+
                           "D=A\n"
                           "@SP\n"
                           "A=M\n"
                           "M=D\n"
                           "@SP\n"
                           "M=M+1\n";

    // push LCL
    assembly.append("// push LCL\n@LCL\n"
                       "D=M\n"
                       "@SP\n"
                       "A=M\n"
                       "M=D\n"
                       "@SP\n"
                       "M=M+1\n");

    // push ARG
    assembly.append("// push ARG\n@ARG\n"
                        "D=M\n"
                        "@SP\n"
                        "A=M\n"
                        "M=D\n"
                        "@SP\n"
                        "M=M+1\n");

    // push THIS
    assembly.append("// push THIS\n@THIS\n"
                        "D=M\n"
                        "@SP\n"
                        "A=M\n"
                        "M=D\n"
                        "@SP\n"
                        "M=M+1\n");

    // push THAT
    assembly.append("// push THAT\n@THAT\n"
                        "D=M\n"
                        "@SP\n"
                        "A=M\n"
                        "M=D\n"
                        "@SP\n"
                        "M=M+1\n");

    // ARG = SP-n-5; (Reposition ARG)
    if(functionName != "Sys.init") {
        assembly.append("// ARG = SP-n-5; (Reposition ARG)\n@5\n"
                        "D=A\n"
                        "@" + std::to_string(numArgs) + '\n' +
                        "D=D+A\n"
                        "@SP\n"
                        "D=M-D\n"
                        "@ARG\n"
                        "M=D\n");



        // Reposition LCL
        assembly.append("// Reposition LCL\n@SP\n"
                        "D=M\n"
                        "@LCL\n"
                        "M=D\n");
    }

    // transfer control
    std::string functionGoTo = functionName;
    writeGoto(functionGoTo);
    assembly.append("// transfer control\n"+functionGoTo+'\n');

    // Declare a label for the return address
    std::string label = '('+ returnAddress + ')';
    assembly.append("// Declare a label for the return address\n"+label+'\n');
    functionName = assembly;
}

void CodeWriter::writeFunction(std::string &functionName, int numLocal) {
    functionName.erase(0,9);
    functionName.erase(functionName.find(' '), 1);

    // Declare a label for function entry
    std::string assembly = "// Declare a label for function entry\n";
    std::string labelFunction = functionName; writeLabel(labelFunction);
    assembly.append(labelFunction+'\n');

    // k = number of local variables
    for(int i = 0; i < numLocal; ++i){
        assembly.append("@0\n"
                        "D=A\n"
                        "@SP\n"
                        "A=M\n"
                        "M=D\n"
                        "@SP\n"
                        "M=M+1\n");
    }
    functionName = assembly;

}

std::string CodeWriter::writeReturn() {
    std::string assembly = "// FRAME is a temporary variable\n"
                           "@LCL\n"
                           "D=M\n"
                           "@FRAME\n"
                           "M=D\n"
                           "// Put the return-address in a temp var\n"
                           "@5\n"
                           "D=A\n"
                           "@FRAME\n"
                           "D=M-D\n"
                           "A=D\n"
                           "D=M\n"
                           "@RET\n"
                           "M=D\n"
                           "// Reposition the return value for the caller\n"
                           "@SP\n"
                           "AM=M-1\n"
                           "D=M\n"
                           "@ARG\n"
                           "A=M\n"
                           "M=D\n"
                           "// Restore SP of the caller\n"
                           "@ARG\n"
                           "D=M+1\n"
                           "@SP\n"
                           "M=D\n"
                           "// Restore THAT of the caller\n"
                           "@FRAME\n"
                           "A=M-1\n"
                           "D=M\n"
                           "@THAT\n"
                           "M=D\n"
                           "// Restore THIS of the caller\n"
                           "@2\n"
                           "D=A\n"
                           "@FRAME\n"
                           "D=M-D\n"
                           "A=D\n"
                           "D=M\n"
                           "@THIS\n"
                           "M=D\n"
                           "// Restore ARG of the caller\n"
                           "@3\n"
                           "D=A\n"
                           "@FRAME\n"
                           "D=M-D\n"
                           "A=D\n"
                           "D=M\n"
                           "@ARG\n"
                           "M=D\n"
                           "// Restore LCL of the caller\n"
                           "@4\n"
                           "D=A\n"
                           "@FRAME\n"
                           "D=M-D\n"
                           "A=D\n"
                           "D=M\n"
                           "@LCL\n"
                           "M=D\n"
                           "// Goto return-address\n"
                           "@RET\n"
                           "A=M\n"
                           "0;JMP\n";
    return assembly;
}

std::string CodeWriter::writeInit() {
    std::string bootStrapCode = "@256\n"
                                "D=A\n"
                                "@SP\n"
                                "M=D\n"
                                "// call Sys.init\n";
    std::string callSysInit = "call Sys.init";
    writeCall(callSysInit, 0);
    bootStrapCode.append(callSysInit);
    return bootStrapCode;
}



