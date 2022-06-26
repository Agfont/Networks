/* Por Prof. Daniel Batista <batista@ime.usp.br>
 * Em 4/4/2021
 * 
 * Um código simples de um servidor de eco a ser usado como base para
 * o EP1. Ele recebe uma linha de um cliente e devolve a mesma linha.
 * Teste ele assim depois de compilar:
 * 
 * ./ep1-servidor-exemplo 8000
 * 
 * Com este comando o servidor ficará escutando por conexões na porta
 * 8000 TCP (Se você quiser fazer o servidor escutar em uma porta
 * menor que 1024 você precisará ser root ou ter as permissões
 * necessáfias para rodar o código com 'sudo').
 *
 * Depois conecte no servidor via telnet. Rode em outro terminal:
 * 
 * telnet 127.0.0.1 8000
 * 
 * Escreva sequências de caracteres seguidas de ENTER. Você verá que o
 * telnet exibe a mesma linha em seguida. Esta repetição da linha é
 * enviada pelo servidor. O servidor também exibe no terminal onde ele
 * estiver rodando as linhas enviadas pelos clientes.
 * 
 * Obs.: Você pode conectar no servidor remotamente também. Basta
 * saber o endereço IP remoto da máquina onde o servidor está rodando
 * e não pode haver nenhum firewall no meio do caminho bloqueando
 * conexões na porta escolhida.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>    // open() flags
#include <sys/stat.h> // stat() to retrieve file information

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

/* MQTT Control Packet Types */
#define MQTT_CONTROL_CONNECT 0x10 
#define MQTT_CONTROL_CONNACK 0x20 
#define MQTT_CONTROL_PUBLISH 0x30 
#define MQTT_CONTROL_PUBACK 0x40
#define MQTT_CONTROL_PUBREC 0x50 
#define MQTT_CONTROL_PUBREL 0x60 
#define MQTT_CONTROL_PUBCOMP 0x70 
#define MQTT_CONTROL_SUBSCRIBE 0x82
#define MQTT_CONTROL_SUBACK 0x90 
#define MQTT_CONTROL_UNSUBSCRIBE 0xA0 
#define MQTT_CONTROL_UNSUBACK 0xB0 
#define MQTT_CONTROL_PINGREQ 0xC0
#define MQTT_CONTROL_PINGRESP 0xD0
#define MQTT_CONTROL_DISCONNECT 0xE0

/* CONNACK Return Codes */
#define MQTT_CONNACK_ACCEPTED 0x00

/* MQTT Fixed Header */
struct mqtt_fixed_header {
    uint8_t packet_type;
    uint8_t remaining_length;
};

/* CONNACK Headers */
struct connack_variable_header {
    uint8_t flags;
    uint8_t return_code;
};

/* MQTTv5
struct connack_properties_mqttv5 {
    uint8_t total_length;
    uint8_t topic_alias_id;
    uint16_t value;
    uint8_t client_id;
    uint16_t string_length;
};
struct connack_mqttv5 {
    char value[MAXDATASIZE+1];
}; */

/* SUBSCRIBE Headers */
struct subscribe_topic_length {
    uint8_t topic_length_msb;
    uint8_t topic_length_lsb;
};
struct subscribe_variable_header {
    uint8_t message_id_msb;
    uint8_t message_id_lsb;
    uint8_t topic_length_msb;
    uint8_t topic_length_lsb;
};
struct subscribe_payload {
     char topic[MAXDATASIZE+1];
};

/* SUBACK Headers */
struct suback_variable_header {
    uint8_t packet_id_msb;
    uint8_t packet_id_lsb;
};
struct suback_payload {
    uint8_t reason_code;
};

/* PUBLISH Headers */
struct publish_variable_header {
    uint8_t topic_length_msb;
    uint8_t topic_length_lsb;
};
struct publish_topic {
    char topic[MAXDATASIZE+1];
};
struct publish_payload {
    char message[MAXDATASIZE+1];
};

int file_is_modified(const char *path, time_t oldMTime) {
    struct stat file_stat;
    int err = stat(path, &file_stat);
    if (err != 0) {
        perror(" [file_is_modified] stat");
        exit(errno);
    }
    return file_stat.st_mtime > oldMTime;
}

