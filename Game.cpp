#include "Game.h"
#include <algorithm>


void Game::process_message_Game(Player* p_player, QString str){

    if (str[0] == 'd' && str[1] >= '0' && str[1] <= '7'){
        //p_player->answer_action();
        p_player->answer_action(str);
    }
    else if(str[0] == 'v'){
        p_player->answer_vote(str);
    }

}

int Game::checkGameStatus()
{
    int mafiaPlayersDead = 0, mafiaPlayerAlive = 0;
    int otherPlayersDead = 0, otherPlayersAlive = 0;
    for (auto& player : players) {
        if (player.getPlayerRole() == 3 && player.getStatus() != 2) {
            mafiaPlayerAlive++;
        }
        if (player.getPlayerRole() != 3 && player.getStatus() != 2) {
            otherPlayersAlive++;
        }


        if (player.getPlayerRole() == 3 && player.getStatus() == 2) {
            mafiaPlayersDead++;
        }
        if (player.getPlayerRole() != 3 && player.getStatus() == 2) {
            otherPlayersDead++;
        }
    }

    if (mafiaPlayersDead == mafia.size()) return 1;
    if (otherPlayersDead == (players.size() - mafia.size()) || ((otherPlayersAlive == 1) && (mafiaPlayerAlive >= otherPlayersAlive))) return 2;
    return 0;
}

void Game::showAdminMorningInfo()
{
    //std::cout << "Current players:\n";
    p_mainwin->tcp_server_ptr->m_ptxtOutput->append("Current players:\n");
    for (auto player : players) {
        //std::cout << player.getPlayerName();
        p_mainwin->tcp_server_ptr->m_ptxtOutput->append(QString::fromStdString(player.getPlayerName()));
        if (player.getStatus() == 2){
            //std::cout << ": is dead";
            p_mainwin->tcp_server_ptr->m_ptxtOutput->append(": is dead\n");
        }
        if (player.getStatus() == 1){
            //std::cout << ": was killed by mafia, but then was healthed by doctor";
            p_mainwin->tcp_server_ptr->m_ptxtOutput->append(": was killed by mafia, but then was healthed by doctor\n");
        }
        if (player.getStatus() == 4){
            //std::cout << ": was healthed by doctor";
            p_mainwin->tcp_server_ptr->m_ptxtOutput->append(": was healthed by doctor\n");
        }
        //std::cout << "\n\n";
        p_mainwin->tcp_server_ptr->m_ptxtOutput->append("\n\n");
    }


    if (lastDeath != -1) {
        //std::cout << "Was killed by mafia tonight: " << players[lastDeath].getPlayerName() << "\n\n";
        p_mainwin->tcp_server_ptr->m_ptxtOutput->append("Was killed by mafia tonight: " + QString::fromStdString(players[lastDeath].getPlayerName())+ "\n\n");
    }
    else {
        //std::cout << "Nobody was killed by mafia\n\n";
        p_mainwin->tcp_server_ptr->m_ptxtOutput->append("Nobody was killed by mafia\n\n");
    }
}

void Game::allWakeUp() {
    for (auto& player : players) {
        if (player.getStatus() == 3)
            player.setAwakeStatus();
    }
}

void Game::sendMorningStatus()
{
    std::string sendBuf = "m";
    for (int i = 0; i < players.size(); ++i) {
        sendBuf += std::to_string(i) + 'i' + std::to_string(players[i].getStatus());
    }

    for (auto& player : players) {
        int iSendResult;
        if (!player.getIsAdmin()) {

            p_mainwin->tcp_server_ptr->sendToClient(player.getSocket(),QString::fromStdString(sendBuf));
        }
    }
}

bool compare(const std::vector<int>& victims) {
    switch (victims.size()) {
    case 0:
        return false;
    case 1:
        return true;
    case 2:
        return (victims[0] == victims[1]);
    case 3:
        return((victims[0] == victims[1]) && (victims[0] == victims[2]) && (victims[1] == victims[2]));
    case 4:
        return((victims[0] == victims[1]) && (victims[2] == victims[3]) && (victims[1] == victims[3]));
    }
}

int Game::validateNameAndGetID(std::string playerName)
{
    if (playerName.length() == 1 && playerName.front() >= '0' && playerName.front() <= '7') {
        return atoi(playerName.c_str());
    }

    auto playerID = std::find_if(players.begin(), players.end(), [playerName](Player& player) {
        return (playerName == player.getPlayerName());
        });


    if (playerID != players.end()) {
        return std::distance(players.begin(), playerID);
    }

    return 100;
}

