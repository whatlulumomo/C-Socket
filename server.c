/*socket tcp服务器端*/  
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
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

  
#define SERVER_PORT 5555  
#define MAX_USER 10
#define INFOLENGTH 300

typedef struct client{
    int serverSocket;
    int iDataNum;
    int client;
} *ClientInfo;

struct user{
    char ID;
    char IP[INFOLENGTH];
    int Port;
    int active;
};

struct user User[MAX_USER];
int usercount = 0;

void AddUser(char* IP, char* port){
    for(int i=0;i<MAX_USER;i++){
        if(User[i].active == 0){
            User[i].active = 1;
            strcpy(User[i].IP, IP);
            User[i].Port = port;
            break;
        }
    }
}

void PrintUser(char* reply){
    strcpy(reply, "User List:\n");
    for(int i=0;i<MAX_USER;i++){
        if(User[i].active == 1){
            //printf("IP: %s, Port: %d\n", User[i].IP, User[i].Port);
            strcat(reply, "ID: ");
            char t[1];
            t[0] = User[i].ID;
            strcat(reply, t);
            strcat(reply, " IP: ");
            strcat(reply, User[i].IP);
            strcat(reply, ", Port: ");
            char port[10];
            sprintf(port, "%d", User[i].Port);
            strcat(reply, port);
            strcat(reply, "\n");
        }
    }
}

void init(){
    for(int i=0;i<MAX_USER;i++){
        User[i].ID = 'A' + i;
        User[i].active = 0;
        memset(User[i].IP,0,INFOLENGTH);
        User[i].Port = 0;
    }
}


void* ClientPthread(void* arg){
    char buffer[200];
    char applybuffer[200];
    ClientInfo clientnode = (ClientInfo)arg;  
    int serverSocket = clientnode->serverSocket;
    int iDataNum = clientnode->iDataNum;
    int client = clientnode->client;
    while(1)  
    {  
        iDataNum = recv(client, buffer, 1024, 0);  
        if(iDataNum < 0)  
        {  
            perror("recv");  
            continue;  
        }  
        buffer[iDataNum] = '\0';  
        printf("\n------\nData length:%d\nData : %s\n----------\n\n", iDataNum, buffer); 
        memset(applybuffer,0,200);

        if(strcmp(buffer, "quit") == 0){  
            char* s = "Bye";
            strcpy(applybuffer,s);
            break; 
        }
        else if(strcmp(buffer, "time") == 0){
            time_t timep;
            time (&timep);
            strcpy(applybuffer,asctime(gmtime(&timep)));
        }
        else if(strcmp(buffer, "name") == 0){
            char* s = "HOST name: Gesq";
            strcpy(applybuffer,s);
        }
        else if(strcmp(buffer, "list") == 0){
            PrintUser(applybuffer);
        }
        else{
            char* s = "I do not know";
            strcpy(applybuffer,s);
        }      

        // printf("Data length:%d\nrRecv data is %s\n", iDataNum, buffer);  
        // buffer[iDataNum] = 'A';  
        // buffer[iDataNum+1] = '\0';  
        send(client, applybuffer, strlen(applybuffer), 0);  
    }  
}
  
/* 
 监听后，一直处于accept阻塞状态， 
 直到有客户端连接， 
 当客户端如数quit后，断开与客户端的连接 
 */  
  
int main()  
{  
    init();
    //调用socket函数返回的文件描述符  
    int serverSocket;  
    //声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器  
    struct sockaddr_in server_addr;  
    struct sockaddr_in clientAddr;  
    int addr_len = sizeof(clientAddr);  
    int client;  
    char buffer[200];  
    int iDataNum;  
      
    //socket函数，失败返回-1  
    //int socket(int domain, int type, int protocol);  
    //第一个参数表示使用的地址类型，一般都是ipv4，AF_INET  
    //第二个参数表示套接字类型：tcp：面向连接的稳定数据传输SOCK_STREAM  
    //第三个参数设置为0  
    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
    {  
        perror("socket");  
        return 1;  
    }  
  
    bzero(&server_addr, sizeof(server_addr));  
    //初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序  
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(SERVER_PORT);  
    //ip可是是本服务器的ip，也可以用宏INADDR_ANY代替，代表0.0.0.0，表明所有地址  
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    //对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)  
    //bind三个参数：服务器端的套接字的文件描述符，  
    if(bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)  
    {  
        perror("connect");  
        return 1;  
    }  
    //设置服务器上的socket为监听状态  
    if(listen(serverSocket, 5) < 0)   
    {  
        perror("listen");  
        return 1;  
    }  
  
    while(1)  
    {  
        printf("Listening on port: %d\n", SERVER_PORT);  
        //调用accept函数后，会进入阻塞状态  
        //accept返回一个套接字的文件描述符，这样服务器端便有两个套接字的文件描述符，  
        //serverSocket和client。  
        //serverSocket仍然继续在监听状态，client则负责接收和发送数据  
        //clientAddr是一个传出参数，accept返回时，传出客户端的地址和端口号  
        //addr_len是一个传入-传出参数，传入的是调用者提供的缓冲区的clientAddr的长度，以避免缓冲区溢出。  
        //传出的是客户端地址结构体的实际长度。  
        //出错返回-1  
        client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);  
        if(client < 0)  
        {  
            perror("accept");  
            continue;  
        }  
        printf("\nrecv client data...\n");  
        //inet_ntoa   ip地址转换函数，将网络字节序IP转换为点分十进制IP  
        //表达式：char *inet_ntoa (struct in_addr);  
        printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr));  
        printf("Port is %d\n", htons(clientAddr.sin_port));  
        AddUser(inet_ntoa(clientAddr.sin_addr), htons(clientAddr.sin_port));
        pid_t pid;  
        int temp;
        struct client clientinfo;
        clientinfo.client = client;
        clientinfo.iDataNum = iDataNum;
        clientinfo.serverSocket = serverSocket;
        if((temp=pthread_create(&pid,NULL,ClientPthread,(void *)&clientinfo)))  
        {  
            printf("can't create thread: %s\n",strerror(temp));  
            return 1;  
        }  
        
    }  
    return 0;  
}  