//Faaizah Saiddudin ; ID#23251827
//Timothy Hakobian ; ID#56197426
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>


#define MAXLINE 200

#define LISTENQ 5


#define RIO_BUFSIZE 8192
typedef struct {
    int rio_fd;                /* descriptor for this internal buf */
    int rio_cnt;               /* unread bytes in internal buf */
    char *rio_bufptr;          /* next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE]; /* internal buffer */
} rio_t;

typedef struct sockaddr SA;

int open_listenfd(char*port){
    struct addrinfo hints, *listp, *p;
    int listenfd, optval=1;/* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype= SOCK_STREAM;             /* Accept connect. */
    hints.ai_flags= AI_PASSIVE | AI_ADDRCONFIG; /* ...on any IP addr*/
    hints.ai_flags|= AI_NUMERICSERV;            /* ...using port no. */
    getaddrinfo(NULL, port, &hints, &listp);
    /* Walk the list for one that we can bind to */
    for(p = listp; p; p = p->ai_next) {/* Createa socketdescriptor*/
        if ((listenfd= socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;  /* Socketfailed, trythe next*/
        /* Eliminates"Address alreadyin use" errorfrom bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));/* Bindthe descriptorto the address*/
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        close(listenfd); /* Bind failed, try the next */
    }
    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return-1;/* Make it a listening socket ready to accept conn. requests */
    if (listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
        return-1;
    }
    return listenfd;
}

int main(int argc, char **argv){
    struct data {
        char type[20];
        int game_id;
        char home_team[20];
        char away_team[20];
        int week;
        int season;
        int home_score;
        int away_score;
    };

    struct data* dataBase = malloc(269 * (sizeof(struct data)));

    char* file = argv[1];

    FILE *fptr;
    fptr = fopen(file, "r");
    char fileline[120];
    int index = 0;

    while (fgets(fileline, 120, fptr)) {
        if(index != 0) {
            strcpy(dataBase[index].type, strtok(fileline, ","));
            dataBase[index].game_id = atoi(strtok(NULL, ","));
            strcpy(dataBase[index].home_team, strtok(NULL, ","));
            strcpy(dataBase[index].away_team, strtok(NULL, ","));
            dataBase[index].week = atoi(strtok(NULL, ","));
            dataBase[index].season = atoi(strtok(NULL, ","));
            dataBase[index].home_score = atoi(strtok(NULL, ","));
            dataBase[index].away_score = atoi(strtok(NULL, "\n"));
        }
        index++;
    }
    printf("server started\n");

    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough room for any addr*/
    char client_hostname[MAXLINE], client_port[MAXLINE];

    listenfd = open_listenfd(argv[2]);

    while(1) {
        clientlen= sizeof(struct sockaddr_storage); /* Important! */
        connfd = accept(listenfd, (SA*) &clientaddr, &clientlen);
        getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        //printf("Connected to (%s, %s)\n", client_hostname, client_port);

        size_t n;
        char buf[MAXLINE];
        while(1) {
            int size;
            if(read(connfd, &size, 1) > 0) {
                read(connfd, buf, size);

                printf("%s", buf);

                char *temp = strtok(buf, " ");
                int gameId = atoi(temp);
                char *entry = strtok(NULL, "\n");

                int i = 0;
                while (i < index) {
                    if (dataBase[i].game_id == gameId) {
                        if (strncmp(entry, "type", 4) == 0) {
                            unsigned char size = strlen(dataBase[i].type) + 1;
                            write(connfd, &size, 1);
                            write(connfd, dataBase[i].type, size);
                        } else if (strncmp(entry, "game_id", 7) == 0) {
                            //printf("FOUND1\n");
                            char t[10];
                            sprintf(t, "%d", dataBase[i].game_id);
                            unsigned char size = strlen(t) + 1;
                            write(connfd, &size, 1);
                            write(connfd, t, size);
                        } else if (strncmp(entry, "home_team", 9) == 0) {
                            unsigned char size = strlen(dataBase[i].home_team) + 1;
                            write(connfd, &size, 1);
                            write(connfd, dataBase[i].home_team, size);
                        } else if (strncmp(entry, "away_team", 9) == 0) {
                            unsigned char size = strlen(dataBase[i].away_team) + 1;
                            write(connfd, &size, 1);
                            write(connfd, dataBase[i].away_team, size);
                        } else if (strncmp(entry, "week", 4) == 0) {
                            char t[10];
                            sprintf(t, "%d", dataBase[i].week);
                            unsigned char size = strlen(t) + 1;
                            write(connfd, &size, 1);
                            write(connfd, t, size);
                        } else if (strncmp(entry, "season", 6) == 0) {
                            char t[10];
                            sprintf(t, "%d", dataBase[i].season);
                            unsigned char size = strlen(t) + 1;
                            write(connfd, &size, 1);
                            write(connfd, t, size);
                        } else if (strncmp(entry, "home_score", 10) == 0) {
                            char t[10];
                            sprintf(t, "%d", dataBase[i].home_score);
                            unsigned char size = strlen(t) + 1;
                            write(connfd, &size, 1);
                            write(connfd, t, size);
                        } else if (strncmp(entry, "away_score", 10) == 0) {
                            char t[10];
                            sprintf(t, "%d", dataBase[i].away_score);
                            unsigned char size = strlen(t) + 1;
                            write(connfd, &size, 1);
                            write(connfd, t, size);
                        } else {
                            unsigned char size = strlen("unknown") + 1;
                            write(connfd, &size, 1);
                            write(connfd, "unknown", size);
                        }
                    }
                    i++;
                }
            }
            else
                break;
        }

        close(connfd);
    }
    exit(0);
}