int Game::voting()
{
   voting_counter = 0;
   voting_must_have = 0;
   votes.assign(players.size(), 0);
    for (auto& player : players) {
        if (player.getStatus() == 2){
            continue;
        }
         voting_must_have++;
        player.asc_vote();
    }
    return 0;
}


Game::Game(std::vector<Player>& players, MainWindow * p_main_win) :players(players), police(-1), doctor(-1), lastDeath(-1), p_mainwin(p_main_win) {
    for (int i = 0; i < players.size(); ++i) {
        switch (players[i].getPlayerRole())
        {
        case 1:
            police = i;
            break;
        case 2:
            doctor = i;
            break;
        case 3:
            mafia.push_back(i);
            break;
        default:
            break;
        }
    }
}

void Game::startGame() {

    p_mainwin->players_cards[players.size() - 1]->show_role(players[players.size() - 1].getPlayerRole());
    if(players[players.size()-1].getPlayerRole() == 3){
        for (int i = 0; i < mafia.size(); i++) {
            if(mafia[i] != (players.size()-1)){
                p_mainwin->players_cards[mafia[i]]->show_role(3);
            }
        }
    }

set_cards_style_and_clickable_Game();

//for(int i = 0; i < p_mainwin->numberOfPlayers_sr;i++){
//    p_mainwin->players_cards[i]->is_clickable=false;
//}

p_mainwin->denie_clicks();

night_flags_cur.mafia_done = false;
        night_flags_cur.police_done = false;
        night_flags_cur.doctor_done = false;

        //QString c_text = QInputDialog::getText(nullptr, "check", "before night");
        QMessageBox msgBox;
        msgBox.setText("Let the night come?");
        msgBox.exec();
        night();
}

void Game::allSleep()
{
    for (auto& player : players) {
        //if (player.getStatus() != 2)
            player.setSleepStatus();
    }
}

int Game::mafiaAction()
{
    victims.clear();
    mafia_must_have = 0;
    std::string victim;
    for (auto mafiaID : mafia) {
        if (players[mafiaID].getStatus() == 2) continue;
        mafia_must_have++;
        players[mafiaID].asc_action();

    }
    return 0;
}

int Game::policeAction()
{
    if (players[police].getStatus() == 2 || police == -1){
        night();
        return 100;
    }
    std::string suspect_str;
    int suspect;
    players[police].asc_action();
    return 0;
}

int Game::doctorAction()
{
    if ((doctor == -1) || (players[doctor].getStatus() == 2)) {
        night();
        return 100;
    }

    players[doctor].asc_action();
    return 0;
}

void Game::night()
{
    if(!night_flags_cur.mafia_done){
        p_mainwin->p_background_label->setPixmap(*p_mainwin->p_background_night_image);

        allSleep();
        night_flags_cur.killedByMafiaID = 100;
        night_flags_cur.killedByMafiaID; // reinitialise in answer_action of player
        night_flags_cur.mafia_done = true;

        //QString c_text = QInputDialog::getText(nullptr, "check", "before mafia");

        QMessageBox msgBox;
        msgBox.setText("There is mafia's time!");
        msgBox.exec();
        mafiaAction();
        return;
    }

    if(!night_flags_cur.police_done){
        night_flags_cur.suspectID = 100;
        night_flags_cur.suspectID; // reinitialise in answer_action of player
        night_flags_cur.police_done = true;

        //QString c_text = QInputDialog::getText(nullptr, "check", "before police");

        QMessageBox msgBox;
        msgBox.setText("The policeman is working now!");
        msgBox.exec();
        policeAction();
        return;
    }

    if(!night_flags_cur.doctor_done){
        night_flags_cur.curedID = 100;
        night_flags_cur.curedID; // reinitialise in answer_action of player
        night_flags_cur.doctor_done = true;

        //QString c_text = QInputDialog::getText(nullptr, "check", "before doctor");

        if (!((doctor == -1) || (players[doctor].getStatus() == 2))){
            QMessageBox msgBox;
            msgBox.setText("The doctor goes to save lives!");
            msgBox.exec();
        }
        doctorAction();
        return;
    }

    if(night_flags_cur.doctor_done){
        lastDeath = -1;
        if (night_flags_cur.killedByMafiaID == night_flags_cur.curedID && night_flags_cur.killedByMafiaID != 100)
            players[night_flags_cur.killedByMafiaID].setKilledAndHealthedStatus();
        else {
            if (night_flags_cur.killedByMafiaID != 100) players[night_flags_cur.killedByMafiaID].setDeadStatus();
            if (night_flags_cur.curedID != 100) players[night_flags_cur.curedID].setHealthed();
            lastDeath = night_flags_cur.killedByMafiaID;
        }
        night_flags_cur.mafia_done = false;
        night_flags_cur.police_done = false;
        night_flags_cur.doctor_done = false;
        if (checkGameStatus() != 0) {
            if (checkGameStatus() == 1) {
                p_mainwin->tcp_server_ptr->m_ptxtOutput->append("Villians won\n");
                for(int i = 0; i < players.size();i++){
                    if(!players[i].getIsAdmin()){
                        p_mainwin->tcp_server_ptr->sendToClient(players[i].getSocket(),"w1");
                    }
                }
            }
            else {
                p_mainwin->tcp_server_ptr->m_ptxtOutput->append("Mafia win\n");
                for(int i = 0; i < players.size();i++){
                    if(!players[i].getIsAdmin()){
                        p_mainwin->tcp_server_ptr->sendToClient(players[i].getSocket(),"w0");
                    }
                }
            }
            return;
        }
        day_flags_cur.is_before_voting = true;
       // QString c_text = QInputDialog::getText(nullptr, "check", "before day");

        day();
    }
}

