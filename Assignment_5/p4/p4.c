/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE 2048


typedef struct node{
	char ip[100];
	int port;
	int status;
	int heartbeat;
} gossip_node;

gossip_node *node_list;
gossip_node *neighbour_list;

struct random_data *si_buf;
struct random_data *s_buf;

int localClock = 0;
int my_line_number = 0;
int last_fail_time = 0;
int fd;
int N;
int b;
int S;
int c;
int T;
int B;
int P;
int F;

int failed_arr[100];
struct sockaddr_in myaddr;
pthread_mutex_t mutex;


int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    he = gethostbyname( hostname );
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++){
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    } 
    return 1;
}

int get_node_list_index(gossip_node new_node){
	int i, index = -1;
	for(i=0; i<N; i++){
		if(new_node.port == node_list[i].port && strcmp(new_node.ip,node_list[i].ip) == 0){
			index = i;
			break;
		}
	}
	return index;
}

int get_fail_count(){
	int i, count = 0;
	for(i=0; i<N; i++){
		if(node_list[i].status == 0){
			count++;
		}
	}
	//printf("Fail Count: %d\n", count);
	return count;
}

void update_node_list_to_failures(){
	int i;
	for(i=0; i<N; i++){
		if(localClock - node_list[i].heartbeat >= F){
			node_list[i].status = 0;
		} else if (i != my_line_number){
			node_list[i].status = 1;
		}
	}
}

int contains_neighbour(int node_index, gossip_node * list, int count){
	int i;
	gossip_node new_node = node_list[node_index];
	for(i=0; i<count; i++){
		if(new_node.port == list[i].port && strcmp(new_node.ip,list[i].ip) == 0){
			return 1;
		}
	}
	return 0;
}

int compare_nodes(gossip_node node1, gossip_node node2){
	return (node1.port == node2.port && strcmp(node1.ip,node2.ip) == 0);
}

void node_list_to_char_arr(gossip_node * list, char *buf, int length){
	int i;
	for(i=0; i<length; i++){
		if(i == 0)
			sprintf(buf,"%s:%d:%d:%d|",list[i].ip,list[i].port, list[i].status, list[i].heartbeat);
		else
			sprintf(buf+strlen(buf),"%s:%d:%d:%d|",list[i].ip,list[i].port, list[i].status, list[i].heartbeat);
	}
	buf[strlen(buf)-1] = '\0';
}

void update_node_list(gossip_node* receive_list){
	int i;
	for(i=0; i<N; i++){
		int index = get_node_list_index(receive_list[i]);
		if(node_list[index].heartbeat < receive_list[i].heartbeat){
			node_list[index].heartbeat = receive_list[i].heartbeat;
		}
	}
}

int fail_myself(){
	
	int index;
	if(B == 0)
		return 0;
	
	while(1){
		random_r(s_buf,&index);
		index = index%N;
		//printf("---- FAIL INDEX -> %d\n", index);
		int i;
		if(failed_arr[index] == 1){
			continue;
		} else {
			break;
		}
	}
	failed_arr[index] = 1;
	if(index == my_line_number){
		return 1;
	}
	return 0;
}


/*Thread function for receiving UDP messages*/
void* server_handler(void *args){
	struct sockaddr_in remaddr;	
	unsigned char data[N*50];
	socklen_t addrlen = sizeof(remaddr);
	gossip_node * receive_list = (gossip_node*)malloc(sizeof(gossip_node)*N);
	int rcvcount = 0;
	while(1){
		int recvlen = recvfrom(fd, data, N*50, 0, (struct sockaddr *)&remaddr, &addrlen);
		//printf("Receiver Port : %d\n", ntohs(remaddr.sin_port));
		/* Parsing incoming message fomr char[] to gossip_node list */
		if (recvlen > 0) {
			data[recvlen] = 0; 
			  int port;
			  int status;
			  int heartbeat;
			  int i = 0;
			  
			  
	
			  //gossip_node *new_node = (gossip_node*)malloc(sizeof(gossip_node));
			  for(rcvcount = 0; rcvcount < N ;rcvcount++)
			  {
				char tmpheartbeat[5];
				char ip[17];
				char tmpport[7];
				char tmpstatus[2];
				int ipc = 0;
				while(data[i] != ':'){
				  sprintf(ip+ipc,"%c",data[i]);
				  i++;
				  ipc++;
				}
				ip[i] = '\0';
				i++;
				int idk = 0;
				while(data[i] != ':') 
				{
				  sprintf(tmpport+idk,"%c",data[i]);
				  i++;
				  idk++;
				}
				i++;
				tmpport[idk] = '\0';
				port = atoi(tmpport);
				int idl = 0;
				while(data[i] != ':') 
				{
				  sprintf(tmpstatus+idl,"%c",data[i]);
				  i++;
				  idl++;
				}
				i++;
				tmpstatus[idl] = '\0';
				status = atoi(tmpstatus);
				int idz= 0;
				while(data[i] && data[i] != '|') 
				{
				  sprintf(tmpheartbeat+idz,"%c",data[i]);
				  i++;
				  idz++;
				}
				tmpheartbeat[idz] = '\0';
				heartbeat = atoi(tmpheartbeat);
				//printf("%s %s %s %s\n",ip,tmpport,tmpstatus,tmpheartbeat);
				gossip_node new_node;
				strcpy(new_node.ip,ip);
				new_node.port = port;
				new_node.status = status;
				new_node.heartbeat = heartbeat;
				//printf("Rcvcount %d\n", rcvcount);
				receive_list[rcvcount] = new_node;
				//printf("Rcvlist updated \n");
				i++;
			}
			//printf("before lock\n");
			pthread_mutex_lock(&mutex);
			//printf("before update\n");
			update_node_list(receive_list);
			//printf("between update\n");
			update_node_list_to_failures();
			//printf("after update\n");
			pthread_mutex_unlock(&mutex);			
		}
	}
}

