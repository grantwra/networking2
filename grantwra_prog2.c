#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>

//GRANTWRA

//top file layout
//1st line : total num of servers
//2nd : number of neighbors
//3 - 6 (in this case) ips and ids of servers
//last few lines are costs

#define BUFFER_SIZE BUFSIZ 512
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */

struct peer_info {
	char name[30];
	char ip_address[30];
	char port[30];
	int id;
};

struct recieved_datagram	{
	//struct sockaddr_in recipient_sockaddr_in;

	char recipient_ip[30];
	char recipient_port[30];
	uint16_t recipient_id;
	uint64_t recipient_cost;

};

struct datagram
{
	uint16_t num_of_fields;
	//uint16_t sender_port;
	//uint32_t sender_ip;

	struct sockaddr_in sender_sockaddr_in;

	//uint32_t recipient1_ip;
	//uint16_t recipient1_port;

	struct sockaddr_in recipient1_sockaddr_in;
	uint16_t recipient1_id;
	uint64_t recipient1_cost;

	//uint32_t recipient2_ip;
	//uint16_t recipient2_port;
	struct sockaddr_in recipient2_sockaddr_in;
	uint16_t recipient2_id;
	uint64_t recipient2_cost;

	//uint32_t recipient3_ip;
	//uint16_t recipient3_port;
	struct sockaddr_in recipient3_sockaddr_in;
	uint16_t recipient3_id;
	uint64_t recipient3_cost;

	//uint32_t recipient4_ip;
	//uint16_t recipient4_port;
	struct sockaddr_in recipient4_sockaddr_in;
	uint16_t recipient4_id;
	uint64_t recipient4_cost;

	//uint32_t recipient5_ip;
	//uint16_t recipient5_port;
	struct sockaddr_in recipient5_sockaddr_in;
	uint16_t recipient5_id;
	uint64_t recipient5_cost;

};

int packet_count;

int time_interval;

int who_am_i;
int my_listen_fd;
char my_listen_port[30];
char my_ip[30];

//struct peer_info peer_info_array[5];
char *num_of_neighbors;

char *findIpName(char *ip_address){
	if(strcmp(ip_address,"128.205.36.8") == 0){ return "timberlake.cse.buffalo.edu"; }
	if(strcmp(ip_address,"128.205.36.33") == 0){ return "highgate.cse.buffalo.edu"; }
	if(strcmp(ip_address,"128.205.36.34") == 0){ return "euston.cse.buffalo.edu"; }
	if(strcmp(ip_address,"128.205.36.35") == 0){ return "embankment.cse.buffalo.edu"; }
	if(strcmp(ip_address,"128.205.36.36") == 0){ return "underground.cse.buffalo.edu"; }

	else { return "UNKNOWN"; }

}

void print_routing_diagram(int routing_diagram[5][5]){
	printf("\n");
	int i;
	int j;
	for(i = 1; i <=5; i++){
		for(j = 1; j <=6; j++){
			if(j > 5){
				printf("\n");
				break;
			}
			else{
				printf("%d ", routing_diagram[j][i]);
			}
		}
	}
}

void pretty_print_routing_diagram(int routing_diagram[5][5]){
	printf("\n");
	printf("     1. 2. 3. 4. 5.\n");
	printf("    ______________\n");
	int i;
	int j;
	for(i = 1; i <=5; i++){
		printf("%d. | ",i);
		for(j = 1; j <=6; j++){
			if(j > 5){
				printf("\n");
				break;
			}
			else{
				printf("%d ", routing_diagram[j][i]);
			}
		}
	}
}

void print_servers(struct peer_info peer_info_array[5]){
	int i;
	for(i = 1; i <= 5; i ++){
		if(peer_info_array[i].id > 0){
			printf("Server id: %d\n",peer_info_array[i].id);
			printf("Server ip: %s\n",peer_info_array[i].ip_address);
			printf("Server port: %s\n",peer_info_array[i].port);
			printf("Server name: %s\n",peer_info_array[i].name);

		}
	}
}

int create_socket(char *port){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL,port,&hints,&server_info);
	if(status != 0){
		printf("Error: getaddrinfo error!!!!! : %s\n", gai_strerror(status));
		exit(1);
	}
	int sock = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if(sock == -1)
	{
    	printf("error opening socket");
    	return -1;
	}
	if(bind(sock, server_info->ai_addr,server_info->ai_addrlen) == -1)
	{
    	printf("error binding socket\n");
    	return -1;
	}
	freeaddrinfo(server_info);
	printf("Listening on port %s!!\n", port);

	return sock;
}

