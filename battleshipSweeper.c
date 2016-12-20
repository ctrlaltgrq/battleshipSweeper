#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define MAX_CLIENT 2
#define MAX_BUFFER 2048
#define PORT "1111"
#define TIMING 10
#define MAXHIT 28

/*
 * a linked list for clients
 */
typedef struct client {
    int fd;
    struct client* next;
    bool is_next;

} client;

/**
 * a struct for cells
 * hasShip: the cell has ship?
 * size: marks the size of a ship
 */
typedef struct cell {
    int hasShip;
    int size;
}cell;

/**
 * head of the clients linked list
 */
client* head = NULL;

/**
 * table that stores the ships
 */
cell table[15][15];

/**
 * add client to the linkedlist
*/
void add_client(int fd)
{
    client* new_item = (client*)malloc(sizeof(client));
    new_item->fd = fd;
    new_item->next = head;
    new_item->is_next = false;
    head = new_item;
}

/**
 * del client from the linkedlist
 */
void del_client(int fd)
{
    client* it = head;
    client* prev = NULL;

    for (; it->next != NULL && it->fd != fd; it = it->next) {
        prev = it;
    }

    if (it->fd != fd) {
        printf("There is no %d fd", fd);
    } else {
        if (it == head) {
            head = head->next;
            free(it);
        } else {
            prev->next = it->next;
            free(it);
        }
    }
}

/**
 * print the clients linkedlist
 */
void print_clients()
{
    client* it = head;
    puts("");
    for (; it != NULL; it = it->next) {
        printf("%d: %d --> ", it->is_next, it->fd);
    }
    puts("");
}

/**
 * how many clients are in the list
 */
int get_number_of_clients()
{
    client* it = head;
    int counter = 0;

    for (; it != NULL; it = it->next) {
        counter++;
    }

    return counter;
}

/**
 * enum for the ship placing
 */
typedef enum direction {
    LEFT,
    RIGHT,
    UP,
    DOWN
} direction;

/**
 * place the ships to the table
 * size: size of the ship
 * pc: how many ships have to place to the table
 */
void place_ships(int size, int pc) {

    /**
     * horizontal or vertical
     */
    int dir = 0;

    /**
     * the beginning x coordinate of a ship
     */
    int xCoord = 0;

    /**
     * the beginning y coordinate of a ship
     */
    int yCoord = 0;

    /**
     * loop variables
     */
    int i,j,k;

    /**
     * if bad == true we have to place the ship somewhere else
     */
    bool bad = false;
    direction direct;

    for (i = 0; i < pc; i++) {

        bad = true;

        /**
         * while bad we have to ask new coordinate
         */
        while (bad) {

            /**
             * random x coordinate 0-14
             */
            xCoord = rand() % 15;

            /**
             * random y coordinate 0-14
             */
            yCoord = rand() % 15;

            /**
              * random direction
              * 0: horizontal
              * 1: vertical
              */
            dir = rand() % 2;


            /**
             * if the cell is empty
             */
            if (table[xCoord][yCoord].hasShip == 0) {
                /**
                 * if horizontal
                 */
                if (dir == 0) {  //horizontal

                    /**
                     * if the ship is fit to the table
                     */
                    if (xCoord+size <= 14) {
                        for (j = 0; j <= size; j++) {
                            /**
                              * this is avoid overlap
                              */
                            if (table[xCoord + j][yCoord].hasShip == 0) {
                                /**
                                 * if there is no overlap
                                 */
                                if (j == size) {
                                    direct = RIGHT;
                                    bad = false;
                                } else {
                                    bad = true;
                                }
                            } else {
                                bad = true;
                                break;
                            }
                        }
                    } else if (xCoord-size >= 0) {
                        for (j = 0; j <= size; j++) {
                            if (table[xCoord - j][yCoord].hasShip == 0) {
                                if (j == size) {
                                    direct = LEFT;
                                    bad = false;
                                } else {
                                    bad = true;
                                }
                            } else {
                                bad = true;
                                break;
                            }
                        }
                    } else {
                        bad = true;
                    }
                } else { //vertical
                    if (yCoord+size <= 14) {
                        for (j = 0; j <= size; j++) {
                            if (table[xCoord][yCoord + j].hasShip == 0) {
                                if (j == size) {
                                    direct = DOWN;
                                    bad = false;
                                } else {
                                    bad = true;
                                }
                            } else {
                                bad  = true;
                                break;
                            }
                        }
                    } else if (yCoord-size >= 0) {
                        for (j = 0; j <= size; j++) {
                            if (table[xCoord][yCoord - j].hasShip == 0) {
                                if (j == size) {
                                    direct = UP;
                                    bad = false;
                                } else {
                                    bad = true;
                                }
                            } else {
                                bad = true;
                                break;
                            }
                        }
                    } else {
                        bad = true;
                    }
                }
            } else {
                bad = true;
            }
        }

        /**
          * placing the ships
          */
        switch (direct) {
        case LEFT:
            for (k = 0; k < size; k++) {
                table[xCoord - k][yCoord].hasShip = 1;
                table[xCoord - k][yCoord].size = size;
            }

            break;
        case RIGHT:
            for (k = 0; k < size; k++) {
                table[xCoord + k][yCoord].hasShip = 1;
                table[xCoord + k][yCoord].size = size;
            }
            break;
        case UP:
            for (k = 0; k < size; k++) {
                table[xCoord][yCoord - k].hasShip = 1;
                table[xCoord][yCoord - k].size = size;
            }
            break;
        case DOWN:
            for (k = 0; k < size; k++) {
                table[xCoord][yCoord + k].hasShip = 1;
                table[xCoord][yCoord + k].size = size;
            }
            break;
        }
    }
}

