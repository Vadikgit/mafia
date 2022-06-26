#undef UNICODE

#include "mainwindow.h"

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
#include "QPixmap"


MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{
    //out = new QTextEdit(this);
    //out->setGeometry(10,350,380,40);

    cenntral_widget_for_game = new QWidget(this);
    start_new_game_btn = new QPushButton("Start new game",this);
    join_the_game_btn = new QPushButton("Join the game",this);

    about_you_label = new QLabel(this);
    about_you_label->setGeometry(0,0,100,20);
    start_new_game_btn->setGeometry(480,200,100,80);
    join_the_game_btn->setGeometry(620,200,100,80);
    p_grid = new QGridLayout(cenntral_widget_for_game);
    //p_grid->addWidget(about_you_label,0,0,1,1);
    //about_you_label->setText("kjnkj");
    setCentralWidget(cenntral_widget_for_game);

    tcp_client_ptr = nullptr;
    tcp_server_ptr = nullptr;

    p_background_day_image = new QPixmap(":/new/images/day.png");
    p_background_night_image = new QPixmap(":/new/images/night.png");
    //*p_man_image = p_man_image->scaled(120,120,Qt::KeepAspectRatio);

    p_background_label = new QLabel(cenntral_widget_for_game);
    p_background_label->setPixmap(*p_background_night_image);

    p_background_label->setMaximumSize(p_background_day_image->size());
    p_background_label->setMinimumSize(p_background_day_image->size());

    connect(start_new_game_btn, &QPushButton::clicked, this, &MainWindow::server_game_process);
    connect(join_the_game_btn, &QPushButton::clicked, this, &MainWindow::client_game_process);
    //pvbxLayout = new QVBoxLayout;


    p_background_label->setStyleSheet(".QLabel{ border: 8px solid rgb(198, 100, 255); }");

}

void MainWindow::client_game_process(){

    start_new_game_btn->hide();
    join_the_game_btn->hide();

    QString c_text = QInputDialog::getText(this, "Target IP", "Enter target IP");
    static MyClient mcl;
    tcp_client_ptr = &mcl;
    tcp_client_ptr->setFixedSize(300,300);
    p_grid->addWidget(tcp_client_ptr,0,0,1,1);

    tcp_client_ptr->startClient(c_text,2323);
    p_client = new GameClient(this);
    tcp_client_ptr->p_game_client = p_client;

    p_client->enter_name();
    tcp_client_ptr->sendToServer(QString::fromStdString('&' + p_client->name));

}

void MainWindow::server_game_process(){
    start_new_game_btn->hide();
    join_the_game_btn->hide();

    static MyServer msr;
    tcp_server_ptr = &msr;
    tcp_server_ptr->setFixedSize(300,300);

    tcp_server_ptr->startServer(2323);
    tcp_server_ptr->p_mainwin = this;
    //tcp_server_ptr->setGeometry(0,0,200,200);

    numberOfPlayers_sr = inputAmountOfPlayers();

    for (int i = 0; i<numberOfPlayers_sr; i++) {
       players_cards.push_back(new PlayerCard(cenntral_widget_for_game));
       players_cards[i]->p_Main_Window = this;
       p_grid->addWidget(players_cards[i],0,i+1,1,1);
    }
    p_grid->addWidget(tcp_server_ptr,1,1,2,numberOfPlayers_sr-1);
    id_sr = 0;

    players.clear();
    roles_sr = setRoles(numberOfPlayers_sr);

}

void showPlayers(MainWindow* p_mainw,std::vector<Player>& players) {
//    for (auto player : players) {
//        p_mainw->tcp_server_ptr->m_ptxtOutput->append(QString::fromStdString(player.getPlayerName() + "\n"));
//    }

    std::string str_of_names = "";
    std::string adm_nm;
    for (auto player : players) {
        if(!player.getIsAdmin()){
            str_of_names.append(", " + player.getPlayerName());
        }
        else{
            adm_nm = player.getPlayerName();
        }
    }

    str_of_names = str_of_names.substr(2,str_of_names.length()-2);
    p_mainw->tcp_server_ptr->m_ptxtOutput->append("\n" + QString::fromStdString(adm_nm) + ", you are playing with:\n"+
                                                       QString::fromStdString(str_of_names));
}