int send_mes(int reciever_id, char *msg, struct peer_info peer_info_array[5]){

	//printf("I got here!\n");

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	char *reciever_ipaddr = peer_info_array[reciever_id].ip_address;
	char *reciever_port = peer_info_array[reciever_id].port;

	//to test local
	int status = getaddrinfo("0.0.0.0",reciever_port,&hints,&server_info);

//	int status = getaddrinfo(reciever_ipaddr,reciever_port,&hints,&server_info);

	if(status != 0){
		printf("Error: getaddrinfo error!!!!! : %s\n", gai_strerror(status));
		exit(1);
	}

	int udp_sock = socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_protocol);
	if(udp_sock == -1){
		printf("Error %d: socket() failed\n", errno);
		exit(1);
	}

	int bytes_sent = sendto(udp_sock,msg,strlen(msg)+1,0,server_info->ai_addr,server_info->ai_addrlen);
	if(bytes_sent == -1){
		printf("bytes_sent failed to send : %d\n", errno);
		exit(1);
	}
	//printf("bytes_sent: %d\n",bytes_sent);
	freeaddrinfo(server_info);
	close(udp_sock);

	return 0;
}

int send_broadcast(int reciever_id, struct datagram *mydatagram, struct peer_info peer_info_array[5]){

	//printf("I got here!\n");

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	char *reciever_ipaddr = peer_info_array[reciever_id].ip_address;
	char *reciever_port = peer_info_array[reciever_id].port;

	//to test local
	int status = getaddrinfo("0.0.0.0",reciever_port,&hints,&server_info);

	//int status = getaddrinfo(reciever_ipaddr,reciever_port,&hints,&server_info);

	if(status != 0){
		printf("Error: getaddrinfo error!!!!! : %s\n", gai_strerror(status));
		exit(1);
	}

	int udp_sock = socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_protocol);
	if(udp_sock == -1){
		printf("Error %d: socket() failed\n", errno);
		exit(1);
	}

	int bytes_sent = sendto(udp_sock,mydatagram,sizeof(struct datagram),0,server_info->ai_addr,server_info->ai_addrlen);
	if(bytes_sent == -1){
		printf("bytes_sent failed to send : %d\n", errno);
		exit(1);
	}
	//printf("bytes_sent: %d\n",bytes_sent);
	freeaddrinfo(server_info);
	close(udp_sock);

	return 0;
}

struct datagram *fill_datagram_disable(int who,int me){
	struct datagram *mydatagram = malloc(sizeof(struct datagram));

	mydatagram->num_of_fields = 20;
	mydatagram->recipient1_id = who;
	mydatagram->recipient2_id = me;
	//printf("who %d\n", mydatagram->recipient1_id);
	//printf("me %d\n", mydatagram->recipient2_id);

	return mydatagram;

}

struct datagram *fill_datagram_crash(int me){
	struct datagram *mydatagram = malloc(sizeof(struct datagram));

	mydatagram->num_of_fields = 15;
	mydatagram->recipient1_id = me;

	return mydatagram;

}

struct datagram *fill_datagram(struct peer_info peer_info_array[5], int routing_diagram[5][5]){
	struct datagram *mydatagram = malloc(sizeof(struct datagram));

	mydatagram->num_of_fields = 0;
	/*
	mydatagram->sender_port = 0;
	mydatagram->sender_ip = 0;
	*/
	//mydatagram->sender_sockaddr_in;

	//mydatagram->recipient1_ip = 0;
	//mydatagram->recipient1_port = 0;
	mydatagram->recipient1_id = 0;
	mydatagram->recipient1_cost = -1;

	//mydatagram->recipient2_ip = 0;
	//mydatagram->recipient2_port = 0;
	mydatagram->recipient2_id = 0;
	mydatagram->recipient2_cost = -1;

	//mydatagram->recipient3_ip = 0;
	//mydatagram->recipient3_port = 0;
	mydatagram->recipient3_id = 0;
	mydatagram->recipient3_cost = -1;