void Game::day()
{
    if(day_flags_cur.is_before_voting){

        //QString c_text = QInputDialog::getText(nullptr, "check", "before sending");

        QMessageBox msgBox;
        msgBox.setText("Let the day come?");
        msgBox.exec();
        p_mainwin->p_background_label->setPixmap(*p_mainwin->p_background_day_image);

        allWakeUp();

        p_mainwin->tcp_server_ptr->m_ptxtOutput->append("\nIt's morning in the city");
        sendMorningStatus();
        set_cards_style_and_clickable_Game();
        showAdminMorningInfo();

        //std::cout << "\nVoting is started\n";
        p_mainwin->tcp_server_ptr->m_ptxtOutput->append("\nVoting is started\n");
        day_flags_cur.is_before_voting = false;

        //QString cc_text = QInputDialog::getText(nullptr, "check", "before voting");
        QMessageBox msBox;
        msBox.setText("Lets begin the voting?");
        msBox.exec();

        voting();
    }
    else{
        //std::cout << players[dead].getPlayerName() << " was killed by voting\n";
        p_mainwin->tcp_server_ptr->m_ptxtOutput->append(QString::fromStdString(players[dead_after_voting].getPlayerName())+" was killed by voting\n");

        p_mainwin->players_cards[dead_after_voting]->show_role(players[dead_after_voting].getPlayerRole());
        std::string sendBuf = "k" + std::to_string(dead_after_voting) + "r" + std::to_string(players[dead_after_voting].getPlayerRole());
        for (auto& player : players) {
            if (!player.getIsAdmin()) {
                p_mainwin->tcp_server_ptr->sendToClient(player.getSocket(), QString::fromStdString(sendBuf));
            }
        }

        set_cards_style_and_clickable_Game();

        night_flags_cur.mafia_done = false;
        night_flags_cur.police_done = false;
        night_flags_cur.doctor_done = false;
        if (checkGameStatus() != 0) {
            if (checkGameStatus() == 1) {
                p_mainwin->tcp_server_ptr->m_ptxtOutput->append("Villians won\n");
                for(int i = 0; i < players.size();i++){
                    if(!players[i].getIsAdmin()){
                        p_mainwin->tcp_server_ptr->sendToClient(players[i].getSocket(),"w1");
                    }
                }
            }else{
                p_mainwin->tcp_server_ptr->m_ptxtOutput->append("Mafia won\n");
                for(int i = 0; i < players.size();i++){
                    if(!players[i].getIsAdmin()){
                        p_mainwin->tcp_server_ptr->sendToClient(players[i].getSocket(),"w0");
                    }
                }
            }
            return;
        }
        day_flags_cur.is_before_voting = true;

        //QString c_text = QInputDialog::getText(nullptr, "check", "before night");

        QMessageBox msgBox;
        msgBox.setText("Let the night come?");
        msgBox.exec();
        night();
    }
}

void Game::set_cards_style_and_clickable_Game(){
    for(int i = 0; i < players.size(); i++){
        if(players[i].getIsAdmin()){
            p_mainwin->players_cards[i]->set_you_style();
            p_mainwin->players_cards[i]->is_clickable = false;

        }
        else if(players[i].getStatus() == 2){
            p_mainwin->players_cards[i]->set_killed_style();
            p_mainwin->players_cards[i]->is_clickable = false;
        }
    }
}
