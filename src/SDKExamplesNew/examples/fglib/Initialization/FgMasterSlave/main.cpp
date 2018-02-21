#include "FgInitMasterSlaveExample.cpp"
int main(int argc, char* argv[], char* envp[]){
    

    bool isSlave = false;
    std::cout << "Is this a slave process? " << std::endl;
    std::cout << "Type 0 for false and !=0 for true" << std::endl;
    std::cin >> isSlave;

    int32_t dmaPort = 0;
    std::cout << "Wich dma port do you want to see?" <<std::endl;
    std::cin >> dmaPort;

    FgInitMasterSlaveExample example;
    example.isSlave(isSlave);
    example.setDmaPort(dmaPort);
    example.run();
    return FG_OK;
}
