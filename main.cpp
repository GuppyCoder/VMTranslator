#include <iostream>
#include <vector>
#include <dirent.h>
#include "Parser.h"
#include "CodeWriter.h"


int main() {
    DIR* dir; struct dirent* diread;
    std::vector<std::ifstream*> VMFiles;
    const char* workingDirectory = "/Users/BrianChaidez/Desktop/nand2tetris/projects/07/StackArithmetic/StackTest";
    std::string path = workingDirectory;
    std::string DirName = path.substr(path.find_last_of('/')+1, std::string::npos);
    std::vector<std::string> fileNameVector;
    std::map<std::ifstream*, std::string> stream_file_table;
    if((dir = opendir(workingDirectory)) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            std::string fileName = diread->d_name;
            fileNameVector.push_back(fileName);
            if (fileName.find(".vm") != std::string::npos) {
                std::ifstream *f = new std::ifstream();
                f->open(fileName);      // if it is already open why do we have to do this again?
                if (f->is_open())
                    std::cout << "Good." << std::endl;
                else
                    std::cout << "Uh oh." << std::endl;
                VMFiles.push_back(f);
                stream_file_table.insert({f, fileName});
            }
        }
    }
    std::ofstream fileOut;
    CodeWriter output(fileOut, DirName);
    if(std::find(fileNameVector.begin(), fileNameVector.end(), "Sys.vm") != fileNameVector.end()) {
        /* v contains x */
        fileOut << "@R13\nM=-1\n@R14\nM=0\n" + output.writeInit() << std::endl;    // boot strap code + @R13 if true, @R14 if false
    } else {
        /* v does not contain x */
        fileOut << "@R13\nM=-1\n@R14\nM=0\n" << std::endl;    // boot strap code + @R13 if true, @R14 if false
    }
    for(auto input: VMFiles){
        // get the VM file name from the map <ifstream, string>
        auto itr = stream_file_table.find(input);
        std::string vmFileName = itr->second;

        // remove vm extension
        vmFileName = vmFileName.substr(0, vmFileName.find('.') + 1);

        // do some stuff
        Parser parser;
        while (std::getline(*input, parser.advance)) {
            size_t pos = 0;
            if((pos = parser.advance.find("/")) != std::string::npos)
                parser.advance.erase(pos, std::string::npos);
            if (!isalpha(parser.advance[0]))
                continue;
            int commandType = parser.getCommandType(parser.advance);
            std::string comment = "// ";
            comment.append(parser.advance);
            //comment.erase(remove_if(comment.begin(), comment.end(), isspace), comment.end());
            fileOut << comment << std::endl;
            if (commandType != C_RETURN) {
                parser.arg1 = parser.getArg1(parser.advance);
                if (commandType == C_ARITHMETIC) {
                    output.writeArithmetic(parser.arg1);
                    fileOut << parser.arg1 << std::endl;
                }
                if (commandType == C_PUSH || commandType == C_POP) {
                    parser.arg2 = parser.getArg2(parser.advance);
                    output.WritePushPop(commandType, parser.arg1, parser.arg2, vmFileName);
                    fileOut << parser.arg1 << std::endl;
                } else if (commandType == C_LABEL) {
                    output.writeLabel(parser.arg1);
                    fileOut << parser.arg1 << std::endl;
                } else if (commandType == C_GOTO) {
                    output.writeGoto(parser.arg1);
                    fileOut << parser.arg1 << std::endl;
                } else if (commandType == C_IF) {
                    output.writeIf(parser.arg1);
                    fileOut << parser.arg1 << std::endl;
                } else if(commandType == C_CALL){
                    parser.arg2 = parser.getArg2(parser.advance);
                    output.writeCall(parser.arg1, parser.arg2);
                    fileOut << parser.arg1 << std::endl;
                } else if(commandType == C_FUNCTION){
                    parser.arg2 = parser.getArg2(parser.advance);
                    output.writeFunction(parser.arg1, parser.arg2);
                    fileOut << parser.arg1 << std::endl;
                }
            }else
                fileOut << output.writeReturn() << std::endl;
        }
    }

    for(auto input: VMFiles)
        delete input;
    fileOut.clear();
    fileOut.close();
    return 0;
}