void showPlayerRole(MainWindow* p_mainw,std::vector<Player>& players) {
    switch (players.back().getPlayerRole()) {
    case 0:
        p_mainw->tcp_server_ptr->m_ptxtOutput->append("\nYou are the villian");
        break;
    case 1:
        p_mainw->tcp_server_ptr->m_ptxtOutput->append("\nYou are the police");
        break;
    case 2:
        p_mainw->tcp_server_ptr->m_ptxtOutput->append("\nYou are the doctor");
        break;
    case 3:
        p_mainw->tcp_server_ptr->m_ptxtOutput->append("\nYou are the mafia");
        break;
    }


}

void MainWindow::add_new_player_in_players(QString name, QTcpSocket *pClntSckt){
    players.push_back(Player(name.toStdString(), pClntSckt,this, roles_sr[id_sr++], false));

    players_cards[players.size()-1]->p_name->setText(name);

    QString str1, str2;
    str1.setNum(players.size()+1);
    str2.setNum(numberOfPlayers_sr);
    tcp_server_ptr->m_ptxtOutput->append("\n" + str1+ " players of " + str2 + " have connected!\n");

    if(players.size() == (numberOfPlayers_sr - 1)){
        std::string adminName;
        adminName = (QInputDialog::getText(this, "Name", "Enter your name")).toStdString();

        players.push_back(Player(adminName, nullptr, this,roles_sr[id_sr++], true));
        players_cards[players.size()-1]->p_name->setText(QString::fromStdString(adminName));

        QString label_text = QString::fromStdString(adminName);
        switch (roles_sr[roles_sr.size()-1])
        {
        case 0:
            label_text = label_text + " (Villian)";
            break;
        case 1:
            label_text = label_text + " (Policeman)";
            break;
        case 2:
            label_text = label_text + " (Doctor)";
            break;
        case 3:
            label_text = label_text + " (Mafia)";
            break;
        }
        about_you_label->setText(label_text);


        showPlayers(this, players);
        showPlayerRole(this, players);

        std::string playersNames = "n";
        for (auto player : players) {
            playersNames += player.getPlayerName() + ",";
        }

        playersNames.erase(playersNames.end() - 1);
        std::string sendBuf;
        for (auto player : players) {
            std::string mafstr = "";
            if(player.getPlayerRole() == 3){
                mafstr = "t";
                for (int i = 0; i< numberOfPlayers_sr; i++) {
                    if((players[i].getPlayerRole() == 3) && (player.getPlayerName() != players[i].getPlayerName())){
                        mafstr = mafstr + char(i+48);
                    }
                }
            }

            sendBuf = "sa" + std::to_string(numberOfPlayers_sr) + "r" + std::to_string(player.getPlayerRole())+ "i0"+ mafstr + playersNames;

            if (!player.getIsAdmin()) {
                tcp_server_ptr->sendToClient(player.getSocket(), QString::fromStdString(sendBuf));
            }
        }

        p_game = new Game(players, this);

        for (int i = 0; i < players.size(); i++) {
            players[i].parent_game = p_game;
        }
        tcp_server_ptr->m_ptxtOutput->append("\nThe game is started!");

        p_game->startGame();
    }
}

void GameClient::enter_name(){
    QString c_text = QInputDialog::getText(nullptr, "Your name", "Enter your name:");
    name = c_text.toStdString();
    p_MainWindow->about_you_label->setText(QString::fromStdString(name));
}

MainWindow::~MainWindow(){}


//client
///////

GameClient::GameClient(MainWindow* m_ptr):p_MainWindow(m_ptr){
    other_mafias_for_mafia.clear();
}