int main (int argc, char **argv) {
    /* Os sockets. Um que será o socket que vai escutar pelas conexões
     * e o outro que vai ser o socket específico de cada conexão */
    int listenfd, connfd;
    /* Informações sobre o socket (endereço e porta) ficam nesta struct */
    struct sockaddr_in servaddr;
    /* Retorno da função fork para saber quem é o processo filho e
     * quem é o processo pai */
    pid_t childpid;
    /* Armazena linhas recebidas do cliente */
    char recvline[MAXLINE + 1];
    /* Armazena o tamanho da string lida do cliente */
    ssize_t n;

    /* Store published messages */
    char subline[MAXDATASIZE + 1];;
    /* File descriptor used to read and write on topic's files */
    int fd;

    if (argc != 2) {
        fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
        fprintf(stderr,"Vai rodar um servidor de echo na porta <Porta> TCP\n");
        exit(1);
    }

    /* Criação de um socket. É como se fosse um descritor de arquivo.
     * É possível fazer operações como read, write e close. Neste caso o
     * socket criado é um socket IPv4 (por causa do AF_INET), que vai
     * usar TCP (por causa do SOCK_STREAM), já que o MQTT funciona sobre
     * TCP, e será usado para uma aplicação convencional sobre a Internet
     * (por causa do número 0) */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket :(\n");
        exit(2);
    }

    /* Agora é necessário informar os endereços associados a este
     * socket. É necessário informar o endereço / interface e a porta,
     * pois mais adiante o socket ficará esperando conexões nesta porta
     * e neste(s) endereços. Para isso é necessário preencher a struct
     * servaddr. É necessário colocar lá o tipo de socket (No nosso
     * caso AF_INET porque é IPv4), em qual endereço / interface serão
     * esperadas conexões (Neste caso em qualquer uma -- INADDR_ANY) e
     * qual a porta. Neste caso será a porta que foi passada como
     * argumento no shell (atoi(argv[1]))
     */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(atoi(argv[1]));
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind :(\n");
        exit(3);
    }

    /* Como este código é o código de um servidor, o socket será um
     * socket passivo. Para isto é necessário chamar a função listen
     * que define que este é um socket de servidor que ficará esperando
     * por conexões nos endereços definidos na função bind. */
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen :(\n");
        exit(4);
    }
    //printf("PID Broker: %ld, %ld\n", (long)getpid(), (long)getppid());
    printf("[Servidor no ar. Aguardando conexões na porta %s]\n",argv[1]);
    printf("[Para finalizar, pressione CTRL+c ou rode um kill ou killall]\n"); 
   
    /* O servidor no final das contas é um loop infinito de espera por
     * conexões e processamento de cada uma individualmente */
	for (;;) {
        /* O socket inicial que foi criado é o socket que vai aguardar
         * pela conexão na porta especificada. Mas pode ser que existam
         * diversos clientes conectando no servidor. Por isso deve-se
         * utilizar a função accept. Esta função vai retirar uma conexão
         * da fila de conexões que foram aceitas no socket listenfd e
         * vai criar um socket específico para esta conexão. O descritor
         * deste novo socket é o retorno da função accept. */
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept :(\n");
            exit(5);
        }
        /* Agora o servidor precisa tratar este cliente de forma
         * separada. Para isto é criado um processo filho usando a
         * função fork. O processo vai ser uma cópia deste. Depois da
         * função fork, os dois processos (pai e filho) estarão no mesmo
         * ponto do código, mas cada um terá um PID diferente. Assim é
         * possível diferenciar o que cada processo terá que fazer. O
         * filho tem que processar a requisição do cliente. O pai tem
         * que voltar no loop para continuar aceitando novas conexões.
         * Se o retorno da função fork for zero, é porque está no
         * processo filho. */
        if ( (childpid = fork()) == 0) {
            /**** PROCESSO FILHO ****/
            printf("[Uma conexão aberta]\n");
            /* Já que está no processo filho, não precisa mais do socket
             * listenfd. Só o processo pai precisa deste socket. */
            close(listenfd);

            /* Agora pode ler do socket e escrever no socket. Isto tem
             * que ser feito em sincronia com o cliente. Não faz sentido
             * ler sem ter o que ler. Ou seja, neste caso está sendo
             * considerado que o cliente vai enviar algo para o servidor.
             * O servidor vai processar o que tiver sido enviado e vai
             * enviar uma resposta para o cliente (Que precisará estar
             * esperando por esta resposta) 
             */

            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 INÍCIO                        */
            /* ========================================================= */
            /* ========================================================= */
            /* TODO: É esta parte do código que terá que ser modificada
             * para que este servidor consiga interpretar comandos MQTT  */
            while ((n=read(connfd, recvline, MAXLINE)) > 0) {
                recvline[n]=0;
                printf("[Cliente conectado no processo filho %d]: ",getpid());
                if ((fputs(recvline,stdout)) == EOF) {
                    perror("fputs :( \n");
                    exit(6);
                }
                /* DEBUG
                char str [MAXLINE+1];
                memcpy(str, &recvline[2], MAXLINE+1);
                for(int i=0;i<40+1;i++)
                    printf("array[%d]: %hhx || tmp[%d]: %hhx\n", i, recvline[i], i, str[i]);*/

                struct mqtt_fixed_header *fixed_header = malloc(sizeof(struct mqtt_fixed_header));
                memcpy(fixed_header, recvline, sizeof(struct mqtt_fixed_header));
                printf("\n- NEW Packet type received: 0x%hhx\n", fixed_header->packet_type);

                switch (fixed_header->packet_type ) {
                    case MQTT_CONTROL_CONNECT: {
                        /* CONNACK*/
                        printf("Connect request to Server!\n");

                        /* CONNACK Implementation*/
                        printf("CONNACK details:\n");
                        struct connack_variable_header *var_header = malloc(sizeof(struct connack_variable_header));
                        fixed_header->packet_type = MQTT_CONTROL_CONNACK;
                        fixed_header->remaining_length = sizeof(struct connack_variable_header);
                        var_header->flags = 0x00;
                        var_header->return_code = MQTT_CONNACK_ACCEPTED;

                        write(connfd, fixed_header, sizeof(struct mqtt_fixed_header));
                        write(connfd, var_header, sizeof(struct connack_variable_header));

                        printf("----- Control Packet sent: 0x%hhx\n", fixed_header->packet_type);
                        printf("----- Remaining_length: %d\n", fixed_header->remaining_length);
                        printf("----- Flags: %d\n", var_header->flags);
                        printf("----- Return Code: %d\n", var_header->return_code);
                        
                        free(var_header);
                        break;
                    }
                    case MQTT_CONTROL_SUBSCRIBE: {
                        /* SUBCRIBE */
                        printf("Subscribe request to Server!\n");

                        /* SUBACK implementation */
                        printf("SUBACK details:\n");
                        struct suback_variable_header *suback_var_header = malloc(sizeof(struct suback_variable_header));
                        struct suback_payload *suback_payload = malloc(sizeof(struct suback_payload));

                        fixed_header->packet_type = MQTT_CONTROL_SUBACK;
                        fixed_header->remaining_length = 0x03;
                        suback_var_header->packet_id_msb = 0x00;
                        suback_var_header->packet_id_lsb = 0x01; 
                        suback_payload->reason_code = 0x00;
                        write(connfd, fixed_header, sizeof(struct mqtt_fixed_header));
                        write(connfd, suback_var_header, sizeof(struct suback_variable_header));
                        write(connfd, suback_payload, sizeof(struct suback_payload));

                        printf("----- Control Packet sent: 0x%hhx\n", fixed_header->packet_type);
                        printf("----- Remaining_length: %d\n", fixed_header->remaining_length);
                        printf("----- Packet ID: %d\n", suback_var_header->packet_id_msb << 8 | suback_var_header->packet_id_lsb);
                        printf("----- Reason Code: %d\n", suback_payload->reason_code);
 
                        free(suback_var_header);
                        free(suback_payload);

                        /* SUBSCRIBE implementation */
                        printf("SUBSCRIBE details:\n");
                        int it = sizeof(struct mqtt_fixed_header);
                        struct subscribe_variable_header *var_header = malloc(sizeof(struct subscribe_variable_header));
                        memcpy(var_header, &recvline[it], sizeof(struct subscribe_variable_header));
                        uint16_t message_id = var_header->message_id_msb << 8 | var_header->message_id_lsb;
                        printf("----- Message ID: %d\n", message_id);

                        it += sizeof(message_id);
                        struct subscribe_topic_length *sub_topic_len = malloc(sizeof(struct subscribe_topic_length));
                        memcpy(sub_topic_len, &recvline[it], sizeof(struct subscribe_topic_length));
                        uint16_t topic_length = sub_topic_len->topic_length_msb << 8 | sub_topic_len->topic_length_lsb;
                        printf("----- Topic Length: %d\n", topic_length);
                        
                        it += sizeof(topic_length);
                        struct subscribe_payload *payload = malloc(topic_length);
                        memcpy(payload, &recvline[it], topic_length);

                        uint8_t QoS_byte = payload->topic[topic_length];
                        payload->topic[topic_length] = 0;
                        printf("----- Topic: %s\n", payload->topic);
                        printf("----- Requested QoS: %d\n", QoS_byte);

                        free(var_header);
                        free(sub_topic_len);

                        struct stat file_stat;
                        time_t cur_time;
                        time(&cur_time);
                        uint8_t num_bytes;
                        int ret;
                        ret = fork();
                        if (ret == 0) { // Child process
                            while (1) {
                                sleep(1); // Brief optimization
                                /* Check if file exists and the user has permission to read */
                                if(access(payload->topic, F_OK | R_OK)  == 0) {
                                    //printf("========= File Exists and is ready to read =========\n");
                                    int err = stat(payload->topic, &file_stat);
                                    if (err != 0) {
                                        perror("[file_is_modified] stat");
                                        exit(errno);
                                    }
                                    /* Only read the file when changes ocurr */
                                    int last_modified = file_stat.st_mtime;
                                    if (last_modified > cur_time) {
                                        //printf("****** Modification detected on file ******\n");
                                        fd = open(payload->topic, O_RDONLY);
                                        read(fd, subline, 1);
                                        num_bytes = subline[0];
                                        //printf("****** Bytes received: %d ******\n", num_bytes);
                                        num_bytes -= 1; 
                                        read(fd, &subline[1], num_bytes);;
                                        close(fd);
                                        
                                        write(connfd, &subline[1], num_bytes);
                                        cur_time = last_modified;
                                    }
                                }
                            }
                        }
                        free(payload);
                        break;
                    }
                    case MQTT_CONTROL_PUBLISH: {
                        /* PUBLISH */
                        printf("Publish request to Server!\n");
                        printf("----- Remaining length: %d\n", fixed_header->remaining_length);
                        int it = sizeof(struct mqtt_fixed_header);

                        struct publish_variable_header *var_header = malloc(sizeof(struct publish_variable_header));
                        memcpy(var_header, &recvline[it], sizeof(struct publish_variable_header));
                        uint16_t topic_length = var_header->topic_length_msb << 8 | var_header->topic_length_lsb;
                        printf("----- Topic length: %d\n", topic_length);

                        it += sizeof(topic_length);
                        struct publish_topic *pub_topic = malloc(sizeof(struct publish_topic));
                        memcpy(pub_topic, &recvline[it], sizeof(struct publish_topic));

                        uint8_t msg_first_byte = pub_topic->topic[topic_length];
                        pub_topic->topic[topic_length] = 0;
                        printf("----- Topic: %s\n", pub_topic->topic);
                        
                        it += topic_length;
                        struct publish_payload *payload = malloc(sizeof(struct publish_payload));
                        memcpy(payload, &recvline[it], sizeof(struct publish_payload));
                        uint16_t message_length = fixed_header->remaining_length - topic_length - sizeof(struct mqtt_fixed_header);
                        payload->message[message_length] = 0;
                        printf("----- Message: %s\n", payload->message);
                        
                        /* Communication to subscribers through files*/
                        fd = open(pub_topic->topic, O_WRONLY | O_CREAT | O_TRUNC, // O_APPEND (to not overwrite)
                                                    S_IRWXU | S_IRGRP | S_IROTH);
                        pub_topic->topic[topic_length] = msg_first_byte;
                        uint8_t num_bytes[1];
                        num_bytes[0] = sizeof(struct mqtt_fixed_header) + sizeof(struct publish_variable_header)
                                            + topic_length + message_length + 1;
                        write(fd, &num_bytes, sizeof(num_bytes));
                        write(fd, fixed_header, sizeof(struct mqtt_fixed_header));
                        write(fd, var_header, sizeof(struct publish_variable_header));
                        write(fd, pub_topic, topic_length);
                        write(fd, payload, message_length);
                        close(fd);

                        pub_topic->topic[topic_length] = 0;
                        // printf("----- Bytes sent to file '%s': %d\n", pub_topic->topic, num_bytes[0]);

                        free(var_header);
                        free(pub_topic);
                        free(payload);
                        break;
                    }
                    case MQTT_CONTROL_PINGREQ: {
                        /* PINGREQ */
                        printf("Ping request to Server!\n");

                        /* PINGRESP Implementation */
                        printf("PINGRESP details:\n");
                        fixed_header->packet_type = MQTT_CONTROL_PINGRESP;
                        fixed_header->remaining_length = 0x00;
                        write(connfd, fixed_header, sizeof(struct mqtt_fixed_header));
                        printf("----- Control Packet sent: 0x%hhx\n", fixed_header->packet_type);
                        break;
                    }
                    case MQTT_CONTROL_DISCONNECT: {
                        /* DISCONNECT */
                        printf("Disconnect request to Server!\n");
                        break;
                    }
                }
                free(fixed_header);     
                fflush(stdout);
            }
            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 FIM                           */
            /* ========================================================= */
            /* ========================================================= */

            /* Após ter feito toda a troca de informação com o cliente,
             * pode finalizar o processo filho */
            printf("[Uma conexão fechada]\n");
            exit(0);
        }
        else
            /**** PROCESSO PAI ****/
            /* Se for o pai, a única coisa a ser feita é fechar o socket
             * connfd (ele é o socket do cliente específico que será tratado
             * pelo processo filho) */
            close(connfd);
    }
    exit(0);
}
