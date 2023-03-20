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
#include <vector>
#include <iterator>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#define SAVE_FILE_NAME "monk_save.txt"

struct Vice {
    std::string name;
    unsigned int damage;

    // add serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & name;
        ar & damage;
    }

    Vice(const std::string& name, unsigned int damage) : name(name), damage(damage) {}
    Vice() : name("<DEFAULT NAME>"), damage(0) {}
};

bool operator==(const Vice& lhs, const Vice& rhs) {
    return lhs.name == rhs.name && lhs.damage == rhs.damage;
}


struct Monk {
    Monk() : vices() {};

    std::string name;
    std::time_t birthday = std::time(nullptr);
    std::vector<Vice> vices;
} monk;

void character_creation() {
    std::cout << "A new monk has a arrived at the monastery..." << std::endl;
    std::cout << "His name is Brother ";
    std::getline(std::cin, monk.name);

    std::cout << "What are his vices? (d to stop)" << std::endl;
    for (int i = 0 ; i < 10 ; i++) {
        std::cout << "Vice " << i+1 << ": ";

        std::string vice_name;
        std::getline(std::cin, vice_name);
        if (vice_name == "quit")
            break;

        std::cout << "Vice Damage: ";
        std::string vice_dmg_str;
        std::getline(std::cin, vice_dmg_str);
        unsigned int vice_damage = 0;
        vice_damage = std::stoi(vice_dmg_str);

        monk.vices.push_back(Vice(vice_name, vice_damage));
    }

    time(&monk.birthday);
}

void save() {
    std::ofstream output_file(SAVE_FILE_NAME);
    if (output_file.is_open()) {
        boost::archive::text_oarchive oa(output_file);

        oa << monk.name;
        oa << monk.birthday;
        oa << monk.vices;

        output_file.close();
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
    double sanity = difftime(now, monk.birthday);
    printw("Sanity: %g\n", sanity);

    for (Vice v : monk.vices) {
        printw("Vice Name : %s\n", v.name.c_str());
        printw("Vice Damage : %d\n", v.damage);
    }

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
    std::ifstream input_file(SAVE_FILE_NAME);
    if (input_file.is_open()) {
        boost::archive::text_iarchive ia(input_file);

        ia >> monk.name;
        ia >> monk.birthday;
        ia >> monk.vices;

        input_file.close();
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

    return 0;
}