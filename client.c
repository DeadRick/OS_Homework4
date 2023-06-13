#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8888

typedef struct {
    int items;
    int value;
} StolenGoods;

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Создание сокета
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Преобразование IP-адреса из текстового вида в бинарный
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr)) <= 0) {
        perror("Ошибка преобразования IP-адреса");
        exit(EXIT_FAILURE);
    }

    // Главный цикл клиента
    while (1) {
        // Отправка данных на сервер
        if (sendto(clientSocket, "Data from client", strlen("Data from client"), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Ошибка отправки данных");
            exit(EXIT_FAILURE);
        }

        // Получение данных от сервера
        memset(buffer, 0, sizeof(buffer));
        socklen_t serverAddrLen = sizeof(serverAddr);
        int bytesGet = recvfrom(clientSocket, buffer, 1024, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        
        if (bytesGet == 0) {
            printf("На складе украли всё, что можно.\n");
            break;
        }

        printf("Клиент: вор украл со склада %s$\n", buffer);

        // Проверка условия завершения
        if (strcmp(buffer, "Сервер: все предметы похищены") == 0) {
            printf("Клиент: все предметы похищены, завершение работы\n");
            break;
        }

        // Задержка перед следующей итерацией
        sleep(1);
    }

    // Закрытие сокета клиента
    close(clientSocket);

    return 0;
}