	//mydatagram->recipient4_ip = 0;
	//mydatagram->recipient4_port = 0;
	mydatagram->recipient4_id = 0;
	mydatagram->recipient4_cost = -1;

	//mydatagram->recipient5_ip = 0;
	//mydatagram->recipient5_port = 0;
	mydatagram->recipient5_id = 0;
	mydatagram->recipient5_cost = -1;

	//uint16_t count = 0;

	int i;
	int temp[5];
	int j;
	for(j = 1; j <=5; j++){
		temp[j] = -1;
	}
	for(i = 1; i <=5; i++){
		if(routing_diagram[i][who_am_i] != -1){
			mydatagram->num_of_fields++;
			temp[i] = routing_diagram[i][who_am_i];
		}
	}


	uint16_t count = mydatagram->num_of_fields;
	int x;

	//for 1
	int val = 0;
	if(count != 0){
		for(x = 1; x <= 5; x++){
			if(temp[x] != -1){
				val = temp[x];
				temp[x] = -1;
				break;
			}
		}
		mydatagram->recipient1_id = x;
		mydatagram->recipient1_cost = val;

		inet_pton(AF_INET, peer_info_array[x].ip_address, &(mydatagram->recipient1_sockaddr_in.sin_addr));
		mydatagram->recipient1_sockaddr_in.sin_port = htons(atoi(peer_info_array[x].port));
		//printf("Deserialized ID: : %hu\n", mydatagram->recipient1_id);
		//printf("Deserialized port : %hu\n", ntohs(mydatagram->recipient1_sockaddr_in.sin_port));

		/*

		printf("Deserialized port : %hu\n", ntohs(mydatagram->recipient1_sockaddr_in.sin_port));
		printf("Deserialized ID: : %hu\n", mydatagram->recipient1_id);
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(mydatagram->recipient1_sockaddr_in.sin_addr), str, INET_ADDRSTRLEN);

		printf("Deserialized IP: %s\n", str);
		printf("Cost : %llu\n", mydatagram->recipient1_cost);


		
		printf("Number: %d\n", x);
		printf("Port: %s\n", peer_info_array[x].port);
		printf("IP : %s\n", peer_info_array[x].ip_address);
		printf("Cost : %d\n", val);
		*/

		count--;
	}

	//for 2
	val = 0;
	if(count != 0){
		for(x = 1; x <= 5; x++){
			if(temp[x] != -1){
				val = temp[x];
				temp[x] = -1;
				break;
			}
		}
		mydatagram->recipient2_id = x;
		mydatagram->recipient2_cost = val;

		inet_pton(AF_INET, peer_info_array[x].ip_address, &(mydatagram->recipient2_sockaddr_in.sin_addr));
		mydatagram->recipient2_sockaddr_in.sin_port = htons(atoi(peer_info_array[x].port));
		//printf("Deserialized ID: : %hu\n", mydatagram->recipient2_id);

		/*
		printf("Deserialized port : %hu\n", ntohs(mydatagram->recipient2_sockaddr_in.sin_port));
		printf("Deserialized ID: : %hu\n", mydatagram->recipient2_id);
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(mydatagram->recipient2_sockaddr_in.sin_addr), str, INET_ADDRSTRLEN);

		printf("Deserialized IP: %s\n", str);
		printf("Cost : %llu\n", mydatagram->recipient2_cost);
		

		printf("Number: %d\n", x);
		printf("Port: %s\n", peer_info_array[x].port);
		printf("IP : %s\n", peer_info_array[x].ip_address);
		printf("Cost : %d\n", val);
		*/

		count--;
	}

	//for 3
	val = 0;
	if(count != 0){
		for(x = 1; x <= 5; x++){
			if(temp[x] != -1){
				val = temp[x];
				temp[x] = -1;
				break;
			}
		}
		mydatagram->recipient3_id = x;
		mydatagram->recipient3_cost = val;

		inet_pton(AF_INET, peer_info_array[x].ip_address, &(mydatagram->recipient3_sockaddr_in.sin_addr));
		mydatagram->recipient3_sockaddr_in.sin_port = htons(atoi(peer_info_array[x].port));
		//printf("Deserialized ID: : %hu\n", mydatagram->recipient3_id);
		/*

		printf("Deserialized port : %hu\n", ntohs(mydatagram->recipient3_sockaddr_in.sin_port));
		printf("Deserialized ID: : %hu\n", mydatagram->recipient3_id);
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(mydatagram->recipient3_sockaddr_in.sin_addr), str, INET_ADDRSTRLEN);

		printf("Deserialized IP: %s\n", str);
		printf("Cost : %llu\n", mydatagram->recipient3_cost);
		

		printf("Number: %d\n", x);
		printf("Port: %s\n", peer_info_array[x].port);
		printf("IP : %s\n", peer_info_array[x].ip_address);
		printf("Cost : %d\n", val);
		*/

		count--;
	}
	
