#include "setup.h"
#include "potato.h"
#include <vector>
#include <algorithm>

/** This function is to help ringmaster to connect with its players before the begining of the game
 * @param socket_fd specifies the server socket fd 
 * @param port specifies the port used to communicate with clients
 * @param num_players is the total number of players
 * @param players_fd is all the players' fds
 * @param players_port is all the players' ports
 * @param players_ip is all the players' ips
 */
void set_player(int socket_fd, const char * port, int num_players, vector<int>& players_fd,  vector<int>& players_port, vector<string>& players_ip) {
    for(int i = 0; i < num_players; i++) {
        int player_port;
        string player_ip_addr;
        int player_connect_fd = accept_connection(socket_fd, &player_ip_addr);
        players_fd.push_back(player_connect_fd);
        players_ip.push_back(player_ip_addr);
        send(player_connect_fd, &i, sizeof(i), 0);
        send(player_connect_fd, &num_players, sizeof(num_players), 0);
        recv(player_connect_fd, &player_port, sizeof(player_port), 0);
        players_port.push_back(player_port);
        cout<<"Player " << i << " is ready to play" << endl;
    }
}
/** This function is to send the neighbor information to each player
 * @param players_fd is all the players' fds
 * @param players_port is all the players' ports
 * @param players_ip is all the players' ips
 */
void send_neigb_info(vector<int>&players_fd, vector<int>& players_port, vector<string>&players_ip)  {
    int num_players = players_fd.size();
    for(int i = 0; i < num_players; i++) {
        int neigb_id = (i + 1) % num_players;
        int neigb_port = players_port[neigb_id];
        char neigb_ip[100];
        memset(neigb_ip, 0, sizeof(neigb_ip));
        string neigb_ip_add = players_ip[neigb_id];
        strcpy(neigb_ip, neigb_ip_add.c_str());
        send(players_fd[i], &neigb_ip, sizeof(neigb_ip), 0);
        send(players_fd[i], &neigb_port, sizeof(neigb_port), 0);
    }
}
/** This function is to assign the potato randomly by generating a random number (range from[0, num_players - 1])
 * @param players_fd is all the players' fds
 * @param potato is the object passing around
 */
void assign_potato(vector<int>&players_fd, Potato& potato) {
    int num_players = players_fd.size();
    srand((unsigned int)time(NULL) + num_players);
    int ran_num = rand() % num_players;
    cout << "Ready to start the game, sending potato to player " << ran_num << endl;
    if (send(players_fd[ran_num], &potato, sizeof(potato), 0) != sizeof(potato)) {
        report_error("the potato is broken");
    }
}
/** This function is to receive the potato from players who is ready
 * @param players_fd is all the players' fds
 * @param potato is the object passing around
 * @param max_fd is the largest player_fd
 */
void receive_potato(vector<int>&players_fd, Potato& potato, int& max_fd) {
    fd_set readfds;
    FD_ZERO (&readfds);
    int num_players = players_fd.size();
    for(int i = 0; i < num_players; i++) {
            FD_SET(players_fd[i], &readfds);
        }
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
    for (int i = 0; i < num_players; i++) {
        if (FD_ISSET(players_fd[i], &readfds)) {
        recv(players_fd[i], &potato, sizeof(potato), MSG_WAITALL);
        break;
        }
    }
}
/** This function is announce all the players to end the game
 * @param players_fd is all the players' fds
 * @param p is a Potato object that reach its last hop
 */
void announce_all(vector<int>&players_fd, Potato& p) {
    for(int i = 0; i < players_fd.size(); i++) {
        send(players_fd[i], &p,sizeof(p), 0);
    }
}
/** This function is announce all the players to end the game
 * @param players_fd is all the players' fds
 * @param socket_fd is the server socket fd
 */
void close_all(vector<int> players_fd, int socket_fd) {
    for(int i = 0; i < players_fd.size(); i++) {
        close(players_fd[i]);
    }
    close(socket_fd);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        report_error("usage should be: ringmaster <port_num> <num_players> <num_hops>");
        return EXIT_FAILURE;
    }
    const char * m_port = argv[1];
    int num_players = atoi(argv[2]);
    if (num_players <= 1) {
        report_error("the 'num_players' must be greater than 1");
        return EXIT_FAILURE;
    }
    int num_hops = atoi(argv[3]);
    if (num_hops < 0 || num_hops > 512) {
        report_error("the 'num_hops' must be in the range [0, 512]");
        return EXIT_FAILURE;
    }    
    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << num_players << endl;
    cout << "Hops = " << num_hops << endl;

    vector<int> players_fd;
    vector<int> players_port;
    vector<string> players_ip;
    int socket_fd = server_init(m_port);
    set_player(socket_fd, m_port, num_players, players_fd, players_port, players_ip);
    send_neigb_info(players_fd, players_port, players_ip);
    Potato potato;
    potato.set_hops(num_hops);

 // start to play the game and choose the player randomly by generating the random number
    if (num_hops > 0) {
        assign_potato(players_fd, potato);
        vector<int>::iterator p = max_element(players_fd.begin(), players_fd.end());
        int max_fd = *p;
        receive_potato(players_fd, potato, max_fd);
    }
    announce_all(players_fd, potato);
    potato.showTrace();
    close_all(players_fd, socket_fd);
    return EXIT_SUCCESS;
}

