#include "iolibrtTiffExample.cpp"
int main(int argc, char* argv[], char* envp[]){
    if(argc < 2){
        std::cout << "Please start this example with a path to a "
        "target tiff file." << std::endl;
        std::cout << "iolibrtTiffExample.exe [config file path]" 
            <<std::endl;
        return -1;
    }

    iolibrtTiffExample example;
    return example.run(argv[1]);

}