	//for 4
	val = 0;
	if(count != 0){
		for(x = 1; x <= 5; x++){
			if(temp[x] != -1){
				val = temp[x];
				temp[x] = -1;
				break;
			}
		}
		mydatagram->recipient4_id = x;
		mydatagram->recipient4_cost = val;

		inet_pton(AF_INET, peer_info_array[x].ip_address, &(mydatagram->recipient4_sockaddr_in.sin_addr));
		mydatagram->recipient4_sockaddr_in.sin_port = htons(atoi(peer_info_array[x].port));
		/*

		printf("Number: %d\n", x);
		printf("Port: %s\n", peer_info_array[x].port);
		printf("IP : %s\n", peer_info_array[x].ip_address);
		printf("Cost : %d\n", val);
		*/

		count--;
	}

	//for 5
	val = 0;
	if(count != 0){
		for(x = 1; x <= 5; x++){
			if(temp[x] != -1){
				val = temp[x];
				temp[x] = -1;
				break;
			}
		}
		mydatagram->recipient5_id = x;
		mydatagram->recipient5_cost = val;

		inet_pton(AF_INET, peer_info_array[x].ip_address, &(mydatagram->recipient5_sockaddr_in.sin_addr));
		mydatagram->recipient5_sockaddr_in.sin_port = htons(atoi(peer_info_array[x].port));
		/*

		printf("Number: %d\n", x);
		printf("Port: %s\n", peer_info_array[x].port);
		printf("IP : %s\n", peer_info_array[x].ip_address);
		printf("Cost : %d\n", val);
		*/

		count--;
	}
	
	//mydatagram->sender_port = (uint16_t)atoi(my_listen_port);
	//mydatagram->sender_ip = (uint32_t)atoi(my_ip);

	//mydatagram->sender_port = (uint16_t)my_listen_port;
	//mydatagram->sender_ip = (uint32_t)my_ip;

	struct sockaddr_in server_ip_sockaddrin;
	//char str[INET_ADDRSTRLEN];

	// store this IP address in sa:
	inet_pton(AF_INET, my_ip, &(mydatagram->sender_sockaddr_in.sin_addr));
	mydatagram->sender_sockaddr_in.sin_port = htons(atoi(my_listen_port));

	//mydatagram->recipient1_id = (uint16_t)5;

	//printf("Serialized : %s\n", server_ip_sockaddrin.sin_addr);

	// now get it back and print it
	//inet_ntop(AF_INET, &(mydatagram->sender_sockaddr_in.sin_addr), str, INET_ADDRSTRLEN);

	//printf("Deserialized : %s\n", str);
	//printf("Deserialized port : %hu\n", ntohs(mydatagram->sender_sockaddr_in.sin_port));
	//printf("Num for datagram : %hu\n", mydatagram->recipient1_id);

	//free(str);

	//printf("Num for datagram : %hu\n", mydatagram->num_of_fields);
	//printf("My Port for datagram : %hu\n", mydatagram->sender_port);
	//printf("my ip for datagram : %u\n", mydatagram->sender_ip);
	//free(mydatagram);

	return mydatagram;
}

//flag defines operation : 0 normal, 1 crash, 2 disable <flag2>
int broadcast(struct peer_info peer_info_array[5], int routing_diagram[5][5],int flag, int flag2){

	struct datagram *yo;
	if(flag == 1){
		yo = fill_datagram_crash(who_am_i);
	}
	else if(flag == 2){
		yo = fill_datagram_disable(flag2,who_am_i);
	}
	else{
		yo = fill_datagram(peer_info_array,routing_diagram);
	}

	int i;
	//printf("BROADCASTING!\n");
	for(i = 1; i <=5; i++){
		if(routing_diagram[i][who_am_i] != -1 && routing_diagram[i][who_am_i] != 0){
			//send_mes(i, "testing duh",peer_info_array);
			send_broadcast(i, yo,peer_info_array);
		}
	}
	//free(mydatagram);

	return 0;
}

