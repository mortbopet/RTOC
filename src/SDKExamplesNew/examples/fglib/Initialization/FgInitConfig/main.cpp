#include "FgInitConfigExample.cpp"

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "Please start this example with a path to a "
        "valid configuration file." << std::endl;
        std::cout << "FgInitConfigExample.exe [config file path]" 
            <<std::endl;
        return -1;
    }

    FgInitConfigExample example;
    return example.run(argv[1]);
}