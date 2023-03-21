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

WINDOW* info_win;
WINDOW* monk_win;
WINDOW* vices_win;

const char* monk_img = "         ^?YYY5PP5?       \n"
                       "        ^BP#5YYPP5&J      \n"
                       "        .#B5YYYPGG#&?     \n"
                       "         !@BYYYGBB&#&Y.   \n"
                       "         7@@&GPPG#&#&#P:  \n"
                       "        ?@&&&#B&#GP55BGG: \n"
                       "        J@#&@&GPYY5YJY5#J \n"
                       "      .J##G#BPYY5YGBBP5PB5\n"
                       "     ^BBGPPB5YY5GG&@&GPBG!\n"
                       "     :##BG#BB#&@@@@G5YYP: \n"
                       "    ~GBBBB#&#&@@#BBG5YYB: \n"
                       "   :#&BBYYBP55B#5YYYYYB#. \n"
                       "   :###PYYGBB#PB5YY5PB#7  \n"
                       "    G&G5PY55G#GGBPPG&G.   \n"
                       "    ^5GGBBBBBGGBB#&&@&!   \n"
                       "      .^&##&&@@&&&&&#&#^  \n"
                       "       7&PGG&&&GBG&##@#~  \n"
                       "       5BPGP#&&GGG&B#&#7  \n"
                       "      .#GGGGB&P5PG#G&BBP  \n"
                       "      !@GGGGBGYY5PGB@GG#. \n"
                       "      G@55GP#GYYY5G&@P5@~ \n"
                       "     :&@5YY5##YYYP#@BYP@? \n"
                       "     ~@@PYY5&#YYYB@&5YP@Y \n"
                       "     7@@PYYP&#5Y5@&GPYP@5 \n"
                       "     5@#YYYPB#5YB#PGGPP@P \n"
                       "     B@GYYYPGGPY5YYYGGB@B \n"
                       "    :&@PYYYP#GPYYYYY5GB@#.\n"
                       "    7@@PYYYP&BG5YYYYYGB@&:\n"
                       "   ^&@@PYYYG@#GPYYYYYGB@@~\n"
                       "  ^#@@@PYYYG@&GGG5Y5PG#@@!\n"
                       "  JB@@@#5YYG&@&GGGGGGG&@@J\n"
                       "    YB#BB#BB&@@@&&&&&&@Y~^\n"
                       "      . ?#&&@@@&&&##BGP:  ";

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
    time_t now;
    time(&now);
    char* timeStr = std::ctime(&now);

    int row = 1;
    int col = 1;

    // Non Window Stuff
    mvprintw(1, 2, "Monk Mode v0.0");

    refresh();

    // Info Window
    mvwprintw(info_win, row++, col, "Time: %s", timeStr);
    mvwprintw(info_win, row++, col, "Monk Name: %s\n", monk.name.c_str());
    double sanity = difftime(now, monk.birthday);
    mvwprintw(info_win, row++, col, "Sanity: %g\n", sanity);
    box(info_win, 0, 0);
    mvwprintw(info_win, 0, 2, "INFO");

    wrefresh(info_win);

    // Vices Window
    row = 1;
    int vice_num = 1;
    for (Vice v : monk.vices) {
        mvwprintw(vices_win, row++, col, "[%d] %s - %ddmg\n", vice_num++, v.name.c_str(), v.damage);
    }
    box(vices_win, 0, 0);
    mvwprintw(vices_win, 0, 2, "VICES");
    wrefresh(vices_win);

    // Monk Window
    mvwprintw(monk_win, 1, 0, monk_img);
    box(monk_win, 0, 0);
    mvwprintw(monk_win, 0, 2, "MONK");

    wrefresh(monk_win);
}

void* input_thread_func(void* arg){
    initscr(); // Initialize ncurses
    noecho(); // Disable automatic echoing of input characters
    keypad(stdscr, true); // Enable function keys (e.g. arrow keys)

    info_win = newwin(10, 32, 3, 1);
    vices_win = newwin(10, 30, 3, 33);
    monk_win = newwin(35, 30, 3, 63);

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