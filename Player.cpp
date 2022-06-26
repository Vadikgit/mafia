#include "Player.h"


Player::Player(const std::string& userName,  QTcpSocket* p_Socket,MainWindow* p_main, int role, bool isAdmin) :
    playerName(userName), pClientSocket(p_Socket),parent_main_win(p_main), statusID(0), playerRole(role), isAdmin(isAdmin){}

const std::string& Player::getPlayerName()
{
    return playerName;
}

int Player::setSleepStatus()
{
    if(statusID != 2){
        statusID = 3;
    }
    if (!isAdmin) {
        std::string sendBuf = "i3";

        parent_main_win->tcp_server_ptr->sendToClient(pClientSocket , QString::fromStdString(sendBuf));
    }
    else
        parent_main_win->tcp_server_ptr->m_ptxtOutput->append( "\nIt's night in the city");

    return 0;
}

int Player::setAwakeStatus()
{
    //if(statusID != 2){
        statusID = 0;
    //}
    return 0;
}

int Player::asc_vote()
{
    if (!isAdmin) {
        std::string sendBuf = "V";

        //QString c_text = QInputDialog::getText(nullptr, "check", "before vote ask");

        parent_main_win->tcp_server_ptr->sendToClient(pClientSocket , QString::fromStdString(sendBuf));


        //parent_game->set_cards_style_and_clickable_Game();
    }
    else {

        parent_main_win->accept_clicks();
        parent_main_win->tcp_server_ptr->m_ptxtOutput->append("\nPlease, vote");

        //QString c_text = QInputDialog::getText(nullptr, "Enter name of suspect", "Enter:");

        parent_main_win->why_choosing = parent_main_win->voting_now;
        //answer_vote('v' + c_text);
    }
    return 0;
}

int Player::answer_vote(QString received_string){
    //if (playerRole == 2) return 100;

    parent_game->voting_counter++;
    received_string = QString::fromStdString(received_string.toStdString().erase(0,1));
    parent_game->votes[parent_game->validateNameAndGetID(received_string.toStdString())]++;

    if(parent_game->voting_counter == parent_game->voting_must_have){
        parent_game->max_votes = std::max_element(parent_game->votes.begin(), parent_game->votes.end());

        if(std::find(parent_game->max_votes + 1, parent_game->votes.end(), *parent_game->max_votes) != parent_game->votes.end()){
            parent_game->voting();
        }
        else
        {
            parent_game->players[std::distance(parent_game->votes.begin(), parent_game->max_votes)].setDeadStatus();
            parent_game->dead_after_voting = std::distance(parent_game->votes.begin(), parent_game->max_votes);
            parent_game->day();
        }
    }

    return 0;
}

int Player::asc_action()
{
    //if (playerRole == 2) return 100;
    if (!isAdmin) {
        std::string sendBuf = "D";

        //QString c_text = QInputDialog::getText(nullptr, "check", "before mafia");
        parent_main_win->tcp_server_ptr->sendToClient(pClientSocket, QString::fromStdString(sendBuf));
    }
    else {
        //std::cout << "\nEnter name of Target person\n";
        //parent_main_win->tcp_server_ptr->m_ptxtOutput->append("\nEnter name of Target person\n");


        switch (getPlayerRole())
        {
        case 0:
            parent_main_win->tcp_server_ptr->m_ptxtOutput->append( "\n((((((((((");
            break;
        case 1:
            parent_main_win->tcp_server_ptr->m_ptxtOutput->append( "\nArest anybody, policeman");
            break;
        case 2:
            parent_main_win->tcp_server_ptr->m_ptxtOutput->append( "\nCare anybody, doctor");
            break;
        case 3:
            parent_main_win->tcp_server_ptr->m_ptxtOutput->append( "\nKill anybody, mafia");
            break;
        }


        //QString c_text = QInputDialog::getText(nullptr, "name of Target", "Enter name of Target person:");

        //parent_game->set_cards_style_and_clickable_Game();
        parent_main_win->accept_clicks();
        parent_main_win->why_choosing = parent_main_win->action_now;
        //answer_action('d'+c_text);//for erasing
    }
    return 0;
}

int Player::answer_action(QString received_string)
{
    //if (playerRole == 2) return 100;

    received_string = QString::fromStdString(received_string.toStdString().erase(0,1));
    //parent_main_win->tcp_server_ptr->m_ptxtOutput->append(received_string);
    switch (playerRole) {
    case 1:
    {    std::string suspect_str;
        int suspect;
        suspect = parent_game->validateNameAndGetID(received_string.toStdString());
        std::string sendBuf;
        if (parent_game->players[suspect].getPlayerRole() == 3)
            sendBuf = "C1d" + std::to_string(suspect);
        else
            sendBuf = "C0d" + std::to_string(suspect);

        if (!parent_game->players[parent_game->police].getIsAdmin()) {

            parent_main_win->tcp_server_ptr->sendToClient(parent_game->players[parent_game->police].getSocket(), QString::fromStdString(sendBuf));
        }
        else {
            //parent_main_win->tcp_server_ptr->m_ptxtOutput->append(QString::fromStdString(sendBuf) + '\n');
            if(parent_game->players[suspect].getPlayerRole() == 3){
                parent_main_win->tcp_server_ptr->m_ptxtOutput->append("\nYou catched mafia!!! Congradulations!!!");
                parent_main_win->players_cards[suspect]->show_role(3);
            }else{
                parent_main_win->tcp_server_ptr->m_ptxtOutput->append("\nYou'll be lucky another time(((");
            }
        }

        parent_game->night_flags_cur.suspectID = suspect;
        parent_game->night();
        break;
    }
    case 2:
    {   int cured;
        cured = parent_game->validateNameAndGetID(received_string.toStdString());
        parent_game->night_flags_cur.curedID = cured;
        parent_game->night();
        break;
    }

    case 3:
    {   parent_game->victims.push_back(parent_game->validateNameAndGetID(received_string.toStdString()));
        if(parent_game->victims.size() == parent_game->mafia_must_have){
            if(!compare(parent_game->victims)){
                parent_game->mafiaAction();
            }
            else{
                parent_game->night_flags_cur.killedByMafiaID = parent_game->victims[0];
                parent_game->night();
            }
        }
        break;
    }
    }

    return 0;
}


QTcpSocket* Player::getSocket()
{
    return pClientSocket;
}

bool Player::getIsAdmin()
{
    return isAdmin;
}

int Player::getPlayerRole()
{
    return playerRole;
}

int Player::getStatus()
{
    return statusID;
}

int Player::setKilledAndHealthedStatus()
{
    statusID = 1;
    return 0;
}

int Player::setHealthed()
{
    statusID = 4;
    return 0;
}

int Player::setDeadStatus()
{
    statusID = 2;
    return 0;
}
