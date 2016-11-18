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

int who_am_i;

void pretty_print_routing_diagram(int routing_diagram[6][6]){
	
	printf("\n");
	printf("     1. 2. 3. 4. 5.\n");
	printf("    ________________\n");
	int i;
	int j;
	for(i = 1; i <=5; i++){
		printf("%d. | ",i);
		for(j = 1; j <=6; j++){
			if(j > 5){
				printf("\n");
				break;
			}
			else {
				//printf("I AM HERE\n");
				//if(i == who_am_i || j == who_am_i){
					//printf(RED"%d "RESET, routing_diagram[j][i]);
					//printf(RED"%d "RESET, newdiagram[j][i]);
				//}
				//else {
					printf("%d ", routing_diagram[j][i]);
					//printf("%d ", newdiagram[j][i]);
				//}
			}
		}
	}
}

void bellmanford(int routing_diagram[6][6],int whoami){

	int i,j;

	int bestcost[6];
	int num_paths[6];
	for(i = 0; i <=5;i++){
		if(routing_diagram[whoami][i] == -1){
			bestcost[i] = 190567421;
			num_paths[i] = -1;
			continue;
		}
		bestcost[i] = routing_diagram[whoami][i];
		num_paths[i] = 1;
	}

	int xi;
	for(xi = 1; xi <=5;xi++){
		for(i = 1; i <=5; i++){
			for(j =1; j<=5;j++){
				if(routing_diagram[i][j] != -1){
					if((bestcost[i] + routing_diagram[i][j]) < bestcost[j]){
						if(i == 4 || j == 4){
						//printf("bestcost[i] = %d \n routing_diagram[i][j] = %d \n bestcost[j] = %d \n", bestcost[i], routing_diagram[i][j], bestcost[j]);
						}
						bestcost[j] = bestcost[i] + routing_diagram[i][j];
					}
				}
			}
		}
	}
	/*
	printf("From: %d\n", whoami);
	for(i = 1; i<=5;i++){
		printf("Space: %d with Cost: %d\n", i, bestcost[i]);
	}
	*/
	for(i = 1; i <=5;i++){
		routing_diagram[whoami][i] = bestcost[i];
	}
}

int main(int argc, char *argv[]){

	int routing_diagram[6][6];
	int i,j;
	for(i = 0; i <=5;i++){
		for(j=0;j<=5;j++){
			routing_diagram[i][j] = -1;
		}
	}
	routing_diagram[1][1] = 0;
	routing_diagram[1][2] = 6;
	routing_diagram[1][3] = 7;
	routing_diagram[1][4] = 14;
	routing_diagram[1][5] = 2;

	routing_diagram[2][1] = 6;
	routing_diagram[2][2] = 0;
	routing_diagram[2][3] = 2;
	routing_diagram[2][4] = 2;
	routing_diagram[2][5] = 13;

	routing_diagram[3][1] = 7;
	routing_diagram[3][2] = 2;
	routing_diagram[3][3] = 0;
	routing_diagram[3][4] = 1;
	routing_diagram[3][5] = 1;

	routing_diagram[4][1] = 14;
	routing_diagram[4][2] = 2;
	routing_diagram[4][3] = 1;
	routing_diagram[4][4] = 0;
	routing_diagram[4][5] = 47;

	routing_diagram[5][1] = 2;
	routing_diagram[5][2] = 13;
	routing_diagram[5][3] = 1;
	routing_diagram[5][4] = 47;
	routing_diagram[5][5] = 0;

	who_am_i = 1;

	pretty_print_routing_diagram(routing_diagram);
	int x;
	for(x = 1; x <=5;x++){
		bellmanford(routing_diagram,x);
	}
	pretty_print_routing_diagram(routing_diagram);

	return 0;
}