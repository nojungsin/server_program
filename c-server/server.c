#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define DATA_FILE "users.txt"

// 사용자 데이터를 파일에 저장하는 함수
void save_user_data(const char *username, const char *password) {
    FILE *file = fopen(DATA_FILE, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "%s %s\n", username, password);
    fclose(file);
}

// 회원가입 요청 처리
void handle_register(const char *body, SOCKET client_socket) {
    char username[50], password[50];
    char response_body[BUFFER_SIZE];
    char response_header[BUFFER_SIZE];

    if (sscanf(body, "{\"username\":\"%[^\"]\",\"password\":\"%[^\"]\"}", username, password) == 2) {
        save_user_data(username, password);

        snprintf(response_body, sizeof(response_body), "{\"message\":\"Registration success\"}");
        snprintf(response_header, sizeof(response_header),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %d\r\n"
                 "Access-Control-Allow-Origin: *\r\n"
                 "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                 "Access-Control-Allow-Headers: Content-Type\r\n"
                 "\r\n",
                 (int)strlen(response_body));

        send(client_socket, response_header, strlen(response_header), 0);
        send(client_socket, response_body, strlen(response_body), 0);
    } else {
        snprintf(response_body, sizeof(response_body), "{\"error\":\"Invalid request format\"}");
        snprintf(response_header, sizeof(response_header),
                 "HTTP/1.1 400 Bad Request\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %d\r\n"
                 "Access-Control-Allow-Origin: *\r\n"
                 "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                 "Access-Control-Allow-Headers: Content-Type\r\n"
                 "\r\n",
                 (int)strlen(response_body));

        send(client_socket, response_header, strlen(response_header), 0);
        send(client_socket, response_body, strlen(response_body), 0);
    }
}

// 로그인 요청 처리
void handle_login(const char *body, SOCKET client_socket) {
    char username[50], password[50], stored_username[50], stored_password[50];
    int authenticated = 0;

    // JSON 형식으로 데이터 파싱
    if (sscanf(body, "{\"username\":\"%[^\"]\",\"password\":\"%[^\"]\"}", username, password) == 2) {
        FILE *file = fopen(DATA_FILE, "r");
        if (file != NULL) {
            while (fscanf(file, "%s %s", stored_username, stored_password) != EOF) {
                if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
                    authenticated = 1;
                    break;
                }
            }
            fclose(file);
        }
    }

    char response_body[BUFFER_SIZE];
    char response_header[BUFFER_SIZE];
    if (authenticated) {
        // 성공 응답
        snprintf(response_body, sizeof(response_body), "{\"message\":\"Login success\"}");
        snprintf(response_header, sizeof(response_header),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %d\r\n"
                 "Access-Control-Allow-Origin: *\r\n"
                 "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                 "Access-Control-Allow-Headers: Content-Type\r\n"
                 "\r\n",
                 (int)strlen(response_body));
    } else {
        // 실패 응답
        snprintf(response_body, sizeof(response_body), "{\"error\":\"Invalid username or password\"}");
        snprintf(response_header, sizeof(response_header),
                 "HTTP/1.1 401 Unauthorized\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %d\r\n"
                 "Access-Control-Allow-Origin: *\r\n"
                 "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                 "Access-Control-Allow-Headers: Content-Type\r\n"
                 "\r\n",
                 (int)strlen(response_body));
    }

    send(client_socket, response_header, strlen(response_header), 0);
    send(client_socket, response_body, strlen(response_body), 0);
}



// OPTIONS 요청 처리
void handle_options(SOCKET client_socket) {
    const char *response =
        "HTTP/1.1 204 No Content\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "\r\n";
    send(client_socket, response, strlen(response), 0);
}
// 텍스트 메시지 처리
void handle_text_message(const char *body, SOCKET client_socket) {
    char message[BUFFER_SIZE];
    sscanf(body, "{\"message\":\"%[^\"]\"}", message);
    printf("Received message: %s\n", message);

    // 자동 응답 메시지
    char response_body[BUFFER_SIZE];
    snprintf(response_body, sizeof(response_body), "{\"response\":\"Server reply to: %s\"}", message);

    char response_header[BUFFER_SIZE];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "\r\n",
             (int)strlen(response_body));

    send(client_socket, response_header, strlen(response_header), 0);
    send(client_socket, response_body, strlen(response_body), 0);
}

// 클라이언트 요청 처리
void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size;

    read_size = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (read_size == SOCKET_ERROR) {
        printf("Error reading from socket\n");
        closesocket(client_socket);
        return;
    }

    buffer[read_size] = '\0';
    printf("Received request:\n%s\n", buffer);

    char method[10], path[50];
    sscanf(buffer, "%s %s", method, path);

    if (strcmp(method, "OPTIONS") == 0) {
        handle_options(client_socket);
    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/register") == 0) {
        char *body = strstr(buffer, "\r\n\r\n") + 4;
        handle_register(body, client_socket);
    } 
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/login") == 0) {
        char *body = strstr(buffer, "\r\n\r\n") + 4; // 요청 본문 추출
        handle_login(body, client_socket);
    }
    else {
        const char *response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Not Found";
        send(client_socket, response, strlen(response), 0);
    }

    closesocket(client_socket);
}

int main() {
    WSADATA wsa_data;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Winsock initialization failed\n");
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Could not create socket\n");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is running on http://localhost:%d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed\n");
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }

        handle_client(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
