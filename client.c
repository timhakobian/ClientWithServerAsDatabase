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


#define RIO_BUFSIZE 8192
typedef struct {
    int rio_fd;                /* descriptor for this internal buf */
    int rio_cnt;               /* unread bytes in internal buf */
    char *rio_bufptr;          /* next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE]; /* internal buffer */
} rio_t;

int open_clientfd(char*hostname, char*port) {
    int clientfd;
    struct addrinfo hints, *listp, *p;/* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype= SOCK_STREAM;  /* Open a connection */
    hints.ai_flags= AI_NUMERICSERV;  /* ...using numeric port arg. */
    hints.ai_flags|= AI_ADDRCONFIG;  /* Recommended for connections */
    getaddrinfo(hostname, port, &hints, &listp);

    /* Walk the list for one that we can successfully connect to */
    for(p = listp; p; p = p->ai_next) {
        /* Createa socketdescriptor*/
        if ((clientfd= socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue; /*   Socketfailed, trythe next*//* Connect to the server */
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
            break; /* Success */
        close(clientfd); /* Connect failed, try another */
    }
    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* All connects failed */
        return-1;
    else/* The last connect succeeded */
        return clientfd;
}

int main(int argc, char **argv){

    int clientfd;
    char *host, *port, buf[MAXLINE];

    rio_t rio;

    host = argv[1];
    port = argv[2];
    int o;
    int s;
    char* i;

    clientfd = open_clientfd(host, port);

    printf(">");
    i = fgets(buf, MAXLINE, stdin);

    while(strncmp(buf, "quit", 4) != 0) {
        unsigned char size = strlen(buf);
        write(clientfd, &size, 1);
        write(clientfd, buf, size);
        int rsize;
        read(clientfd, &rsize, 1);
        read(clientfd, buf, rsize);
        printf("%s\n", buf);
        printf(">");
        i = fgets(buf, MAXLINE, stdin);
    }
    close(clientfd);
    exit(0);
}