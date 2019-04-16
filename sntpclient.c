#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <sys/socket.h>
// #include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

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
    uint32_t txTm_s;                 // 32 bits and the most important field the client cares
        // about. Transmit time­stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time­stamp fraction of a second.
} ntp_packet;              // Total: 384 bits or 48 bytes.

int main(){

    struct sockaddr_in target;
    int conecta;   
    char target_ip[100];

    printf("Informe o IP de destino:\n");
    scanf("%s", target_ip);

    int mysocket = socket(AF_INET, SOCK_DGRAM, 0);
    target.sin_family = AF_INET;
    target.sin_port = htons(123);
    // target.sin_addr.s_addr = "192.168.0.1";
    // target.sin_addr.s_addr = inet_addr("200.189.40.8");
    target.sin_addr.s_addr = inet_addr(target_ip);

    conecta = connect(mysocket,(struct sockaddr *)&target,sizeof(target));
    if (conecta < 0) // Reposta -1 significa que houve erro ao conectar
        perror("ERROR connecting");
    else 
        printf("sucesso\n");

    // close(mysocket);
    // close(conecta);

    return 0;
}