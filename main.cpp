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

const char* monk_img = "____________\\GbaBe5PNy)._______________\n"
                       "___________vgU333333PkB8o2r`____________\n"
                       "__________jDb33UsG3333Z@&O4Y____________\n"
                       "_________.6@En4EEk3333g$PQ81____________\n"
                       "_________J@DEEEEOP3333Bn3B@<____________\n"
                       "________ydBDEEEERQ3333P3o@0.____________\n"
                       "_______V60B@EEEEE#33333O@@c_____________\n"
                       "______ya6ww@EEEEeW3333#D@0'_____________\n"
                       "_____L9#%g9@DMmw4P333O%@@Z______________\n"
                       "____|0E6@mm9dbO5#4446@@@@9~_____________\n"
                       "___,S6k@&#WEREEEEEE6@@@@@@hV.___________\n"
                       "___P8G$BG3UOb9m88dEwdwmmB@@dr___________\n"
                       "__v@#4oP3333PP#hM@66b$ggogmR'___________\n"
                       "__K8&oP3333PG333P46%@@@%D@@b.___________\n"
                       "_z0qQ33333P4#333333W@@D0qM@@T___________\n"
                       "ySQ3333UWoRER333333Pbmp9#o&B9>__________\n"
                       "S96a9g5&@@@bEQZOs333PEGMZReSW&Z;________\n"
                       "*S@Moq6m@@@DWREk33333Pn6WnEWEEdB(_______\n"
                       "_,Ip333Pp@@@9eopRn3Q#REWaPOEEEE%f_______\n"
                       "___43333npgD@@@@@@9bEEEEaQsEEq$$\"______\n"
                       "___BP333334&@@@@@@@@Mheg&99Sb@@S;_______\n"
                       "___&s333GOb8@@@@@@@@@@@@@@@%@&SbgV______\n"
                       "___$eP33G4O#sGS6MM@6g6MSknsG4oSbd0Y_____\n"
                       "___dd#P333333333Bb&W33Ue9pUPGEEgD&ai____\n"
                       "___4@qkP3333333ZDWm#npPR8ZQ3kgMph6MA____\n"
                       "___K@MpkP333333kEwRkh%s8&3333o@bh&aV____\n"
                       "____1DmWER4Q333GdOng@bZ%63333P$6wD&\\___\n"
                       "_____|5MeEEZ3P#0RnEBDEP5W33ns3oEb@B`____\n"
                       "______,M%SEO#RBpEEEE&oZQ3334Ek5Ee@S_____\n"
                       "______K@@@wWEEEEEEEEEeEE5QsEEEEEo@F_____\n"
                       "_____(9@@@@@9wgoEEEEEEEEWeSgbgg66z______\n"
                       "____.S$0Mb%@%@@@D9$&&09D%D@@d>c=,_______\n"
                       "____f@oM9EmMp%@$9@@@@96@BEbDB___________\n"
                       "____G$h@8E9@W6@eE0@m@mEhhOEw@i__________\n"
                       "____cg8@ME&@SE6EE6@w@@p4E5RqD?__________\n"
                       "____co@@DEB@gEEEEB@b@%O#45#ODN__________\n"
                       "____Ee@&@ES@bEEEEBMb@$R##4440p__________\n"
                       "___:9EMW@EED6EEEEq9h@BEEEEEE6D;_________\n"
                       "___/MEeEDqEwaEE#n4eS@oEEEEEEe@K_________\n"
                       "___fDEEE@wEE6EEs33GE@eEEEEEEo@I_________\n"
                       "___Z%EOe@mEEEEEk333P%SEEEEEEe@q_________\n"
                       "___6@kPo@@qEEsUG33338bEEEEE4p@D;________\n"
                       "__'%@U3q@@&EEQ33333P864OEEZ3#@@z________\n"
                       "__j@@Q3O@@@pEZ33333Gmw4QROP34@@e________\n"
                       "__v@@G3P$@@aEk33333n%dR3UU33E@@M`_______\n"
                       "__2@@Z33nD@%oO333334@$EP3333p@@@!_______\n"
                       "__Z@@n333g@@0EU3333E@9EU3333S@@@+_______\n"
                       "__5@@k33UE&@@hZ333PW@MEQ33336@@@V_______\n"
                       "__4@@533sEWM@D5333QE@@EG3333a@@@N_______\n"
                       "__e@@#3P4EEq@@S333sE$0ps3333q@@@n_______\n"
                       "__B@@43kEEEE6@DQ33nEdbws3333n@@@S_______\n"
                       "__9@@OZEEER5sb@E33kq&EEZ33333a@@m_______\n"
                       "_,@@@pREEEs33Phg334q6EEn33333W@@@!______\n"
                       "_*@@@bEEEk33333Q33OEEEEn33333#@@@K______\n"
                       "_\\@@@bEERP3333333PEEEhEk33333Z@@@^_____\n"
                       "_K@@@hEE533333333QEEE0Ek33333Q@@@s______\n"
                       "_V@@@SEEG33333333sEEeDE#33333P@@@B______\n"
                       "_1@@@SEEP33333333nEEh@E#333333D@@@!_____\n"
                       "_F@@@SEEP3333333POEEd@o#333333M@@@N_____\n"
                       "_Z@@@gEEU333333P5EEE8@q4333333M@@@M}____\n"
                       "_W@@@6EEZ333333nEEEW@@S4333333D@@@@S.___\n"
                       "_6@@@wEEOP3333nEEEEb@@g433333U@@@@@@I___\n"
                       "_8@@@&EEEOsPU#EEEEE8@@SR333335@@@@@@@L__\n"
                       "'@@@@8EEEEEREEEEEEw@@@eRP3333g@@@@@@@d-_\n"
                       "(@@@@%WEEEEEEEEEWa@@@@EEZ3P3ZD@@@@@gu>~_\n"
                       "}4GA9@$SeSb6Ba&8@@@@@%EER44qM@@@@@9,____\n"
                       "____b@@@@@@@@@@@@@@@@@0wd&mgR&M9gQY_____\n"
                       "____F@@@@@@@@@@@@@@@@@@@@@@M(___________\n"
                       "____-xNU#obdm@@@@@@@@@@@%&eP]___________";

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
    refresh();

    time_t now;
    time(&now);
    char* timeStr = std::ctime(&now);

    int row = 2;
    int col = 1;

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

    text_win = newwin(10, 50, 1, 1);
    monk_win = newwin(71, 43, 1, 51);

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