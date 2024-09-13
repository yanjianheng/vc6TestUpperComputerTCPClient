#include <winsock2.h>
#include <stdio.h>		//标准输入输出
#include <windows.h>	//包含Sleep函数和线程函数	
#include <process.h>	//线程函数

#pragma comment(lib,"ws2_32.lib")		//连接winsock库

#define SERVER_IP		"192.168.0.7"
#define SERVER_PORT		23			

const char* COMMAND = "Hello,Server!";

//接收线程的函数声明
unsigned int __stdcall	ReceiveThread(void* param);

int main(){
	WSADATA wsaData;			//用于存储Windsock 初始化信息
	SOCKET	sockClient;			//客户端套接字
	SOCKADDR_IN	addrServer;		//服务器地址结构

	char recvBuffer[1024];		//接收数据缓冲区
	char retFlag = 0;
	int connectResult;
	int retryCount = 0;
	int sendResult = 0;
	int dataLength = 0;
	const int maxRetries = 5;
	
	//初始化Winsock2.2库
	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0){
		printf("Winsock 初始化失败！");
		return 1;
	}

	//创建套接字
	sockClient = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockClient == INVALID_SOCKET){
		printf("创建套接字失败！\n");
		WSACleanup();
		return 1;
	}

	//服务器地址和端口
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(SERVER_PORT);		//服务器端口
	addrServer.sin_addr.s_addr = inet_addr(SERVER_IP);

	//连接服务器，添加重试机制
	do{
		//连接服务器
		connectResult = connect(sockClient,(SOCKADDR*)&addrServer,sizeof(addrServer));
		if(connectResult == SOCKET_ERROR){
			printf("连接服务器失败！错误码：%d\n",WSAGetLastError());
			retryCount++;
			if(retryCount < maxRetries){
			    printf("正在尝试重新连接（%d/%d）...\n", retryCount, maxRetries);
                Sleep(100);  // 等待 1 秒后重试	
			}
			else{
				printf("达到最大重复次数，程序退出。\n");
				closesocket(sockClient);		//关闭套接字
				WSACleanup();
				return 1;		//放到FMC中就得考虑下了
			}
		}
	}while(connectResult == SOCKET_ERROR);

	printf("成功连接服务器!\n");
	
	//设置接收超时时间500ms
	int recvTimeout = 3000;
	setsockopt(sockClient,SOL_SOCKET,SO_RCVTIMEO,(const char*)&recvTimeout,sizeof(recvTimeout));

	//发送数据
	for(int i = 0;i < 10;i++){
		sendResult = send(sockClient, COMMAND,strlen(COMMAND),0);
		if(sendResult == SOCKET_ERROR){
            printf("发送数据失败！错误码：%d\n", WSAGetLastError());
		}
		else{
            printf("已发送第 %d 条指令\n", i + 1);
		}
		retFlag = recv(sockClient,recvBuffer,sizeof(recvBuffer) - 1,0);
		Sleep(3000);
	    if (retFlag > 0)
        {
            recvBuffer[retFlag] = '\0';  // 添加字符串结束符
            printf("收到服务器消息：%s\n", recvBuffer);
            // TODO: 根据需要处理接收到的数据
        }
        else if (retFlag == 0)
        {
            printf("连接已关闭！\n");
            break;  // 退出循环
        }
        else
        {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT)
            {
                // 未在 100 毫秒内收到数据，继续发送下一条指令
                printf("未在 100 毫秒内收到数据，继续发送下一条指令。\n");
            }
            else
            {
                printf("接收数据失败！错误码：%d\n", error);
                break;  // 退出循环
            }
        }
	}
	Sleep(1500);
	//关闭套接字
	closesocket(sockClient);

	//清理 Winsock库
	WSACleanup();
	system("pause");
	return 0;
}

