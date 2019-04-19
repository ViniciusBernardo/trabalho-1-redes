#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define NTP_TIMESTAMP_DELTA 2208988800ull
#define PORT 123
#define BRT -3

typedef struct
{
    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                             // li.   Two bits.   Leap indicator.
                             // vn.   Three bits. Version number of the protocol.
                             // mode. Three bits. Client will pick mode 3 for client.
    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.
    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.
    uint32_t refTm_s;        // 32 bits. Reference time­stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time­stamp fraction of a second.
    uint32_t origTm_s;       // 32 bits. Originate time­stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time­stamp fraction of a second.
    uint32_t rxTm_s;         // 32 bits. Received time­stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time­stamp fraction of a second.
    uint32_t txTm_s;         // 32 bits and the most important field the client cares
                             // about. Transmit time­stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time­stamp fraction of a second.
} ntp_packet;                // Total: 384 bits or 48 bytes.

char *get_dia_semana(int dia);
char *get_mes(int mes);

int main(){

    struct sockaddr_in target;
    struct timeval timeout;
    ntp_packet datagram;
    int n=0, len = sizeof(target);
    char target_ip[100];
    char *dia, *mes;


    memset(&datagram, 0, sizeof(ntp_packet)); // Zera a string de 48 bytes
    datagram.li_vn_mode = 0x1b; // Seta o primeiro byte para 0x1b
    timeout.tv_sec = 20; // Seta timeout para 20000 ms (20 segundos)
    timeout.tv_usec = 0;

    printf("Informe o IP de destino:\n");
    scanf("%s", target_ip);

    // Abre e configura o socket
    int mysocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (mysocket < 0)
        perror("Erro ao abrir socket");

    // Configura timeout
    if (setsockopt(mysocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error");
    }

    // Define que utilizará o protocolo IPv4 da camada de rede,
    // a porta de comunicação com o servidor,
    // e o endereço de IP do servidor NTP (fornecido por linha de comando)
    target.sin_family = AF_INET;
    target.sin_port = htons(PORT);
    target.sin_addr.s_addr = inet_addr(target_ip);

    // Indica onde os datagramas serão enviados por padrão e o único endereço por
    // onde serão recebidos
    n = connect(mysocket,(struct sockaddr *)&target, sizeof(target));
    if (n < 0) // Reposta -1 significa que houve erro ao conectar
        perror("Erro ao conectar");

    // Envia a mensagem para a porta indicada
    n = sendto(mysocket, (char *) &datagram, sizeof(datagram),
        MSG_CONFIRM, (const struct sockaddr *) &target, sizeof(target));
    if (n < 0)
        perror("Erro ao enviar resposta");

    // Recebe a resposta
    n = recvfrom(mysocket, (char *) &datagram, sizeof(datagram),
        MSG_PEEK, (struct sockaddr *) &target, &len);           
    if (n < 0){
        printf("Primeiro timeout excedido\n");

        n = recvfrom(mysocket, (char *) &datagram, sizeof(datagram),
            MSG_PEEK, (struct sockaddr *) &target, &len);

        if(n < 0){
            printf("Data/hora: não foi possível contactar servidor\n");
            return 0;
        }
    }

    // Converte o dado do formato big-endian para little-endian
    datagram.txTm_s = ntohl(datagram.txTm_s);
    datagram.txTm_f = ntohl(datagram.txTm_f);

    time_t txTm = (time_t)(datagram.txTm_s - NTP_TIMESTAMP_DELTA);
    struct tm * result = localtime(&txTm);
    dia = get_dia_semana(result->tm_wday);
    mes = get_mes(result->tm_mon);


    printf("Data/hora: %s %s %d %d:%d:%d %d\n", dia, mes, result->tm_mday,
    result->tm_hour, result->tm_min, result->tm_sec, result->tm_year+1900);

    close(mysocket);

    return 0;
}

char *get_dia_semana(int dia){
    switch (dia)
    {
        case 0:
            return "Dom";
        case 1:
            return "Seg";
        case 2:
            return "Ter";
        case 3:
            return "Qua";
        case 4:
            return "Qui";
        case 5:
            return "Sex";
        case 6:
            return "Sab";
        default:
            return "Dia da semana inexistente";
    }
}

char *get_mes(int mes){
    switch (mes)
    {
        case 0:
            return "Jan";
        case 1:
            return "Fev";
        case 2:
            return "Mar";
        case 3:
            return "Abr";
        case 4:
            return "Mai";
        case 5:
            return "Jun";
        case 6:
            return "Jul";
        case 7:
            return "Ago";
        case 8:
            return "Set";
        case 9:
            return "Out";
        case 10:
            return "Nov";
        case 11:
            return "Dez";
        default:
            return "Mês inexistente";
    }
}