void GameClient::process_morning_info(QString str) {
    int len = str.length();
    int offset = 8;//if you are mafia it changes furter
    if (str[0] == 's')
    {
        my_role = str[4].unicode() - 48;
        QString label_text = p_MainWindow->about_you_label->text();
        switch (my_role)
        {
        case 0:
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nYou are the villian");
            label_text = label_text + " (Villian)";
            break;
        case 1:
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nYou are the policeman");
            label_text = label_text + " (Policeman)";
            break;
        case 2:
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nYou are the doctor");
            label_text = label_text + " (Doctor)";
            break;
        case 3:
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nYou are the mafia");
            label_text = label_text + " (Mafia)";

            while(str[offset] != 'n'){
                other_mafias_for_mafia.push_back(str[offset].unicode() - 48);
                offset++;
            }
            offset++;
            break;
        }
        p_MainWindow->about_you_label->setText(label_text);
        number_of_players_cl = str[2].unicode() - 48;
        morning_status_fr_clnt.resize(number_of_players_cl);
        int cur_name = 0;
        morning_status_fr_clnt[cur_name].first.clear();
        morning_status_fr_clnt[cur_name].second = 0;


        for (int i = 0; i<number_of_players_cl; i++) {
           p_MainWindow->players_cards.push_back(new PlayerCard(p_MainWindow->cenntral_widget_for_game));
           p_MainWindow->players_cards[i]->p_Main_Window = p_MainWindow;
           p_MainWindow->p_grid->addWidget(p_MainWindow->players_cards[i],0,i+1,1,1);
        }
        p_MainWindow->p_grid->addWidget(p_MainWindow->tcp_client_ptr,1,1,2,number_of_players_cl-1);


        p_MainWindow->players_cards[cur_name]->p_name->setText("");

        for (int i = offset; i < len; i++)
        {
            if (str[i] == ',') {
                cur_name++;
                morning_status_fr_clnt[cur_name].first.clear();
                morning_status_fr_clnt[cur_name].second = 0;
                //p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\n");

                p_MainWindow->players_cards[cur_name]->p_name->setText("");
            }
            else
            {
                morning_status_fr_clnt[cur_name].first.push_back(str[i].toLatin1());
                //p_MainWindow->tcp_client_ptr->m_ptxtOutput->append(str[i]);

                QString namestr = p_MainWindow->players_cards[cur_name]->p_name->text();

                p_MainWindow->players_cards[cur_name]->p_name->setText(namestr + str[i]);
            }
        }

        std::string str_of_names = "";

        for(int i = 0; i < morning_status_fr_clnt.size(); i++){
            if(morning_status_fr_clnt[i].first == name){
                p_MainWindow->players_cards[i]->show_role(my_role);

                if(my_role == 3){
                    for(int j = 0; j < other_mafias_for_mafia.size(); j++){
                        p_MainWindow->players_cards[other_mafias_for_mafia[j]]->show_role(my_role);
                    }
                }
            }
            else{
                str_of_names.append(", " + morning_status_fr_clnt[i].first);
            }
        }

        str_of_names = str_of_names.substr(2,str_of_names.length()-2);
        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\n" + QString::fromStdString(name) + ", you are playing with:\n"+
                                                           QString::fromStdString(str_of_names));






        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nThe game is started!");

        if ((len < 9) || (str[1] != 'a') || (str[3] != 'r'))
        {
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nIncorrect start message!");
            return;
        }

        set_cards_style_and_clickable_cl();

//        for(int i = 0; i < number_of_players_cl;i++){
//            p_MainWindow->players_cards[i]->is_clickable=false;
//        }

        p_MainWindow->denie_clicks();

//        for (int i = 0; i<number_of_players_cl; i++) {
//            p_MainWindow->players_cards.push_back(new PlayerCard(p_MainWindow->cenntral_widget_for_game));
//            p_MainWindow->p_grid->addWidget(p_MainWindow->players_cards[i],0,i+1,1,1);
//        }
        p_MainWindow->p_grid->addWidget(p_MainWindow->tcp_client_ptr,1,1,2,number_of_players_cl-1);
    }

    else
    {
        if (str[0] != 'm')
        {
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nIncorrect morning message!");
            return;
        }

        p_MainWindow->p_background_label->setPixmap(*p_MainWindow->p_background_day_image);

        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nIt's morning in the city");

        int status;
        std::vector<std::pair<std::string, int>> prev_status = morning_status_fr_clnt;
        for (int i = 3; i < len; i = i + 3){
            status = str[i].unicode() - 48;
            morning_status_fr_clnt[str[i-2].unicode() - 48].second = status;
        }

        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\n");
        for (int i = 0; i < morning_status_fr_clnt.size(); i++)
        {
            //p_MainWindow->tcp_client_ptr->m_ptxtOutput->append(QString::fromStdString( morning_status_fr_clnt[i].first));
            QString tmp_nm  = QString::fromStdString( morning_status_fr_clnt[i].first);
            switch (morning_status_fr_clnt[i].second)
            {
            case 0:
                p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\n" + tmp_nm + " is awaked");
                    break;
            case 1:
                p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\n" + tmp_nm + " is killed and cured");
                    break;
            case 2:
               p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\n" + tmp_nm +  " is dead");
                    break;
            case 3:
               p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\n" + tmp_nm + " is slipping");
                    break;
            default:
                break;
            }
        }
        //p_MainWindow->tcp_client_ptr->m_ptxtOutput->append( "Status u!\n");

        set_cards_style_and_clickable_cl();
    }
}

