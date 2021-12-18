#include "pipe_networking.h"

char *toUpper(char line[]) {
    char * input = line;
    int i = 0;
    // process input str into all caps
    for (i = 0; i < strlen(line); i++) {
        line[i] = toupper(line[i]);
    }
    return line;
}

int main() {
    int to_client;
    int from_client;

    while(1) {
        from_client = server_setup();
        int f = fork();
        if(f) {
            close(from_client);
        } else {
            to_client = server_connect(from_client);
            while(1) {
                char message[BUFFER_SIZE];
                int r = read(from_client, message, sizeof(message));
                if(r==0) break;
                toUpper(message);
                write(to_client, message, sizeof(message));
            }
        }
    }
}