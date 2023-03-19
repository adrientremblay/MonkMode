//
// Created by adrien on 19/03/23.
//

#include <iostream>
#include <chrono>
#include <thread>

int main() {
    system("clear");
    int i = 0;
    while (true) {
        system("clear");

        std::cout << ++i << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}