void GameClient::ask_action_cl() {

    p_MainWindow->why_choosing = p_MainWindow->action_now;
    switch (my_role)
    {
    case 0:
        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append( "\n((((((((((");
        break;
    case 1:
        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append( "\nArest anybody, policeman");
        break;
    case 2:
        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append( "\nCare anybody, doctor");
        break;
    case 3:
        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append( "\nKill anybody, mafia");
        break;
    }
}

void GameClient::ask_vote_cl() {
    p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nVoting is started");
    p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nPlease, vote");
    p_MainWindow->why_choosing = p_MainWindow->voting_now;
}

void GameClient::process_message(QString str) {

    if ((str[0] == 's') || (str[0] == 'm'))
    {
        process_morning_info(str);
    }
    else if (str[0] == 'D')
    {
        QString rl_str;
        rl_str.setNum(my_role);
        //p_MainWindow->tcp_client_ptr->m_ptxtOutput->append(rl_str);
        //make_choise();

        p_MainWindow->accept_clicks();
        set_cards_style_and_clickable_cl();
        ask_action_cl();
    }
    else if(str[0] == 'C')
    {
        if (str[1] == '1')
        {
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nYou catched mafia!!! Congradulations!!!");

               p_MainWindow->players_cards[str[3].unicode() - 48]->show_role(3);

        }
        else if(str[1] == '0')
        {
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nYou'll be lucky another time(((");
        }
    }
    else if (str[0] == 'V')
    {
        //vote();

        p_MainWindow->accept_clicks();
        set_cards_style_and_clickable_cl();
        ask_vote_cl();
    }
    else if (str[0] == 'k')
    {
        int vote_victim = str[1].unicode() - 48;
         p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nDuring the voting we desided to kill " + QString::fromStdString(morning_status_fr_clnt[vote_victim].first));
         morning_status_fr_clnt[vote_victim].second = 2;
         int vote_victim_role = str[3].unicode() - 48;
         p_MainWindow->players_cards[vote_victim]->show_role(vote_victim_role);

         set_cards_style_and_clickable_cl();
    }
    else if (str[0] == 'i')
    {
        p_MainWindow->p_background_label->setPixmap(*p_MainWindow->p_background_night_image);
        p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nIt's night in the city");
    }
    else if (str[0] == 'w')
    {
        if(str[1] == '0'){
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nMafia won");
        }
        else
        {
            p_MainWindow->tcp_client_ptr->m_ptxtOutput->append("\nVillians won");
        }
    }
}

