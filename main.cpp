//
// Created by adrien on 19/03/23.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <fstream>
#include <string>

#define SAVE_FILE_NAME "monk_save.txt"

struct Monk {
    std::string name;
    int sanity = 0;
} monk;

void character_creation() {
    std::cout << "A new monk has a arrived at the monastery..." << std::endl;
    std::cout << "His name is Brother ";
    std::getline(std::cin, monk.name);
}

void save() {
    std::ofstream outFile(SAVE_FILE_NAME);
    if (outFile.is_open()) {
        outFile << monk.name << std::endl;
        outFile.close();
    } else {
        std::cerr << "Saving Failed!" << std::endl;
    }
}

int main() {
    // Load Save
    std::ifstream inFile(SAVE_FILE_NAME);
    if (inFile.is_open()) {
        inFile >> monk.name;
    } else {
        character_creation();
        save();
    }

    // Main Loop
    system("clear");
    std::time_t now;
    unsigned int save_counter = 0;
    while (true) {
        system("clear");
        now = std::time(nullptr);

        char* timeStr = std::ctime(&now);

        std::cout << "Time: " << timeStr;
        std::cout << "Monk Name: " << monk.name << std::endl;

        save_counter++;
        if (save_counter >= 5) {
            save_counter = 0;
            save();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}