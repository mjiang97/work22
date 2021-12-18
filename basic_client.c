#include "pipe_networking.h"

int main() {
    int to_server;
    int from_server;

    from_server = client_handshake( &to_server );
    char line[BUFFER_SIZE];
    while(1) {
        printf("input: ");
        fgets(line, sizeof(line), stdin);
        write(to_server, line, sizeof(line));
        read(from_server, line, sizeof(line));
        printf("response: %s", line);
    }
}