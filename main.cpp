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

WINDOW* text_win;
WINDOW* monk_win;

const char* monk_img = "       ?5PP5YYY?^        \n"
                       "      J&5PPYY5#PB^       \n"
                       "     ?&#GGPYYY5B#.       \n"
                       "   .Y&#&BBGYYYB@!        \n"
                       "  :P#&#&#GPPG&@@7        \n"
                       " :GGB55PG#&B#&&&@?       \n"
                       " J#5YJY5YYPG&@&#@J       \n"
                       "5BP5PBBGY5YYPB#G##J.     \n"
                       "!GBPG&@&GG5YY5BPPGBB^    \n"
                       " :PYY5G@@@@&#BB#GB##:    \n"
                       " :BYY5GBB#@@&#&#BBBBG~   \n"
                       " .#BYYYYY5##55PBYYB#&#:  \n"
                       "  7#BP5YY5BP#BBGYYP###:  \n"
                       "   .G&GPPBGG#G55YP5G&G   \n"
                       "   !&@&&#BBGGBBBBBGG5^   \n"
                       "  ^#&#&&&&&@@&&##&^.     \n"
                       "  ~#@##&GBG&&&GGP&7      \n"
                       "  7#&#B&GGG&&#PGPB5      \n"
                       "  PBB&G#GP5P&BGGGG#.     \n"
                       " .#GG@BGP5YYGBGGGG@!     \n"
                       " ~@5P@&G5YYYG#PG55@G     \n"
                       " ?@PYB@#PYYY##5YY5@&:    \n"
                       " Y@PY5&@BYYY#&5YYP@@~    \n"
                       " 5@PYPG&@5Y5#&PYYP@@7    \n"
                       " P@PPGGP#BY5#BPYYY#@5    \n"
                       " B@BGGYYY5YPGGPYYYG@B    \n"
                       ".#@BG5YYYYYPG#PYYYP@&:   \n"
                       ":&@BGYYYYY5GB&PYYYP@@7   \n"
                       "~@@BGYYYYYPG#@GYYYP@@&^  \n"
                       "!@@#GP5Y5GGG&@GYYYP@@@#^ \n"
                       "J@@&GGGGGGG&@&GYY5#@@@BJ \n"
                       "^~Y@&&&&&&@@@&BB#BB#BY   \n"
                       "  :PGB##&&&@@@&&#? .     ";

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

    int row = 2;
    int col = 1;

    mvprintw(0, 0, "Monk Mode v0.0");

    refresh();

    mvwprintw(text_win, row++, col, "Time: %s", timeStr);
    mvwprintw(text_win, row++, col, "Monk Name: %s\n", monk.name.c_str());
    double sanity = difftime(now, monk.birthday);
    mvwprintw(text_win, row++, col, "Sanity: %g\n", sanity);

    for (Vice v : monk.vices) {
        mvwprintw(text_win, row++, col, "Vice Name : %s\n", v.name.c_str());
        mvwprintw(text_win, row++, col, "Vice Damage : %d\n", v.damage);
    }

    box(text_win, 0, 0);
    wrefresh(text_win);

    mvwprintw(monk_win, 1, 1, monk_img);

    box(monk_win, 0, 0);
    wrefresh(monk_win);
}

void* input_thread_func(void* arg){
    initscr(); // Initialize ncurses
    noecho(); // Disable automatic echoing of input characters
    keypad(stdscr, true); // Enable function keys (e.g. arrow keys)

    text_win = newwin(10, 50, 2, 1);
    monk_win = newwin(34, 30, 2, 51);

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