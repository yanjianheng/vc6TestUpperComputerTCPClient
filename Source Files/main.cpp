#include <winsock2.h>
#include <stdio.h>		//��׼�������
#include <windows.h>	//����Sleep�������̺߳���	
#include <process.h>	//�̺߳���

#pragma comment(lib,"ws2_32.lib")		//����winsock��

#define SERVER_IP		"192.168.0.7"
#define SERVER_PORT		23			

const char* COMMAND = "Hello,Server!";

//�����̵߳ĺ�������
unsigned int __stdcall	ReceiveThread(void* param);

int main(){
	WSADATA wsaData;			//���ڴ洢Windsock ��ʼ����Ϣ
	SOCKET	sockClient;			//�ͻ����׽���
	SOCKADDR_IN	addrServer;		//��������ַ�ṹ

	char recvBuffer[1024];		//�������ݻ�����
	char retFlag = 0;
	int connectResult;
	int retryCount = 0;
	int sendResult = 0;
	int dataLength = 0;
	const int maxRetries = 5;
	
	//��ʼ��Winsock2.2��
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
	addrServer.sin_port = htons(SERVER_PORT);		//�������˿�
	addrServer.sin_addr.s_addr = inet_addr(SERVER_IP);

	//���ӷ�������������Ի���
	do{
		//���ӷ�����
		connectResult = connect(sockClient,(SOCKADDR*)&addrServer,sizeof(addrServer));
		if(connectResult == SOCKET_ERROR){
			printf("���ӷ�����ʧ�ܣ������룺%d\n",WSAGetLastError());
			retryCount++;
			if(retryCount < maxRetries){
			    printf("���ڳ����������ӣ�%d/%d��...\n", retryCount, maxRetries);
                Sleep(100);  // �ȴ� 1 �������	
			}
			else{
				printf("�ﵽ����ظ������������˳���\n");
				closesocket(sockClient);		//�ر��׽���
				WSACleanup();
				return 1;		//�ŵ�FMC�о͵ÿ�������
			}
		}
	}while(connectResult == SOCKET_ERROR);

	printf("�ɹ����ӷ�����!\n");
	
	//���ý��ճ�ʱʱ��500ms
	int recvTimeout = 3000;
	setsockopt(sockClient,SOL_SOCKET,SO_RCVTIMEO,(const char*)&recvTimeout,sizeof(recvTimeout));

	//��������
	for(int i = 0;i < 10;i++){
		sendResult = send(sockClient, COMMAND,strlen(COMMAND),0);
		if(sendResult == SOCKET_ERROR){
            printf("��������ʧ�ܣ������룺%d\n", WSAGetLastError());
		}
		else{
            printf("�ѷ��͵� %d ��ָ��\n", i + 1);
		}
		retFlag = recv(sockClient,recvBuffer,sizeof(recvBuffer) - 1,0);
		Sleep(3000);
	    if (retFlag > 0)
        {
            recvBuffer[retFlag] = '\0';  // ����ַ���������
            printf("�յ���������Ϣ��%s\n", recvBuffer);
            // TODO: ������Ҫ������յ�������
        }
        else if (retFlag == 0)
        {
            printf("�����ѹرգ�\n");
            break;  // �˳�ѭ��
        }
        else
        {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT)
            {
                // δ�� 100 �������յ����ݣ�����������һ��ָ��
                printf("δ�� 100 �������յ����ݣ�����������һ��ָ�\n");
            }
            else
            {
                printf("��������ʧ�ܣ������룺%d\n", error);
                break;  // �˳�ѭ��
            }
        }
	}
	Sleep(1500);
	//�ر��׽���
	closesocket(sockClient);

	//���� Winsock��
	WSACleanup();
	system("pause");
	return 0;
}

