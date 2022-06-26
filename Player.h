#pragma once
#include <string>

#include <iostream>
#include "mainwindow.h"
#include "Game.h"

#include <QTcpSocket>
#include <QTcpServer>

class MainWindow;
class Game;
class Player
{
public://private:
    bool isAdmin;
    QTcpSocket* pClientSocket;
    //SOCKET ClientSocket;
    int playerRole; //0-ìèðíûé æèòåëü, 1-êîììèñàð, 2-âðà÷,3-ìàôèÿ
    std::string playerName; //Èìÿ ïîëüçîâàòåëÿ â ìàôèè
    int statusID; //0 - íîðìàëüíîå ñîñòîÿíèå (áîäðñòâóåò), 1 - óáèò ìàôèåé è âûëå÷åí, 2 - ìåðòâ, 3 - ñïèò, 4-âûëå÷åí â õîëîñòóþ


public:
    Player(const std::string& userName, QTcpSocket* pSocket, MainWindow* pt_main, int role, bool isAdmin);
    const std::string& getPlayerName();
    QTcpSocket* getSocket();
    MainWindow* parent_main_win;
    bool getIsAdmin();
    int getPlayerRole();
    int getStatus();
    int setSleepStatus();//i3
    int setAwakeStatus();//i0
    int asc_vote();
    int asc_action();
    int answer_vote(QString);
    int answer_action(QString);
    int setKilledAndHealthedStatus();//i1
    int setHealthed();//i4
    int setDeadStatus();//i2

   Game* parent_game;
};