int run_command(char *fullinput, struct peer_info peer_info_array[5],int routing_diagram[5][5]){
	
	char *input;
	char *input2;
	char *input3;
	char *input4;
	char *split = strtok(fullinput, " ");
	int x = 0;
	while(split != NULL){
		if(x == 0){
			input = split;
		}
		if(x == 1){
			split=strtok(NULL," ");
			input2 = split;
		}
		if(x == 2){
			split=strtok(NULL," ");
			input3 = split;
		}
		if(x == 3){
			split=strtok(NULL,"\0");
			input4 = split;
		}
		if(x == 4){
			break;
		}
		x++;
	}

	if(strncmp(input,"PACKETS\n",40) == 0){
	
		printf("Number of packets recieved since last checked: %d\n", packet_count);
		packet_count = 0;
		printf("PACKETS SUCCESS\n");
	}
	else if(strncmp(input,"packets\n",40) == 0){
		
		printf("Number of packets recieved since last checked: %d\n", packet_count);
		packet_count = 0;
		printf("packets SUCCESS\n");
	}
	else if(strncmp(input,"DISPLAY\n",40) == 0){
		pretty_print_routing_diagram(routing_diagram);
		printf("DISPLAY SUCCESS\n");
	}
	else if(strncmp(input,"display\n",40) == 0){
		pretty_print_routing_diagram(routing_diagram);
		printf("display SUCCESS\n");
	}
	else if(strncmp(input,"CRASH\n",40) == 0){
		//pretty_print_routing_diagram(routing_diagram);
		//printf("display SUCCESS\n");
		/*
		int ix;
		int temp[5][5];
		for(ix = 1; ix <= 5; ix++){
			temp[ix][who_am_i] = routing_diagram[ix][who_am_i];
			routing_diagram[ix][who_am_i] = -1;
			
		}
		*/
		broadcast(peer_info_array,routing_diagram,1,0);
		return 1;
	}
	else if(strncmp(input,"crash\n",40) == 0){
		//pretty_print_routing_diagram(routing_diagram);
		//printf("display SUCCESS\n");
		/*
		int ib;
		int temp2[5][5];
		for(ib = 1; ib <= 5; ib++){
			temp2[ib][who_am_i] = routing_diagram[ib][who_am_i];
			routing_diagram[ib][who_am_i] = -1;
			//print_routing_diagram(routing_diagram);
		}
		*/
		broadcast(peer_info_array,routing_diagram,1,0);
		return 1;
	}

	else if(strncmp(input,"STEP\n",40) == 0){
		//help();
		//printf("before broadcast\n");
		broadcast(peer_info_array,routing_diagram,0,0);
		printf("STEP SUCCESS\n");
		//send_mes(1, "testing duh",peer_info_array);
		//printf("worked!\n");
	}
	else if(strncmp(input,"step\n",40) == 0){
		//help();
		//printf("before broadcast\n");
		broadcast(peer_info_array,routing_diagram,0,0);
		printf("step SUCCESS\n");
		//send_mes(1, "testing duh",peer_info_array);
		//printf("worked!\n");
	}
	else if(strncmp(input,"disable",40) == 0){
		//pretty_print_routing_diagram(routing_diagram);
		//printf("display SUCCESS\n");
		int temp = atoi(input2);
		//printf("%d\n", temp);
		//routing_diagram[temp][who_am_i] = -1;
		broadcast(peer_info_array,routing_diagram,2,temp);
		routing_diagram[temp][who_am_i] = -1;
	}
	else if(strncmp(input,"DISABLE",40) == 0){
		//pretty_print_routing_diagram(routing_diagram);
		//printf("display SUCCESS\n");
		int temp = atoi(input2);
		//printf("%d\n", temp);
		//routing_diagram[temp][who_am_i] = -1;
		broadcast(peer_info_array,routing_diagram,2,temp);
		routing_diagram[temp][who_am_i] = -1;
	}
	else if(strncmp(input,"update",40) == 0){
		//printf("%s\n", input);
		if(input2 == NULL || input3 == NULL || input4 == NULL){
			printf("Invalid arguments!\n");
			return 0;
		}
		if(strncmp(input4,"inf\n",40) == 0){
			int i1 = atoi(input2);
			int i2 = atoi(input3);
			routing_diagram[i2][i1] = -1;
			printf("update SUCCESS\n");
			return 0;
		}
		//printf("%s\n", input2);
		//printf("%s\n", input3);
		//printf("%s\n", input4);
		int i1 = atoi(input2);
		int i2 = atoi(input3);
		int i3 = atoi(input4);
		routing_diagram[i2][i1] = i3;
		printf("update SUCCESS\n");
		//printf("%d %d %d\n",i1,i2,i3);
		//do nothing
		//return 1;
	}
	else if(strncmp(input,"UPDATE",40) == 0){
		//printf("%s\n", input);
		if(input2 == NULL || input3 == NULL || input4 == NULL){
			printf("Invalid arguments!\n");
			return 0;
		}
		if(strncmp(input4,"inf\n",40) == 0){
			int i1 = atoi(input2);
			int i2 = atoi(input3);
			routing_diagram[i2][i1] = -1;
			printf("UPDATE SUCCESS\n");
			return 0;
		}
		//printf("%s\n", input2);
		//printf("%s\n", input3);
		//printf("%s\n", input4);
		int i1 = atoi(input2);
		int i2 = atoi(input3);
		int i3 = atoi(input4);
		routing_diagram[i2][i1] = i3;
		printf("UPDATE SUCCESS\n");
		//printf("%d %d %d\n",i1,i2,i3);
		//do nothing
		//return 1;
	}
	else if(strncmp(input,"QUIT\n",40) == 0){
		//do nothing
		return 1;
	}
	else if(strncmp(input,"quit\n",40) == 0){
		//do nothing
		return 1;
	}
	else {
		printf("Invalid command.\n");
	}

	return 0;
}

