#include "includes.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

struct services{
    int s1;
    int s2;
};

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    int service=0;
    int shmid1;
    key_t key1;
    key1=300;
    int size=10000;
    struct services *shm;
    if ((shmid1 = shmget (key1, size, IPC_CREAT | 0666)) == -1) {
       perror("shmget: shmget failed"); exit(1); } else {
       (void) fprintf(stderr, "shmget: shmget returned %d\n", shmid1);
    }
    shm = (struct services *)shmat(shmid1, NULL, 0);


    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Send service number:\n1.Lower\n2.Upper\n");
    while(1){
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        if(strcmp(buffer,"1\n")==0 && shm->s1 <=0)
        {
            printf("No service 1 avialiable\n");
            break;
        }
        else if(strcmp(buffer,"2\n")==0 && shm->s2 <=0)
        {
            printf("No service 2 avialiable\n");
            break;
        }
        else if(strcmp(buffer,"2\n")==0)
            service=2;
        else
            service=1;
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             error("ERROR writing to socket");
        if(strcmp(buffer,"exit\n") ==0)
        {
            if(service == 1)
                shm->s1++;
            else
                shm->s2++;
            break;
        }
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n > 0) 
            printf("%s\n",buffer);
    }
    close(sockfd);
    return 0;
}