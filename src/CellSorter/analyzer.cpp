//
// Created by Eskild Børsting Sørensen on 08/03/2018.
//

#include "analyzer.h"

void analyzer::makeList(std::vector<IMG_PROCESS_TYPES> processes) {
    // Takes input from string and converts it into vector used for class.'
    for(const auto& command : processes){ // Goes through all processes
        switch(command){
            case IMG_PROCESS_TYPES::PRESET {
                setupRBCPreset();
            }
        }
    }


    for (int i = 0; i < sizeof(commands); i++) {  // CHANGE TO ENUM CLASS
        if (commands[i].compare("SUB_BG")) {      // SUB_BG: Adds all required operations for subtracting background.
            list.push_back("abs_diff");
            list.push_back("make_binary");
            list.push_back("remove_noise");
            list.push_back("fill_cell");
        } else {  // If no pre-defined functions are chosen, the single function is added to vector
            list.push_back(commands[i]);
        }
    }
    std::reverse(list.begin(), list.end());  // Reverses list to do the functions in correct order.
}

void analyzer::runProcesses(){

    for(const auto& process : m_processes){
        process.doProcessing(m_img);
    }


    while (!list.empty()) {
        switch(list.pop_back()){
            case "abs_diff":
                cv::absdiff(props.im, props.bg, diff);  // Outputs absolute difference into 'diff'

        }
    }
};

void analyzer::setupRBCPreset() { // Default preset with values
    // Setup background subtraction
    // (...)
}