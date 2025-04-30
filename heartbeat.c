#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define TIMEOUT_SEC  2
#define TIMEOUT_USEC 0
#define INTERVAL 1
#define RETRY_MAX 3
#define MAX_NODENUM 2
#define PEER_IP "127.0.0.1"

#define ALIVE	1
#define DEAD	0
struct nodemap {
	int self;
	int peer;
};

struct nodemap nm;

void print_event(char *str, unsigned int *vc, int node_num)
{
	int i;
	printf("VC ");
	for (i = 0; i < node_num; i++) {
		printf("[%d]", vc[i]);
	}
	printf("\tNM ");
	printf("self[%d] peer[%d]\t", nm.self, nm.peer);
	printf("%s\n", str);
}

int main(int argc, char **argv)
{
	int sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	int client_addr_len;
	struct timeval timeout;
	unsigned int vc[MAX_NODENUM];
	unsigned int buffer[MAX_NODENUM];	/*送受信用バッファ */
	int node_num = MAX_NODENUM;	/*FIXME:今は仮で2ノードのみ取り扱う */
	int self_nodenum;
	int retry_count = 0;
	unsigned int self_port, peer_port;

	nm.self = ALIVE;
	nm.peer = DEAD;

	if (argc != 2) {
		printf("Usage: %s node number\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	self_nodenum = atoi(argv[1]);
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = TIMEOUT_USEC;

	if (&serv_addr == NULL) {
		perror("serv_addr is NULL");
		exit(EXIT_FAILURE);
	}
	if (self_nodenum == 0) {
		self_port = 8000;
		peer_port = 8001;
	} else {
		self_port = 8001;
		peer_port = 8000;
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(self_port);
	memset(&client_addr, 0, sizeof(serv_addr));
	client_addr.sin_family = PF_INET;
	client_addr.sin_addr.s_addr = inet_addr(PEER_IP);
	client_addr.sin_port = htons(peer_port);
	memset(&vc, 0, sizeof(vc));

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		perror("setsockopt(RCVTIMEO) failed");
		exit(EXIT_FAILURE);
	}
	if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}
	print_event("Program start", vc, node_num);
	while (1) {
		client_addr_len = sizeof(client_addr);
	/*********************************送信イベント*********************************/
		//vc[self_nodenum]++;	/*送信を一つのイベントとする */
		memcpy(buffer, vc, sizeof(vc));
		char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));

		if (sendto(sock, &buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, client_addr_len) < 0) {
			perror("sendto() failed");
			exit(EXIT_FAILURE);
		}
		print_event("Server send", vc, node_num);
	/*****************************************************************************/
	/*********************************受信イベント*********************************/
		if (recvfrom(sock, &buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, &client_addr_len) < 0) {
			if (errno == EWOULDBLOCK) {
				/*タイムアウト時のみ継続 */
				retry_count++;	/*タイムアウトカウント */
				if (retry_count == RETRY_MAX) {
					if (nm.peer == ALIVE) {
						printf("Peer server dead\n");
					}
					nm.peer = DEAD;
				}
			} else {
				/*それ以外は終了 */
				perror("recvfrom() failed");
				exit(EXIT_FAILURE);
			}
		} else {
			if (nm.peer == DEAD) {
				printf("Peer server live\n");
			}
			retry_count = 0;
			nm.peer = ALIVE;
			/*自身のベクトルクロックの値と受信したペアのベクトルの値（各要素について）の
			   最大値をとってベクトル内の各要素を更新 */
			for (int j = 0; j < node_num; j++) {
				vc[j] = (vc[j] < buffer[j]) ? buffer[j] : vc[j];
			}
			print_event("Server recv", vc, node_num);
		}
	/*****************************************************************************/
		sleep(INTERVAL);
	}

	return 0;
}