/**
  * set is_next for the client
  */
void setNext(int fd, bool next) {
    client* it = head;
    for (; it != NULL && it->fd != fd; it = it->next);
    it->is_next = next;
}

/**
 * get is_next from the client
 */
bool getNext(int fd) {
    client* it = head;
    for (; it != NULL && it->fd != fd; it = it->next);

    return it->is_next;
}

/**
  * draw the table
  */
void draw(){
    int i, j, k;
    printf("\n\n  ");
    for(k=0;k<15;k++)
        printf(" %2d ", k+1);
    printf("\n  -");
    for(k=0;k<15;k++)
        printf("----");
    printf("\n");
    for(i=0;i<15;i++){
        printf("%c ", i+'A');
        for(j=0;j<15;j++){
            if(table[i][j].size != -1)
                printf("|%2d ", table[i][j].size);
            else
                printf("|   ");
        }
        printf("| %c\n", i+'A');
    }
    printf("  -");
    for(k=0;k<15;k++)
        printf("----");
    printf("\n  ");
    for(k=0;k<15;k++)
        printf(" %2d ", k+1);
    printf("\n");
}

/**
  * send the rules from file
  */
void sendrules(int fd)
{
    char filename[20]="rules.txt";
    char buf[MAX_BUFFER];
    bzero(buf, sizeof(buf));
    FILE *pfile;
    int i=0;
    if((pfile = fopen(filename, "rb")) == NULL){
        perror("Can't open the file");
    }
    while(!feof(pfile)){
        buf[i++]=(unsigned char)fgetc(pfile);
    }
    send(fd, buf, strlen(buf)-1, 0);
}

