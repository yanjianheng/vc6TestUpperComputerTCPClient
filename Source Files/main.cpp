#include <winsock2.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")		//����winsock��

int main(){
	WSADATA wsaData;
	SOCKET	sockClient;
	SOCKADDR_IN	addrServer;
	
	//��ʼ��Winsock��
	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0){
		printf("Winsock ��ʼ��ʧ�ܣ�");
		return 1;
	}

	//�����׽���
	sockClient = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockClient == INVALID_SOCKET){
		printf("�����׽���ʧ�ܣ�\n");
		WSACleanup();
		return 1;
	}

	//��������ַ�Ͷ˿�
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(23);		//�������˿�
	addrServer.sin_addr.s_addr = inet_addr("192.168.0.7");
	
	//���ӷ�����
	if(connect(sockClient,(SOCKADDR*)&addrServer,sizeof(addrServer)) == SOCKET_ERROR){
		printf("���ӷ�����ʧ�ܣ�\n");
		closesocket(sockClient);
		WSACleanup();
		return 1;
	}

	printf("�ɹ����ӷ�����!\n");

	//��������
	const char* sendData = "Hello,Server!";
	for(int i = 0;i < 10;i++){
		int sendResult = send(sockClient,sendData,strlen(sendData),0);
		if(sendResult == SOCKET_ERROR){
			printf("��������ʧ�ܣ�\n");
			closesocket(sockClient);
			WSACleanup();
			return 1;
		}
		else{
            printf("�ѷ��͵� %d ��ָ��\n", i + 1);
		}
		Sleep(100);
	}

	//��������
	char recvData[256];
	int ret = recv(sockClient, recvData, sizeof(recvData) - 1, 0);
	if(ret > 0){
		recvData[ret] = '\0';
		printf("�յ���������Ϣ��%s\n",recvData);
	}
	else{
		printf("��������ʧ�ܣ�\n");
	}
	
	//�ر��׽���
	closesocket(sockClient);

	//���� Winsock��
	WSACleanup();
	system("pause");
	return 0;
}


