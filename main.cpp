//
// Created by adrien on 19/03/23.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <fstream>
#include <string>
#include <ncurses.h>
#include <pthread.h>

#define SAVE_FILE_NAME "monk_save.txt"

struct Vice {
    std::string name;
    unsigned int damage;
};

struct Monk {
    std::string name;
    std::time_t birthday = std::time(nullptr);
} monk;

void character_creation() {
    std::cout << "A new monk has a arrived at the monastery..." << std::endl;
    std::cout << "His name is Brother ";
    std::getline(std::cin, monk.name);
    time(&monk.birthday);
}

void save() {
    std::ofstream outFile(SAVE_FILE_NAME);
    if (outFile.is_open()) {
        outFile << monk.name << std::endl;
        outFile << monk.birthday << std::endl;
        outFile.close();
    } else {
        std::cerr << "Saving Failed!" << std::endl;
    }
}

void draw_screen() {
    clear();

    time_t now;
    time(&now);
    char* timeStr = std::ctime(&now);

    printw("Time: %s", timeStr);
    printw("Monk Name: %s\n", monk.name.c_str());
    double seconds = difftime(now, monk.birthday);
    printw("Sanity: %g.0\n", seconds);

    refresh();
}

void* input_thread_func(void* arg){
    initscr(); // Initialize ncurses
    noecho(); // Disable automatic echoing of input characters
    keypad(stdscr, true); // Enable function keys (e.g. arrow keys)

    char c;
    while ((c = getchar()) != EOF) {
        if (c == 'q') {
            save();
            endwin();
            exit(0);
        }

        draw_screen();
    }

    return NULL;
}

void* game_thread_func(void* arg){
    // Main Loop
    unsigned int save_counter = 0;

    while (true) {
        draw_screen();

        save_counter++;
        if (save_counter >= 5) {
            save_counter = 0;
            save();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return NULL;
}

int main() {
    // Load Save
    std::ifstream inFile(SAVE_FILE_NAME);
    if (inFile.is_open()) {
        inFile >> monk.name;
        inFile >> monk.birthday;
    } else {
        character_creation();
        save();
    }

    // Setting up threads
    pthread_t input_thread;
    pthread_create(&input_thread, NULL, input_thread_func, nullptr);

    pthread_t game_thread;
    pthread_create(&game_thread, NULL, game_thread_func, nullptr);

    pthread_join(input_thread, NULL);
    pthread_join(game_thread, NULL);

    std::cout << "penis" << std::endl;

    return 0;
}