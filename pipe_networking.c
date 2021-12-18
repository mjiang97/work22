#include "pipe_networking.h"

/*=========================
  server_setup
  args:
  creates the WKP (upstream) and opens it, waiting for a
  connection.
  removes the WKP once a connection has been made
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
    int from_client = 0;

    printf("making wkp...\n");
    mkfifo(WKP, 0600);
    from_client = open(WKP, O_RDONLY, 0);
    printf("removing wkp...\n");
    remove(WKP);
    return from_client;
}

/*=========================
  server_connect
  args: int from_client
  handles the subserver portion of the 3 way handshake
  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
    int to_client  = 0;
    char buff[HANDSHAKE_BUFFER_SIZE];

    read(from_client, buff, sizeof(buff));
    printf("message received...\n");
    to_client = open(buff, O_WRONLY, 0);

    srand(time(NULL));
    int random = rand() % sizeof(buff);
    sprintf(buff, "%d", random);
    write(to_client, buff, sizeof(buff));
    read(from_client, buff, sizeof(buff));

    int ra = atoi(buff);
    if (ra != random+1) {
        printf("bad handshake...\n");
        exit(0);
    }

    printf("handshaked...\n");
    return to_client;
}

/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    int from_client = 0;
    printf("creating wkp...\n");
    mkfifo(WKP, 0644);

    from_client = open(WKP, O_RDONLY, 0);

    printf("reading from_client...\n");
    char f[HANDSHAKE_BUFFER_SIZE];
    read(from_client, f, sizeof(f));
    remove(WKP); 
    *to_client = open(f, O_WRONLY, 0);

    srand(time(NULL));
    int random = rand() % sizeof(f);
    sprintf(f, "%d", random);

    printf("writing to client...\n");
    write(*to_client, f, sizeof(f));

    printf("receiving from client...\n");
    read(from_client, f, sizeof(f));

    int r = atoi(f);
    if(r != random+1) {
        printf("bad handshake...\n");
        exit(0);
    }
    printf("received handshake...\n");
    return from_client;
}

/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    int from_server = 0;
    char buff[HANDSHAKE_BUFFER_SIZE];
    printf("creating private pipe...\n");
    sprintf(buff, "%d", getpid());
    mkfifo(buff, 0644);

    printf("opening wkp...\n");
    *to_server = open(WKP, O_WRONLY, 0);

    printf("writing to server...\n");
    write(*to_server, buff, sizeof(buff));

    printf("receiving server message...\n");
    from_server = open(buff, O_RDONLY, 0);

    printf("reading server message...\n");
    read(from_server, buff, sizeof(buff));
    remove(buff);

    int random = atoi(buff) + 1;
    sprintf(buff, "%d", random);
    printf("sending response...%s\n", buff);
    write(*to_server, buff, sizeof(buff));

    return from_server;
}