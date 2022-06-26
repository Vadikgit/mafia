#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <typeinfo>
#include "Player.h"
#include <string>
#include <string_view>
#include "Game.h"
#include <QInputDialog>
#include "playercard.h"
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

#include <QWidget>
#include <QTextEdit>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QThread>
#include <QDir>
#include <QPixmap>


class GameClient;

class MyClient : public QWidget {
Q_OBJECT
public://private:
QTcpSocket* m_pTcpSocket;
    QTextEdit*  m_ptxtOutput;
    QLineEdit*  m_ptxtInput;
    quint16     m_nNextBlockSize;

public:
    GameClient* p_game_client;
    friend class MainWindow;
    MyClient( QWidget* pwgt = 0) ;
    void startClient(const QString& strHost, int nPort);
    void sendToServer(QString);

private slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToServer(                            );
    void slotConnected   (                            );
};

class MainWindow;

class MyServer : public QWidget {
Q_OBJECT
public://private:
QTcpServer* m_ptcpServer;
QTextEdit*  m_ptxtOutput;
QLineEdit*  m_ptxtInput;
quint16     m_nNextBlockSize;

public:
    void sendToClient(QTcpSocket* pSocket, const QString& str);


public:
    friend class MainWindow;
    MainWindow * p_mainwin;
    MyServer(QWidget* pwgt = 0);
    void startServer(int nPort);

public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();

};


class GameClient
{
public:
  MainWindow* p_MainWindow;
  std::string name;
  std::vector<std::pair<std::string, int>> morning_status_fr_clnt;
  std::vector<std::pair<std::string, int>> prev_morning_status_fr_clnt;

  std::vector<int> other_mafias_for_mafia;
  int my_role;
  int number_of_players_cl;
  GameClient(MainWindow*);
  void enter_name();
  void process_message(QString);
  void process_morning_info(QString);
  void ask_vote_cl();
  void ask_action_cl();
  void answer_for_vote_or_action_and_send_result(int);
  void set_cards_style_and_clickable_cl();
};


class Game;
class Player;
class PlayerCard;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QPushButton *start_new_game_btn;
    QPushButton *join_the_game_btn;
    QLabel *about_you_label;
    QLabel *p_background_label;
    QPixmap *p_background_day_image;
    QPixmap *p_background_night_image;

QGridLayout *p_grid;
    bool why_choosing;
    const bool voting_now = true;
    const bool action_now = false;
    QWidget *cenntral_widget_for_game;
    void client_game_process();
    void server_game_process();
    void add_new_player_in_players(QString, QTcpSocket*);

    GameClient * p_client;


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    MyClient *tcp_client_ptr;
    MyServer *tcp_server_ptr;

    void rand_mix(std::vector<int>&);
    int inputAmountOfPlayers();
    std::vector<int> setRoles(int );
    int numberOfPlayers_sr;
    int id_sr;
    std::vector<int> roles_sr;
    std::vector<Player> players;
std::vector<PlayerCard*> players_cards;
    std::vector<bool> available_cards_for_click;
void denie_clicks();
void accept_clicks();

void process_users_choise(QString);
    //void startGame_onserver(std::vector<Player>&, int);
    Game* p_game;

/*     //server
     QVBoxLayout* pvbxLayout;

public:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxtOutput_sr;
    QLineEdit*  m_ptxtInput_sr;
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();


        //client
private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit*  m_ptxtOutput_cl;
    QLineEdit*  m_ptxtInput_cl;
    //quint16     m_nNextBlockSize;


private slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToServer(                            );
    void slotConnected   (                            );*/
};

#endif // MAINWINDOW_H
