#include <winsock2.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")		//连接winsock库

int main(){
	WSADATA wsaData;
	SOCKET	sockClient;
	SOCKADDR_IN	addrServer;
	
	//初始化Winsock库
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
	addrServer.sin_port = htons(23);		//服务器端口
	addrServer.sin_addr.s_addr = inet_addr("192.168.0.7");
	
	//连接服务器
	if(connect(sockClient,(SOCKADDR*)&addrServer,sizeof(addrServer)) == SOCKET_ERROR){
		printf("连接服务器失败！\n");
		closesocket(sockClient);
		WSACleanup();
		return 1;
	}

	printf("成功连接服务器!\n");

	//发送数据
	const char* sendData = "Hello,Server!";
	for(int i = 0;i < 10;i++){
		int sendResult = send(sockClient,sendData,strlen(sendData),0);
		if(sendResult == SOCKET_ERROR){
			printf("发送数据失败！\n");
			closesocket(sockClient);
			WSACleanup();
			return 1;
		}
		else{
            printf("已发送第 %d 条指令\n", i + 1);
		}
		Sleep(100);
	}

	//接收数据
	char recvData[256];
	int ret = recv(sockClient, recvData, sizeof(recvData) - 1, 0);
	if(ret > 0){
		recvData[ret] = '\0';
		printf("收到服务器消息：%s\n",recvData);
	}
	else{
		printf("接收数据失败！\n");
	}
	
	//关闭套接字
	closesocket(sockClient);

	//清理 Winsock库
	WSACleanup();
	system("pause");
	return 0;
}


