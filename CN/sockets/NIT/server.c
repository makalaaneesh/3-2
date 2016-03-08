/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "includes.h"
#include <sys/select.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


struct services{
    int s1;
    int s2;
};
int main(int argc, char *argv[])
{
     int sfd[20],nsfd,n,portno[20];
     char *lower_args[] = {"./lower", NULL};
     char *upper_args[] = {"./upper", NULL};

     fd_set readset;
     FD_ZERO(&readset);
     struct timeval timeptr;
     timeptr.tv_sec=0;
     timeptr.tv_usec=0;
     int max=0;
     
     socklen_t clientn;
     struct sockaddr_in myaddr,clientaddr;
     int i=0;
     for(i=0;i<argc-1;i++)
    {
        portno[i]=atoi(argv[i+1]);
        sfd[i]=socket(AF_INET,SOCK_STREAM,0);
         if(sfd[i]<0)
            error("Socket not initilizated");
        bzero((char *)&myaddr,sizeof(myaddr));
        myaddr.sin_family=AF_INET;
        myaddr.sin_addr.s_addr=INADDR_ANY;
        myaddr.sin_port=htons(portno[i]);
        if((bind(sfd[i],(struct sockaddr *) &myaddr, sizeof(myaddr)))<0)
            error("Error on binding: ");
        listen(sfd[i],5);
    }
    for(i=0;i<argc-1;i++)
    {
        FD_SET(sfd[i],&readset);
        if(sfd[i] > max)
            max=sfd[i];
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
    shm->s1=3;
    shm->s2=4;
    printf("\nSelect socket fds are created................");
    printf("\nServer started..................");
    clientn=sizeof(clientaddr);
    char buffer[250];
    bzero(buffer,250);
    int pid=1;
    while(1 && pid > 0)
    {
        int retstatus=select(max+1,&readset,NULL,NULL,&timeptr);
        if(retstatus > 0)
        {
            // printf("Got message\n");
            for(i=0;i<argc-1;i++)
            {
                if(FD_ISSET(sfd[i],&readset))
                {
                    if(i+1 == 1 && shm->s1 <=0)
                        continue;
                    else if(i+1 == 2 && shm->s2 <= 0)
                        continue;
                    nsfd=accept(sfd[i],(struct sockaddr *) &clientaddr, &clientn);
                    int pid=fork();
                    if(pid == 0){
                        close(sfd[i]);
                        bzero(buffer,250);
                        dup2(nsfd,0);
                        dup2(nsfd,1);
                        close(nsfd);
                        if(i+1 == 1)
                        {
                            shm->s1--;
                            printf("Exceing lower\n");
                            execv("./lower",lower_args);
                            printf("Failed\n");
                        }
                        else
                        {
                            shm->s2--;
                            printf("Exceing Upper\n");
                            execv("./upper",upper_args);
                             printf("Failed\n");
                        }
                    }
                    else if(pid > 0){
                        close(nsfd);
                    }
                }
            }
        }
        fflush(stdout); 
        for(i=0;i<argc-1;i++)
        {
            FD_SET(sfd[i],&readset);
            if(sfd[i] > max)
                max=sfd[i];
        }
    }
}