void update_routing_diagram(struct datagram mydatagram,struct peer_info peer_info_array[5],int routing_diagram[5][5]){
	//printf("NUmber of fields: %hu\n", mydatagram.num_of_fields);
	int count = mydatagram.num_of_fields;
	uint16_t sender_id;
	while(count != 0){
		if(count == 1){
			if(mydatagram.recipient1_cost == 0){
				sender_id = mydatagram.recipient1_id;
				break;
			}
		}
		if(count == 2){
			if(mydatagram.recipient2_cost == 0){
				sender_id = mydatagram.recipient2_id;
				break;
			}
		}
		if(count == 3){
			if(mydatagram.recipient3_cost == 0){
				sender_id = mydatagram.recipient3_id;
				break;
			}
		}
		if(count == 4){
			if(mydatagram.recipient4_cost == 0){
				sender_id = mydatagram.recipient4_id;
				break;
			}
		}
		if(count == 5){
			if(mydatagram.recipient5_cost == 0){
				sender_id = mydatagram.recipient5_id;
				break;
			}

		}
		count--;
	}
	printf("RECEIVED A MESSAGE FROM SERVER %hu\n", sender_id);

	count = mydatagram.num_of_fields;

	int b;
	for(b = 1; b <=5;b++){
		routing_diagram[b][sender_id] = -1;
	}

	while(count != 0){
		if(count == 1){
			routing_diagram[mydatagram.recipient1_id][sender_id] = mydatagram.recipient1_cost;
		}
		if(count == 2){
			routing_diagram[mydatagram.recipient2_id][sender_id] = mydatagram.recipient2_cost;
		}
		if(count == 3){
			routing_diagram[mydatagram.recipient3_id][sender_id] = mydatagram.recipient3_cost;
		}
		if(count == 4){
			routing_diagram[mydatagram.recipient4_id][sender_id] = mydatagram.recipient4_cost;
		}
		if(count == 5){
			routing_diagram[mydatagram.recipient5_id][sender_id] = mydatagram.recipient5_cost;
		}
		count--;
	}

	//print_routing_diagram(routing_diagram);
	//printf("Sender id: %hu\n", sender_id);
	/*
	struct recieved_datagram *recieved_datagram = malloc(sizeof(struct recieved_datagram) * 5);

	int i;
	for(i = 1; i <= count;i++){
		recieved_datagram[i];
	}
	char recipient_ip[30];
	char recipient_port[30];
	uint16_t recipient_id;
	uint64_t recipient_cost;
	

	free(recieved_datagram);
	*/
}

