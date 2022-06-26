#pragma once

#include <string>

#include <iostream>
#include <vector>
#include "Player.h"
#include <QString>
#include "mainwindow.h"

class Player;

class MainWindow;

class Game
{
public://private:
    std::vector<Player> &players;
    std::vector<short> mafia;
    int mafia_must_have;

    short police;
    short doctor;
    short lastDeath;

    std::vector<int> victims;
    std::vector<int> votes;
    std::vector<int>::iterator max_votes;
    int voting_counter;
    int voting_must_have;
    int dead_after_voting;

public:
    Game(std::vector<Player>& players, MainWindow * p_main_win);
    void startGame();
    void night();
    void allSleep();
    void allWakeUp();
    void day();
    void sendMorningStatus();
    void showAdminMorningInfo();
    int checkGameStatus();
    int voting();
    int mafiaAction();
    int policeAction();
    int doctorAction();
    int validateNameAndGetID(std::string playerName);
    void process_message_Game(Player* p_player,QString);
    void set_cards_style_and_clickable_Game();

    friend class MainWindow;
    MainWindow * p_mainwin;

    struct night_flags
    {
        bool mafia_done;
        bool police_done;
        bool doctor_done;

        int killedByMafiaID;
        int suspectID;
        int curedID;

    } night_flags_cur;

    struct day_flags
    {
        bool is_before_voting;
    } day_flags_cur;
};

bool compare(const std::vector<int>& victims);
