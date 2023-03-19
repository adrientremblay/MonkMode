//
// Created by adrien on 19/03/23.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

int main() {
    std::time_t now;

    system("clear");
    while (true) {
        system("clear");
        now = std::time(nullptr);

        char* timeStr = std::ctime(&now);

        std::cout << timeStr << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}