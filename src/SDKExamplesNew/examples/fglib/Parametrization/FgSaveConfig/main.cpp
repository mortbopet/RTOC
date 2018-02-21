#include "FgSaveConfigExample.cpp"
int main(int argc, char* argv[], char* envp[]){
    if(argc < 2){
        std::cout << "Please start this example with a path to a "
        "target configuration file." << std::endl;
        std::cout << "FgSaveConfigExample.exe [config file path]" 
            <<std::endl;
        return -1;
    }

    FgSaveConfigExample example;
    return example.run(argv[1]);
}