int main(int argc, char *argv[])
{
    /**
     * for getaddrinfo
     * result: the getaddrinfo result
     * hints: the parameters for getaddrinfo
     */
    struct addrinfo* result;
    struct addrinfo hints;
    bzero(&hints, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /**
     * get ip for server
     */
    getaddrinfo(NULL, PORT, &hints, &result);

    /**
     * Berkeley sockets:
     *  socket
     *  bind
     *  listen
     *  accept
     *
     */
    int listener_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    int yes = 1;
    if (setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == 1) {
        perror("setsockopt");
        exit(1);
    }

    freeaddrinfo(result);

    bind(listener_socket, result->ai_addr, result->ai_addrlen);

    listen(listener_socket, MAX_CLIENT);

    /**
     * fd_sets for select
     */
    fd_set readFds;
    fd_set master;

    FD_ZERO(&master);
    FD_SET(listener_socket, &master);

    int fdmax = listener_socket;
    int new_fd;
    int i,j,k,x,y;
    int max;

    int received_bytes;
    char received_message[MAX_BUFFER];

    pid_t pid = 0;

    char send_message[MAX_BUFFER];

    bool initialized = false;

    /**
     * stores who is next
     * socket fd-s
     */
    int first, second;

    /**
     * that will be sent for TABLE command
     */
    char visible_table[15][15];

    /**
     * hit counters
     */
    int first_player_hits = 0;
    int second_player_hits = 0;

    /**
     * select will be return every 0.5 sec
     */
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500;

    for (i = 0; i < 15; i++) {
        for (j = 0; j < 15; j++) {
            visible_table[i][j] = ' ';
        }
    }

    int cnum = 0;

    while (1) {

        /**
          * the players shot all the ships
          */
        if (first_player_hits + second_player_hits == MAXHIT) {
            bzero(send_message, sizeof send_message);
            sprintf(send_message, "1st player hits: %d\n 2nd player hits: %d\n\nPlay again?(Y/N)\n", first_player_hits, second_player_hits);

            initialized = false;

            for (k = listener_socket + 1; k <= fdmax; k++) {
                if (FD_ISSET(k, &master)) {
                    send(k, send_message, sizeof send_message, 0);
                }
            }
            first_player_hits = 0;
            second_player_hits = 0;
        }

        readFds = master;
        select(fdmax + 1, &readFds, NULL, NULL, &tv);



        srand(time(NULL));


        for (i = listener_socket; i <= fdmax; i++) {
            if (FD_ISSET(i, &readFds)) {

                /**
                  * if somebody connected
                  */
                if (i == listener_socket) {

                    new_fd = accept(listener_socket,0,0);

                    print_clients();

                    /**
                      * if 2 player connected
                      */
                    if(cnum == MAX_CLIENT) {
                        puts("Maxclients");
                        /**
                          * start a new process
                          */
                        pid = fork();
                        if(pid==0) {//child
                            close(listener_socket);
                            FD_CLR(listener_socket,&master);
                            close(new_fd);
                        } else {//parent
                            for(j=listener_socket + 1;j<=fdmax;j++) {
                                if((FD_ISSET(j,&master))) {
                                    close(j);
                                    del_client(j);
                                }
                            }
                            FD_ZERO(&master);
                            FD_SET(listener_socket,&master);
                            FD_SET(new_fd,&master);
                            add_client(new_fd);
                            fdmax = (listener_socket>new_fd)?listener_socket:new_fd;
                            initialized = false;
                            first = new_fd;
                            second = 0;
                            cnum = 1;
                            send(new_fd, "Welcome to the battleship game.\nWaiting for other player...\n",
                                 sizeof "Welcome to the battleship game.\nWaiting for other player...\n", 0);
                        }
                    } else {

                        if (cnum == 1) {
                            second = new_fd;
                            send(new_fd, "Player2 is connected\n", sizeof "Player2 is connected\n", 0);
                            add_client(new_fd);
                            setNext(new_fd, true);
                            puts("cnum 1");
                            cnum++;
                        } else if (cnum == 0) {
                            add_client(new_fd);
                            first = new_fd;
                            send(new_fd, "Welcome to the battleship game.\nWaiting for other player...\n",
                                 sizeof "Welcome to the battleship game.\nWaiting for other player...\n", 0);
                            cnum++;
                            puts("cnum 0");
                        }

                        FD_SET(new_fd,&master);
                        if(fdmax<new_fd) {
                            fdmax = new_fd;
                        }

                    }


                } else {

                    bzero(received_message, sizeof received_message);
                    received_bytes = recv(i, received_message, sizeof received_message, 0);

                    /**
                      * if somebody send disconnected command
                      */
                    if (received_bytes == 0) {
                        del_client(i);
                        close(i);
                        FD_CLR(i, &master);

                        print_clients();

                        cnum = 0;
                        max = 0;
                        for (j = listener_socket + 1; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master) && j > max) {
                                max = j;
                            }
                        }
                        fdmax = max;
                    } else {

                        bzero(send_message, sizeof send_message);

                        if (first_player_hits + second_player_hits == MAXHIT) {
                            if (strncmp(received_message, "Y", 1) == 0) {
                                initialized = false;
                                setNext(first, false);
                                setNext(second, true);

                                for (k = listener_socket + 1; k <= fdmax; k++) {
                                    if (FD_ISSET(k, &master)) {
                                        send(k, "New game started...\n", sizeof "New game started...\n", 0);
                                    }
                                }

                            } else if (strncmp(received_message, "N", 1) == 0){
                                close(first);
                                close(second);

                                del_client(first);
                                del_client(second);
                                FD_CLR(first, &master);
                                FD_CLR(second, &master);

                                first = 0;
                                second = 0;

                                print_clients();

                                max = 0;
                                for (j = listener_socket + 1; j <= fdmax; j++) {
                                    if (FD_ISSET(j, &master) && j > max) {
                                        max = j;
                                    }
                                }
                            }
                        }

                        /**
                          * initialize
                          */
                        if (!initialized) {

                            first_player_hits = 0;
                            second_player_hits = 0;

                            for (x = 0; x < 15; x++) {
                                for (y = 0; y < 15; y++) {
                                    table[x][y].size = -1;

                                    table[x][y].hasShip = 0;
                                }
                            }

                            for (x = 0; x < 15; x++) {
                                for (y = 0; y < 15; y++) {
                                    visible_table[x][y] = ' ';
                                }
                            }

                            place_ships(5,1);
                            place_ships(4,2);
                            place_ships(3,2);
                            place_ships(2,3);
                            place_ships(1,3);

                            initialized = true;
                        }

                        /**
                         * FIRE command received
                         */
                        char *tok=strtok(received_message, " ");
                        if(strncmp(tok, "FIRE", 4)==0) {
                            if (getNext(i)) {
                                tok=strtok(NULL, " ");

                                int y=0, x=tok[0]-'A';
                                int y1=tok[1]-'0';
                                int y2=tok[2]-'0';
                                if(y2>=0 && y2<=9){
                                    y+=y1*10;
                                    y+=y2-1;
                                }
                                else
                                    y+=y1-1;
                                if(x>14 || y>14){
                                    send(i, "Coordinate is not correct!\n", sizeof "Coordinate is not correct!\n", 0);
                                    continue;
                                }
                                if(table[x][y].hasShip==1){

                                    visible_table[x][y]= 'X';
                                    if (i == first) {
                                        first_player_hits++;
                                    } else {
                                        second_player_hits++;
                                    }
                                }
                                else
                                    visible_table[x][y]= '-';
                                if(getNext(first)) {
                                    setNext(first, false);
                                    setNext(second, true);
                                } else {
                                    setNext(first, true);
                                    setNext(second, false);
                                }
                            }
                            else
                                send(i, "Not your turn!\n", 15, 0);

                            /**
                             * TABLE command received
                             */
                        } else if(strncmp("TABLE", tok, 5)==0){
                            int length = 0;
                            length += sprintf(send_message + length, "\n\n  ");
                            for (x = 0; x < 15; x++) {
                                length += sprintf(send_message + length, " %2d ", x+1);
                            }
                            length += sprintf(send_message + length, "\n  -");
                            for (x = 0; x < 15; x++) {
                                length += sprintf(send_message + length, "----");
                            }
                            length += sprintf(send_message + length, "\n");
                            for (x = 0; x < 15; x++) {
                                length += sprintf(send_message + length, "%c ", x+'A');
                                for (y = 0; y < 15; y++) {
                                    if (visible_table[x][y] == ' ') {
                                        length += sprintf(send_message + length, "|   ");
                                    } else {

                                        length += sprintf(send_message + length, "|%2c ", visible_table[x][y]);
                                    }
                                }

                                length += sprintf(send_message + length, "| %c\n", x+'A');
                            }
                            length += sprintf(send_message + length, "  -");
                            for(k=0;k<15;k++)
                                length += sprintf(send_message + length, "----");
                            length += sprintf(send_message + length, "\n ");
                            for(k=0;k<15;k++)
                                length += sprintf(send_message + length, " %2d ", k+1);
                            length += sprintf(send_message + length, "\n");

                            draw();

                            for (k = listener_socket + 1; k <= fdmax; k++) {
                                if (FD_ISSET(k, &master) && k == i) {
                                    send(k, send_message, sizeof send_message, 0);

                                }
                            }

                            /**
                              * RULES command received
                              */
                        } else if(strncmp("RULES", tok, 5)==0){
                            sendrules(i);


                            /**
                              * QUIT command received
                              */
                        } else if(strncmp("QUIT", tok, 4)==0){
                            bzero(send_message, sizeof(send_message));
                            sprintf(send_message, "Game over!\n");
                            send(first, send_message, strlen(send_message), 0);
                            send(second, send_message, strlen(send_message), 0);
                            close(first);
                            close(second);

                            del_client(first);
                            del_client(second);
                            FD_CLR(first, &master);
                            FD_CLR(second, &master);

                            first = 0;
                            second = 0;

                            cnum = 0;

                            print_clients();

                            max = 0;
                            for (j = listener_socket + 1; j <= fdmax; j++) {
                                if (FD_ISSET(j, &master) && j > max) {
                                    max = j;
                                }
                            }
                            fdmax = max;
                        }
                    } // end of something received
                } // end of i is not listener_socket
            } // end of if FD_ISSET
        } // end of big for
    } // end of infinite loop


    return 0;
}