int server_start(struct peer_info peer_info_array[5],int routing_diagram[5][5]){
	fd_set master;
	fd_set fd_readSockets;
	FD_ZERO(&fd_readSockets);
	FD_ZERO(&master);
    int max_fd = 0;
    max_fd = my_listen_fd;
    //FD_SET(socketNum, &master);
    FD_SET(0, &master);
    FD_SET(my_listen_fd, &master);
    //max_fd = socketNum;

	char i[BUFSIZ];
    
    //int fdtest;
	int j;
	float times = 0.0;
	while (1) {
		struct timeval timer;
    	timer.tv_sec = 1; //time_interval;
   		timer.tv_usec = 0;
		
    	FD_ZERO(&fd_readSockets);
    	fd_readSockets = master;

    	//int selector = select(max_fd+1,&fd_readSockets,NULL,NULL,&timer);
    	time_t start, stop;
    	time(&start);
    	if(select(max_fd+1,&fd_readSockets,NULL,NULL,&timer) == -1){
    	//if(selector == -1){
    		printf("SHOWS OVER\n");
    		break;
    	}
    	//times++;
    	//time(&stop);
    	//printf("Finnished in %f seconds. \n", difftime(stop, start));
    	//printf("Times: %d\n", timer.tv_usec);
    	//else if(selector == 0){
    		//printf("timer fired\n");
    	if(times >= time_interval){
    		//printf("Time elapsed: %f\n", times);
    		times = 0.0;
    		broadcast(peer_info_array,routing_diagram,0,0);
    	
    	}

    	int breaker = 0;
    	int test = 0;

		if(FD_ISSET(0,&fd_readSockets)){
			test++;
			//handle user input!! stdin is fd 0
			fgets(i, BUFSIZ, stdin);
    		breaker = run_command(i,peer_info_array,routing_diagram);
    		if(breaker != 0){ break; }
    		time(&stop);
    		times = times + difftime(stop,start);
		}

		else if (FD_ISSET(my_listen_fd, &fd_readSockets)) {
			test++;
			packet_count++;
			//printf("Recieved something!\n");
			//char buffer[100];
			struct datagram mydatagram;// = malloc(sizeof(struct datagram));
			struct sockaddr addr;
			socklen_t recieved_length = sizeof(addr);
			int bytes_read;
			bytes_read = recvfrom(my_listen_fd,&mydatagram,sizeof(struct datagram),0,&addr,&recieved_length);
			/*
			printf("Message: %hu\n", mydatagram.num_of_fields);
			printf("Message: %llu\n", mydatagram.recipient1_cost);
			printf("Message: %llu\n", mydatagram.recipient2_cost);
			printf("Message: %llu\n", mydatagram.recipient3_cost);
			printf("Message: %llu\n", mydatagram.recipient4_cost);
			printf("Message: %llu\n", mydatagram.recipient5_cost);
			*/
			if(mydatagram.num_of_fields == 15){
				int ix;
				for(ix = 1; ix <= 5; ix++){
					routing_diagram[ix][mydatagram.recipient1_id] = -1;
				}
				continue;
			}
			if(mydatagram.num_of_fields == 20){
				//printf("I got here!\n");
				routing_diagram[mydatagram.recipient1_id][mydatagram.recipient2_id] = -1;
				continue;
			}
			update_routing_diagram(mydatagram,peer_info_array,routing_diagram);
			time(&stop);
    		times = times + difftime(stop,start);
		}
		if(test == 0) {
			times++;
		}

    	if(breaker != 0){ break; }
    }
	return 0;
}