void GameClient::set_cards_style_and_clickable_cl(){
    for(int i = 0; i < morning_status_fr_clnt.size(); i++){
        if(morning_status_fr_clnt[i].first == name){
            p_MainWindow->players_cards[i]->set_you_style();
            p_MainWindow->players_cards[i]->is_clickable = false;
        }
        else if(morning_status_fr_clnt[i].second == 2){
            p_MainWindow->players_cards[i]->set_killed_style();
            p_MainWindow->players_cards[i]->is_clickable = false;
        }
    }
}

//server

//////

void MainWindow::rand_mix(std::vector<int>& arr) {

    //ïîäêëþ÷àåì windows.h
    SYSTEMTIME tm;
    GetLocalTime(&tm);
    srand(tm.wMilliseconds);
    for (int i = 0; i < arr.size(); i++)
    {
        int change_pos = i + (rand()) % (arr.size() - i);
        int tmp = arr[i];
        arr[i] = arr[change_pos];
        arr[change_pos] = tmp;
    }

}

std::vector<int> MainWindow::setRoles(int numberOfPlayers) {

    switch (numberOfPlayers)
    {
    case 2:
        roles_sr = { 3,1 };
        break;
    case 3:
        roles_sr = { 3,1,0 };
        break;
    case 4:
        roles_sr = { 3,1,0,0 };
        break;
    case 5:
        roles_sr = { 3,3,1,0,0 };
        break;
    case 6:
        roles_sr = { 3,3,2,1,0,0 };
        break;
    case 7:
        roles_sr = { 3,3,3,2,1,0,0 };
        break;
    }
    rand_mix(roles_sr);
    return roles_sr;
}

int MainWindow::inputAmountOfPlayers() {
    tcp_server_ptr->m_ptxtOutput->append("Enter amount of players\n");
    QString numberOfPlayers = QInputDialog::getText(this, "Amount of players", "Enter:");
    //return atoi(numberOfPlayers.c_str());
    return numberOfPlayers.toInt();
}// change 2

////////////////////////////

MyClient::MyClient(QWidget* pwgt /*=0*/) : QWidget(pwgt), m_nNextBlockSize(0)
{
    m_pTcpSocket = new QTcpSocket(this);

    //m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );

    m_ptxtOutput  = new QTextEdit;
    //m_ptxtInput = new QLineEdit;

    //connect(m_ptxtInput, SIGNAL(returnPressed()),
    //        this,        SLOT(slotSendToServer())
    //      );
    m_ptxtOutput->setReadOnly(true);

    //QPushButton* pcmd = new QPushButton("&Send");
    //connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    //pvbxLayout->addWidget(new QLabel("<H1>Client</H1>"));
    pvbxLayout->addWidget(m_ptxtOutput);
    //pvbxLayout->addWidget(m_ptxtInput);
    //pvbxLayout->addWidget(pcmd);
    setLayout(pvbxLayout);
    this->show();
}

void MyClient::startClient(const QString &strHost, int nPort){
    m_pTcpSocket->connectToHost(strHost, nPort);
}

void MyClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_2);

    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> /*time >>*/ str;

        //m_ptxtOutput->append(/*time.toString() + " " +*/ str+ '\n');
        p_game_client->process_message(str);
        m_nNextBlockSize = 0;
    }
}

void MyClient::slotError(QAbstractSocket::SocketError err)
{         
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                             "The host was not found." :
                             err == QAbstractSocket::RemoteHostClosedError ?
                                 "The remote host is closed." :
                                 err == QAbstractSocket::ConnectionRefusedError ?
                                     "The connection was refused." :
                                     QString(m_pTcpSocket->errorString())
                                     );


    m_ptxtOutput->append(strError);
}

void MyClient::sendToServer(QString str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << /*QTime::currentTime() <<*/ str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
}

void MyClient::slotSendToServer()
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << /*QTime::currentTime() <<*/ m_ptxtInput->text();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    m_ptxtInput->setText("");
}

void MyClient::slotConnected()
{
    m_ptxtOutput->append("Connection is established!\n");
}

////////////////////////////

