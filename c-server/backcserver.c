#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// 메시지 브로드캐스트 함수
void broadcast_message(const char *message, int sender_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] != sender_sock) {
            if (send(client_sockets[i], message, strlen(message), 0) < 0) {
                perror("Failed to send message");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// 클라이언트 처리 스레드
void *handle_client(void *socket_desc) {
    int client_sock = *(int *)socket_desc;
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("Received from client %d: %s\n", client_sock, buffer);

        // 메시지 브로드캐스트
        broadcast_message(buffer, client_sock);
    }

    if (read_size == 0) {
        printf("Client %d disconnected\n", client_sock);
    } else if (read_size == -1) {
        perror("recv failed");
    }

    // 클라이언트 소켓 제거
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == client_sock) {
            client_sockets[i] = client_sockets[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(client_sock);
    free(socket_desc);
    return NULL;
}

int main() {
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

    // 소켓 생성
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Could not create socket");
        return 1;
    }
    printf("Socket created\n");

    // 서버 설정
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }
    printf("Bind done\n");

    listen(server_sock, 3);
    printf("Waiting for incoming connections...\n");

    c = sizeof(struct sockaddr_in);

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t *)&c))) {
        printf("Client connected: %d\n", client_sock);

        pthread_mutex_lock(&clients_mutex);
        client_sockets[client_count++] = client_sock;
        pthread_mutex_unlock(&clients_mutex);

        pthread_t client_thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        if (pthread_create(&client_thread, NULL, handle_client, (void *)new_sock) < 0) {
            perror("Could not create thread");
            return 1;
        }
    }

    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    return 0;
}
