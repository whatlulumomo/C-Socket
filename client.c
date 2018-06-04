/*socket tcp客户端*/  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <errno.h>  
#include <netdb.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
  
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <unistd.h>  
  
#define SERVER_PORT 5555  

typedef struct Server{
    int clientSocket;
    char* sendbuf;
}*ServerInfo;

int Status = 0;

void* SendPthread(void* arg){
    while(1){
        char recvbuf[200];  
        ServerInfo Info = (ServerInfo)arg;
        int iDataNum = recv(Info->clientSocket, recvbuf, 200, 0);  
        recvbuf[iDataNum] = '\0';  
        printf("\nServer: %s\n", recvbuf);  
        if(Status == 0){
            break;
        }
    }
}
  
/* 
 连接到服务器后，会不停循环，等待输入， 
 输入quit后，断开与服务器的连接 
 */  


struct Server Info;

int connectServer()
{  
    Status = 1;
    //客户端只需要一个套接字文件描述符，用于和服务器通信  
    int clientSocket;  
    //描述服务器的socket  
    struct sockaddr_in serverAddr;  
    char sendbuf[200];  
    char recvbuf[200];  
    int iDataNum;  
    if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
    {  
        perror("socket");  
        return 1;  
    }  
  
    serverAddr.sin_family = AF_INET;  
    serverAddr.sin_port = htons(SERVER_PORT);  
    //指定服务器端的ip，本地测试：127.0.0.1  
    //inet_addr()函数，将点分十进制IP转换成网络字节序IP  
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  
    if(connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)  
    {  
        perror("connect");  
        return 1;  
    }  
  
    printf("connect with destination host...\n");  

    Info.sendbuf = sendbuf;
    Info.clientSocket = clientSocket;

    pid_t pid;  
    int temp;
    if((temp=pthread_create(&pid,NULL,SendPthread,(void *)&Info)))  
    {  
        printf("can't create thread: %s\n",strerror(temp));  
        return 1;  
    }  

    while(1)  
    {  
        printf("Input:>");  
        scanf("%s", sendbuf);  
        printf("\n");  
        
        if(strcmp(sendbuf, "quit") == 0){
            send(clientSocket, sendbuf, strlen(sendbuf), 0);  
            Status = 0;
            break;
        } 
        send(clientSocket, sendbuf, strlen(sendbuf), 0);  
    }  
    close(clientSocket);  
    return 0;  
}  

int main(){
    int ch;
    printf("1 - connect\n2 - quit\n");
    scanf("%d",&ch);
    while(ch != 2){
        if(ch == 1){
            connectServer();
        }
        printf("1 - connect\n2 - quit\n");
        scanf("%d",&ch);
    }
}