int read_top(char *top_file,struct peer_info peer_info_array[5],int routing_diagram[5][5]){
	
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(top_file, "r");
    if (fp == NULL){
        //exit(EXIT_FAILURE);
        printf("Invalid topology file.\n");
        //fclose(fp);
        //return 1;
        exit(1);
    }

    int count = 0;
    int num_to_put_in_array;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s", line);
        if(count == 0){
        	num_to_put_in_array = atoi(line);
        	//printf("%s\n", line);
        	//printf("%d\n",num_to_put_in_array);
        }
        else if(count == 1){
        	//continue;
        }
        else if(count >= 2 && count < (2 + num_to_put_in_array)){
        	//printf("testing\n");
        	//printf("%s\n", line);
        	char *id;
			char *ipadd;
			char *l_port;
			char *split = strtok(line, " ");
			int x = 0;
			while(split != NULL){
				if(x == 0){
					id = split;
				}
				if(x == 1){
					split=strtok(NULL," ");
					ipadd = split;
				}
				if(x == 2){
					split=strtok(NULL,"\n");
					l_port = split;
				}
				if(x == 3){
					break;
				}
				x++;
			}
			//printf("%s\n", id);
			//printf("%s\n", ipadd);
			//printf("%s\n", l_port);
			int idint = atoi(id);
			struct peer_info newpeerinfo;
			//strncpy(newpeerinfo.ip_address, ipadd, strlen(ipadd));
			strcpy(newpeerinfo.ip_address, ipadd);
			//printf("ipadd : %s\n", ipadd);
			//strncpy(newpeerinfo.port,l_port,strlen(l_port));
			strcpy(newpeerinfo.port, l_port);
			//printf("l_port : %s\n", l_port);
			newpeerinfo.id = idint;
			char *nameof = findIpName(ipadd);
			//strncpy(newpeerinfo.name,nameof,strlen(nameof));
			strcpy(newpeerinfo.name, nameof);
			//printf("nameof : %s\n", nameof);
			peer_info_array[idint] = newpeerinfo;

        }
        else {
        	char *first;
			char *second;
			char *dist;
			char *split = strtok(line, " ");
			int x = 0;
			while(split != NULL){
				if(x == 0){
					first = split;
				}
				if(x == 1){
					split=strtok(NULL," ");
					second = split;
				}
				if(x == 2){
					split=strtok(NULL,"\n");
					dist = split;
				}
				if(x == 3){
					break;
				}
				x++;
			}
			int firstint = atoi(first);
			int secondint = atoi(second);
			int distint = atoi(dist);
			//printf("%d\n", firstint);c
        	//printf("%d\n", secondint);
        	//printf("%d\n", distint);

        	routing_diagram[firstint][firstint] = 0;

        	routing_diagram[secondint][firstint] = distint;
        	who_am_i = firstint;
        	//my_listen_port = peer_info_array[firstint].port;
        	strcpy(my_listen_port,peer_info_array[firstint].port);
        	strcpy(my_ip,peer_info_array[firstint].ip_address);

        }
        
        count++;
        //printf("Count: %d\n", count);
    }

    fclose(fp);
    if (line)
        free(line);
    //exit(EXIT_SUCCESS);

	return 0;	
}

int main(int argc, char *argv[]){

	char temptime[30];
	strncpy(temptime,argv[4],sizeof(argv[4]) - sizeof("\n"));
	time_interval = atoi(temptime);

	//printf("time interval: %d\n", time_interval);

	struct peer_info *peer_info_array = malloc(sizeof(struct peer_info ) * 5);
	int routing_diagram[5][5];
	int j;
	int x;
	for(x = 1; x <= 5; x++){
		for(j = 1; j <= 5; j++){
			//if(j == 2){
			//	routing_diagram[j][x] = 0;
			//}
			//else { routing_diagram[j][x] = -1; }
			routing_diagram[j][x] = -1;
		}
	}
	//print_routing_diagram(routing_diagram);
	//int i;
	//for(i = 0; i < 5; i++){
	//	peer_info_array[i].id = -1;
	//}
	struct peer_info null;
	strncpy(null.name,"None",30);
	strncpy(null.ip_address,"None",30);
	strncpy(null.port,"None",30);
	null.id = -1;
	//strncpy(null.,"",30);
	int i;
	for(i = 0; i <= 5; i++){
		peer_info_array[i] = null;
	}
	if(strcmp(argv[1],"-t") != 0){
		return 1;
	}
	if(strcmp(argv[3],"-i") != 0){
		return 1;
	}
	char *top_file = argv[2];
	char *interval = argv[4];
	//print_routing_diagram(routing_diagram);
	read_top(top_file, peer_info_array,routing_diagram);
	//printf("who_am_i: %d\n", who_am_i);
	//if(tester != 0){
	//	return 0;
	//}

	//print_servers(peer_info_array);
	//broadcast();
	//csend_mes();
	//print_routing_diagram(routing_diagram);

	my_listen_fd = create_socket(my_listen_port);

	packet_count = 0;

	server_start(peer_info_array, routing_diagram);
	return 0;
}

