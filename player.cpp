#include <vector>
#include "potato.h"
#include "setup.h"

/** This function is get the server's port number
 * @param fd is the server socket fd
 */
int get_port(int fd) {
    struct sockaddr_in sin;
    socklen_t s_len = sizeof(sin);
    if(getsockname(fd, (struct sockaddr *)& sin, &s_len) == -1) {
        std::cerr << "Error: can not get socket name" << std::endl;
        exit(EXIT_FAILURE);
    }
    return ntohs(sin.sin_port);
}

/** This function is to wait for recieving a potato from its neighbors
 * @param left_neigb is the fd of player's left neighbor
 * @param right_neigb is the fd of player's right neighbor
 * @param m_fd is the fd of ringmaster
 * @param player_id is player's index
 * @param num_players is the total number of players
 */
void receive_potato(int& right_neigb_fd, int& left_neigb_fd, int& m_fd, int player_id, int num_players) {
    Potato potato;
    fd_set readfds;
    int max_fd = max(right_neigb_fd, max(left_neigb_fd, m_fd));
    srand((unsigned int) time(NULL) + player_id);
    while(true) {
        FD_ZERO(&readfds);
        FD_SET(right_neigb_fd, &readfds);
        FD_SET(left_neigb_fd, &readfds);
        FD_SET(m_fd, &readfds);
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        int tmp = 0;
        if (FD_ISSET(right_neigb_fd, &readfds)) {
            tmp = recv(right_neigb_fd, &potato, sizeof(potato), MSG_WAITALL);
        }
        else if (FD_ISSET(left_neigb_fd, &readfds)) {
            tmp = recv(left_neigb_fd, &potato, sizeof(potato), MSG_WAITALL);
        }
        else if (FD_ISSET(m_fd, &readfds)) {
            tmp = recv(m_fd, &potato, sizeof(potato), MSG_WAITALL);
        }
        if (potato.equals_to(0) || tmp == 0) {
            break;
        } else {
            potato.decrease_hops_by_one();
            int times = potato.get_times();
            potato.add_path(times, player_id);
            potato.increase_times();
            if (potato.equals_to(0)) {
                send(m_fd, &potato, sizeof(potato), 0);
                cout << "I'm it" << endl;
            }
            else {
                int random = rand() % 2;
                if (random) {
                    send(right_neigb_fd, &potato, sizeof(potato), 0);
                    int right_nid = (player_id + 1) % num_players;
                    std::cout << "Sending potato to " << right_nid << std::endl;
                }
                else {
                    send(left_neigb_fd, &potato, sizeof(potato), 0);
                    int left_nid = (player_id + num_players - 1) % num_players;
                    std::cout << "Sending potato to " << left_nid << std::endl;
                }
            }
        }

    }
}

/** This function is close the connection on the client side
 * @param left_neigb is the fd of player's left neighbor
 * @param right_neigb is the fd of player's right neighbor
 * @param m_fd is the fd of ringmaster
 */
void close_client(int& left_neigb, int& right_neigb, int& m_fd) {
    close(left_neigb);
    close(right_neigb);
    close(m_fd);
}

int main(int argc, char *argv[]) {
    const char* m_host = argv[1];
    const char* m_port = argv[2];
    int player_id = 0;
    int num_players = 0;
    if (argc != 3) {
        report_error("usage should be : player <machine_name> <port_num>");
        return EXIT_FAILURE;
    }
    int m_fd = client_init(m_host, m_port);
    recv(m_fd, &player_id, sizeof(player_id), 0);
    recv(m_fd, &num_players, sizeof(num_players), 0);
    int player_fd = server_init("");
    int player_port = get_port(player_fd);
    send(m_fd, &player_port, sizeof(player_port), 0);
    std::cout << "Connected as player " << player_id << " out of " << num_players << " total players" << std::endl;
    int neigb_port;
    char neigb_ip[100];
    recv(m_fd, &neigb_ip, sizeof(neigb_ip), MSG_WAITALL);
    recv(m_fd, &neigb_port, sizeof(neigb_port), MSG_WAITALL);   
    char _neigb_port[9];
    sprintf(_neigb_port, "%d", neigb_port);
    int right_neigb_fd = client_init(neigb_ip, _neigb_port);
    string as_server_ip;
    int left_neigb_fd = accept_connection(player_fd, &as_server_ip);
    receive_potato(right_neigb_fd, left_neigb_fd, m_fd, player_id, num_players);
    close_client(left_neigb_fd, right_neigb_fd, m_fd);   
}