int main(int argc, char **argv){
	
	if(argc != 9){
		printf("Usage ./p4 N b c F B P S T %d\n", argc);
		exit(0);
	}
	
	N = atoi(argv[1]);
	b = atoi(argv[2]);
	c = atoi(argv[3]);
	F = atoi(argv[4]);
	B = atoi(argv[5]);
	P = atoi(argv[6]);
	S = atoi(argv[7]);
	T = atoi(argv[8]);
	
	node_list = (gossip_node*)malloc(sizeof(gossip_node)*N);
	neighbour_list = (gossip_node*)malloc(sizeof(gossip_node)*b);
		
	struct sockaddr_in remaddr;	
	socklen_t addrlen = sizeof(remaddr);		
	int recvlen;		
	unsigned char buf[BUFSIZE];	

	pthread_mutex_init(&mutex, NULL);
	
	/*Creating a socket*/
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket failed\n");
		return 0;
	}

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);
	
	/*Binding to a socket*/
	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}
	
	socklen_t len = sizeof(myaddr);
	if (getsockname(fd, (struct sockaddr *)&myaddr, &len) == -1)
		perror("getsockname");
	
	char hostname[1024];
	char ip[100];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	printf("Hostname: %s\n", hostname);
	struct hostent* h;
	h = gethostbyname(hostname);
	hostname_to_ip(h->h_name,ip);
	printf("IP Address: %s\n", ip);
	printf("Port number %d\n", ntohs(myaddr.sin_port));	
	
	
	/*Reading lines in endpoints.txt file */
	FILE *fp1= fopen("endpoints.txt", "ab+");
	int lines = 0;
	char ch;
	while(!feof(fp1)){
		ch = fgetc(fp1);
		if(ch == '\n'){
			lines++;
		}
	}
	
	my_line_number = lines;
	
	/* Creating 2 random buffers based on 2 different seeds [S, S+I] */
	char *rand_buf1 = (char*)calloc(64, sizeof(char));
	char *rand_buf2 = (char*)calloc(64, sizeof(char));
	si_buf = (struct random_data*)calloc(1,sizeof(struct random_data));
	s_buf = (struct random_data*)calloc(1,sizeof(struct random_data));
	initstate_r(S+my_line_number,rand_buf1, 64, si_buf);
	initstate_r(S,rand_buf2, 64, s_buf);
	srandom_r(S+my_line_number, si_buf);
	srandom_r(S, s_buf);
	
	fclose(fp1);
	FILE *fp = fopen("endpoints.txt", "ab+");
	int i;
	size_t buffsize = 150;
	char *line = malloc(sizeof(char)*buffsize);
	
	/*Last node check */
	if(lines == N-1){
		/*Send OK to all the other nodes*/
		fprintf(fp, "%s:%d\n", ip, ntohs(myaddr.sin_port));
		rewind(fp);
		for (i=0; i < N-1; i++) {
			int num = getline(&line, &buffsize, fp);
			printf("Line: %s\n",line);
			char ip[100];
			char port[50];
			int ipdone = 0;
			int j;
			int w;
			for(w=0; w<num; w++){
				if(line[w] != ':' && !ipdone)
					ip[w] = line[w];
				else{
					ipdone = 1;
					ip[w] = '\0';
					j = ++w;
					break;
				}
			}
			int k;
			for(w=0, k=j; k<num; w++, k++){
				port[w] = line[k];
			}
			port[w] = '\0';
			
			memset((char *) &remaddr, 0, sizeof(remaddr));
			remaddr.sin_family = AF_INET;
			remaddr.sin_port = htons(atoi(port));
			remaddr.sin_addr.s_addr = inet_addr(ip);
			sprintf(buf, "OK");
			sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);
		}
		fclose(fp);
	} else {
		/*Not the last node - add to endpoints file and wait for OK */
		fprintf(fp, "%s:%d\n", ip, ntohs(myaddr.sin_port));
		fclose(fp);
		recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		printf("Receiver IP : %s\n", inet_ntoa(remaddr.sin_addr));
		printf("received %d bytes\n", recvlen);
		if (recvlen > 0) {
			buf[recvlen] = 0;
			printf("received message: \"%s\"\n", buf);
		}
	}
	
	/*OK received - creating node_list from endpoints file */
	FILE *fp2 = fopen("endpoints.txt", "ab+");
	for (i=0; i < N; i++) {
		int num = getline(&line, &buffsize, fp);
		char ip[100];
		char port[50];
		int ipdone = 0;
		int j;
		int w;
		for(w=0; w<num; w++){
			if(line[w] != ':' && !ipdone)
				ip[w] = line[w];
			else{
				ipdone = 1;
				ip[w] = '\0';
				j = ++w;
				break;
			}
		}
		int k;
		for(w=0, k=j; k<num; w++, k++){
			port[w] = line[k];
		}
		port[w] = '\0';
		gossip_node new_node;
		new_node.port = atoi(port);
		new_node.status = 1;
		new_node.heartbeat = 0;
		strcpy(new_node.ip, ip);
		node_list[i] = new_node;
	}
	
	for(i=0; i<N; i++){
		printf("Node %d: %s %d %d %d\n",i, node_list[i].ip, node_list[i].port, node_list[i].status, node_list[i].heartbeat);
	}
	printf("\n\n");
	
	pthread_t serverthread;
	
	/* Starting server thread */
	pthread_create(&serverthread,NULL, server_handler, &myaddr);
	i=0;
	int send_msges = 1;
	
	/* Sending messages for c iterations */
	while(1){
		
		if(send_msges){
			pthread_mutex_lock(&mutex);
			int j;
			int neighbour_count = 0;
			/*Randomly find b neighbours to send a message*/
			while(neighbour_count < b){
				int32_t send_index;
				random_r(si_buf, &send_index);
				send_index = send_index%N;
				//printf("Random Index : %d\n", send_index);
				if(!contains_neighbour(send_index, neighbour_list, neighbour_count)){
					gossip_node new_node;
					new_node.port = node_list[send_index].port;
					new_node.status = node_list[send_index].status;
					new_node.heartbeat = node_list[send_index].heartbeat;
					strcpy(new_node.ip, node_list[send_index].ip);
					neighbour_list[neighbour_count] = new_node;
					neighbour_count++;
				}
			}
			for(j=0; j<b; j++){
				memset((char *) &remaddr, 0, sizeof(remaddr));
				remaddr.sin_family = AF_INET;
				remaddr.sin_port = htons(neighbour_list[j].port);
				remaddr.sin_addr.s_addr = inet_addr(neighbour_list[j].ip);
				//printf("Sending packet %d to %s port %d\n", j, neighbour_list[j].ip, neighbour_list[j].port );
				//sprintf(buf, "Sending list");
				char buffer[50*N];
				node_list_to_char_arr(node_list, buffer, N);
				sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr *)&remaddr, addrlen);
			}
			pthread_mutex_unlock(&mutex);	
		}
		/*Check localClock for failing myself */
		if(send_msges &&  localClock !=0 && localClock%P == 0){
			if(get_fail_count() < B && fail_myself()){
				printf("FAILING MYSELF %d\n",localClock);
				send_msges = 0;
				node_list[my_line_number].status = 0;
			}
		}
		i++;
		sleep(1);
		localClock++;
		printf("Localclock : %d\n", localClock);
		node_list[my_line_number].heartbeat = localClock;
		
		/*If c iterations completed or Time T is reached exit the send loop */
		if(i==c || localClock >= T)
			break;
	}
	
	/*If c iterations completed before T, wait till localClock reaches T seconds */
	while(localClock < T){
		sleep(1);
		localClock++;
	}
	
	/*Write the final output to listX file */
	char fileName[10];
	sprintf(fileName, "list%d.txt", my_line_number);
	FILE *fp_new = fopen(fileName, "ab+");
	fprintf(fp_new, "%s\n", node_list[my_line_number].status == 0 ? "FAIL" : "OK");
	int index;
	for(i=0; i<N; i++){
		fprintf(fp_new, "%d %d\n", i, node_list[i].heartbeat);
	}
	fclose(fp_new);
	
	for(i=0; i<N; i++){
		printf("FINAL NODE LIST\n");
		printf("Node %d: IP-%s Port-%d Status-%d Heartbeat-%d\n",i, node_list[i].ip, node_list[i].port, node_list[i].status, node_list[i].heartbeat);
	}
}