MyServer::MyServer(QWidget* pwgt /*=0*/) : QWidget(pwgt), m_nNextBlockSize(0)
{
    m_ptxtOutput = new QTextEdit;
    m_ptxtOutput->setReadOnly(true);

    //p_mainwin = ;
    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    //pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxtOutput);
    //pvbxLayout->addWidget(m_ptxtInput);
    setLayout(pvbxLayout);
    this->show();
}

void MyServer::startServer(int nPort){
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              + m_ptcpServer->errorString()
                             );
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()),
            this,         SLOT(slotNewConnection())
           );
}

void MyServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    QWidget::connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );

    sendToClient(pClientSocket, "Server Response: Connected!");
}

void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_2);
    //pClientSocket->localAddress().toString();
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> /*time >>*/ str;

        QString strMessage =
            /*time.toString() + " " + "Client has sended - " +*/ str;
        //m_ptxtOutput->append(pClientSocket->peerAddress().toString()+":   "+ strMessage);

        m_nNextBlockSize = 0;

        if(p_mainwin->players.size() == p_mainwin->numberOfPlayers_sr){
            //m_ptxtOutput->append(p_mainwin->p_game->players[0].getSocket()->peerAddress().toString());
            //sendToClient(pClientSocket, /*"Server Response: Received \"" + */str /*+ "\""*/);
            for (int i = 0; i < p_mainwin->p_game->players.size(); i++) {
                if(!p_mainwin->players[i].getIsAdmin()){
                    if(p_mainwin->players[i].getSocket()->peerAddress().toString() == pClientSocket->peerAddress().toString()){
                        p_mainwin->p_game->process_message_Game(&p_mainwin->players[i], strMessage);
                        break;
                    }
                }
            }
        }else{
            if(strMessage[0] == '&'){
                std::string name = strMessage.toStdString();
                name.erase(name.begin());
                p_mainwin->add_new_player_in_players(QString::fromStdString(name), pClientSocket);
            }
        }

    }
}

////////////////////////////

void MainWindow::process_users_choise(QString  received_str){

//    for(int i = 0; i < numberOfPlayers_sr;i++){
//        players_cards[i]->is_clickable=false;
//    }

    denie_clicks();

    if(tcp_server_ptr != nullptr){
        int index = p_game->validateNameAndGetID(received_str.toStdString());
        tcp_server_ptr->m_ptxtOutput->append("Your choise is " + QString::fromStdString((players[index].getPlayerName()))+'\n');

        Player* adm_player;
        for (int i = 0; i < players.size() ; i++) {
            if(players[i].getIsAdmin()){
                adm_player = &players[i];
                break;
            }
        }

        if(why_choosing == voting_now){
            adm_player->answer_vote("v" + received_str);
        }
        else{
            adm_player->answer_action("d" + received_str);
        }

    }
    else
    {
        int index = -1;
        for(int i = 0; i < p_client->morning_status_fr_clnt.size(); i++){
            if(p_client->morning_status_fr_clnt[i].first == received_str.toStdString()){
                tcp_client_ptr->m_ptxtOutput->append("Your choise is " + received_str);
                index = i;
                break;
            }
        }

        p_client->answer_for_vote_or_action_and_send_result(index);

    }

}

void GameClient::answer_for_vote_or_action_and_send_result(int index){

    if(p_MainWindow->why_choosing == p_MainWindow->action_now){
    QString numstr;
    numstr.setNum(index);

    p_MainWindow->tcp_client_ptr->sendToServer('d' + numstr);

    }
    else{

    QString numstr;
    numstr.setNum(index);

    p_MainWindow->tcp_client_ptr->sendToServer('v'+numstr);
    }
}

void MyServer::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << /*QTime::currentTime() <<*/ str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}


void MainWindow::denie_clicks(){
    available_cards_for_click.clear();
    for (int i = 0; i < players_cards.size(); i++ ) {
        available_cards_for_click.push_back(players_cards[i]->is_clickable);
        players_cards[i]->is_clickable = false;
    }
}

void MainWindow::accept_clicks(){
    for (int i = 0; i < players_cards.size(); i++ ) {
        players_cards[i]->is_clickable = available_cards_for_click[i];
    }
}
