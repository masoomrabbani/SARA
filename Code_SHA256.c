#include "contiki.h"
#include "net/rime.h"
#include "random.h"
#include "sys/node-id.h"
#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "sha256.c"

//Parameters
#define d 10	//used to compute hash
#define q 109	//prime no. used to compute hash
#define anum 2	//No. of nodes in group A
#define bnum 2	//No. of nodes in group B
#define cnum 2	//No. of nodes in group C
#define dnum 2	//No. of nodes in group D
#define enumb 2	//No. of nodes in group E
#define fnum 2	//No. of nodes in group F
#define gnum 2	//No. of nodes in group G
#define hnum 2	//No. of nodes in group H
#define inum 2	//No. of nodes in group I
#define jnum 2	//No. of nodes in group J

int hashCommon = 0;
int timeTrack=1;
int i=0, j=0;

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/

/* These hold the broadcast and unicast structures, respectively. */
static struct broadcast_conn broadcast;

static void broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
    printf("broadcast message received from %d.%d: Hash Value:'",
         from->u8[0], from->u8[1]);

    char buffer[36];
    sprintf(buffer,"%s", (char *)packetbuf_dataptr());
    
    for(i=0;i<32;i++){
		printf("%.2x", buffer[i]);
    }
    printf("'\n");
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

int* getShort(char* str){
	int i, len;
    static int freq[26];
    len = strlen(str);

    /* Initialize frequency of each character to 0 */
    for(i=0; i<26; i++)
    {
        freq[i] = 0;
    }

    /* Find total number of occurrences of each character */
    for(i=0; i<len; i++)
    {
        /* If the current character is lowercase alphabet */
        if(str[i]>='a' && str[i]<='z')
        {
            freq[str[i] - 97]++;
        }
        else if(str[i]>='A' && str[i]<='Z')
        {
            freq[str[i] - 65]++;
        }
    }
    return freq;
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer periodic;
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  /* Allow some time for the network to settle. */
  etimer_set(&et, 120 * CLOCK_SECOND);
  PROCESS_WAIT_UNTIL(etimer_expired(&et));

  static int countA=0, countB=0, countC=0, countD=0, countE=0, countF=0, countG=0, countH=0, countI=0, countJ=0, iterA = 1;

  while(1) {

    /* Send a packet every 1 seconds. */
    if(etimer_expired(&periodic)) 
    {
      etimer_set(&periodic, CLOCK_SECOND * 1 );
      etimer_set(&et, random_rand() % (CLOCK_SECOND * 1));
    }
    timeTrack += 1;

    PROCESS_WAIT_EVENT();


    if(etimer_expired(&et)) 
    {
      if(countA < 101){
        countA++;
      }
      else if(countB < 51){
        countB++;
      }
      else if(countC < 101){
        countC++;
      }
      else if(countD < 51){
        countD++;
      }
      else if(countE < 101){
        countE++;
      }
      else if(countF < 51){
        countF++;
      }
	  else if(countG < 101){
        countG++;
      }
	  else if(countH < 51){
        countH++;
      }
	  else if(countI < 101){
        countI++;
      }
	  else if(countJ < 51){
        countJ++;
      }
      else if(countJ == 51 && iterA < 2){
	countA = 0;
	iterA += 1;
      }
      
      //printf("Checkpoint 1\n");

      if(countA < 101 && rimeaddr_node_addr.u8[0] < (anum+1))
      {
        //printf("A[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countA);
	if(countA == 100){
	      	char str[]="printf('A: Output %d', countA);'if(countA == 100){'printf('\nA exiting. B & C starting');'char buffer[6];'itoa(buf,buffer,10);'const void *message= buffer;'packetbuf_copyfrom(message, 32);'broadcast_send(&broadcast);'printf('\nbroadcast message sent. Hash value of A: %d', buf);'}'printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
	      	
	      	if(iterA == 1){
			const void *message= text3;
			packetbuf_copyfrom(message, 32);
			broadcast_send(&broadcast);
			printf("Output %d || Hash: '", countA);
			
			for(i=0;i<32;i++){
				printf("%.2x", text3[i]);
			}
			printf("' || Timestamp: %d || Service id: %d", timeTrack, 'A');
			printf("\nA[Node: %d] exiting. broadcast message sent. B nodes starting",rimeaddr_node_addr.u8[0]);
		}
		else{
			printf("Output %d || Hash: '", countA);
			
			for(i=0;i<32;i++){
				printf("%.2x", text3[i]);
			}
			printf("' || Timestamp: %d", timeTrack);
		}
		
	}
	printf("\n");
      }

      else if(countA == 101 && countB < 51 && (anum < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+1))
      {
	//printf("B[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countB);
	if(countB == 50){
		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countB);if(countB == 50){printf('\n%d exiting. broadcast message sent by %d. E,F,G starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countB);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nB[Node: %d] exiting. broadcast message sent. C nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }

      else if(countA == 101 && countB == 51 && countC < 101 && (anum+bnum < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+cnum+1))
      {
	//printf("C[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countC);
	if(countC == 100){
		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countC);if(countC == 100){printf('\n%d exiting. broadcast message sent by %d. H.I.J starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countC);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nC[Node: %d] exiting. broadcast message sent. D nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }

      else if(countA == 101 && countB == 51 && countC == 101 && countD < 51 && (anum+bnum+cnum < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+cnum+dnum+1))
      {
	//printf("D[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countD);
	if(countD == 50){
		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countD);if(countD == 50){printf('\n%d exiting. broadcast message sent by %d. E starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countD);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nD[Node: %d] exiting. broadcast message sent. E nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }

      else if(countA == 101 && countB == 51 && countC == 101 && countD == 51 && countE < 101 && (anum+bnum+cnum+dnum < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+cnum+dnum+enumb+1))
      {
	//printf("E[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countE);
	if(countE == 100){
		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countE);if(countE == 50){printf('\n%d exiting. broadcast message sent by %d. F Nodes starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countE);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nE[Node: %d] exiting. broadcast message sent. F nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }

      else if(countA == 101 && countB == 51 && countC == 101 && countD == 51 && countE == 101 && countF < 51 && (anum+bnum+cnum+dnum+enumb < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+cnum+dnum+enumb+fnum+1))
      {
	//printf("F[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countF);
	if(countF == 50){
		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countF);if(countF == 50){printf('\n%d exiting. broadcast message sent by %d. G Nodes starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countF);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nF[Node: %d] exiting. broadcast message sent. G nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }
	  
	  else if(countA == 101 && countB == 51 && countC == 101 && countD == 51 && countE == 101 && countF == 51 && countG < 101 && (anum+bnum+cnum+dnum+enumb+fnum < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+cnum+dnum+enumb+fnum+gnum+1))
      {
	//printf("G[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countG);
	if(countG == 50){
		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countG);if(countG == 50){printf('\n%d exiting. broadcast message sent by %d. H Nodes starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countG);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nG[Node: %d] exiting. broadcast message sent. H nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }
	  
	  else if(countA == 101 && countB == 51 && countC == 101 && countD == 51 && countE == 101 && countF == 51 && countG == 101 && countH < 51 && (anum+bnum+cnum+dnum+enumb+fnum+gnum < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+cnum+dnum+enumb+fnum+gnum+hnum+1))
      {
	//printf("H[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countH);
	if(countH == 50){
		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countH);if(countH == 50){printf('\n%d exiting. broadcast message sent by %d. I Nodes starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countH);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nH[Node: %d] exiting. broadcast message sent. I nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }
	  
	  else if(countA == 101 && countB == 51 && countC == 101 && countD == 51 && countE == 101 && countF == 51 && countG == 101 && countH == 51 && countI < 101 && (anum+bnum+cnum+dnum+enumb+fnum+gnum+hnum < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+cnum+dnum+enumb+fnum+gnum+hnum+inum+1))
      {
	//printf("I[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countI);
	if(countI == 50){

		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countI);if(countI == 50){printf('\n%d exiting. broadcast message sent by %d. J Nodes starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countI);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nI[Node: %d] exiting. broadcast message sent. J nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }
	  
	  else if(countA == 101 && countB == 51 && countC == 101 && countD == 51 && countE == 101 && countF == 51 && countG == 101 && countH == 51 && countI == 101 && countJ < 51 && (anum+bnum+cnum+dnum+enumb+fnum+gnum+hnum+inum < rimeaddr_node_addr.u8[0]) && (rimeaddr_node_addr.u8[0] < anum+bnum+cnum+dnum+enumb+fnum+gnum+hnum+inum+jnum+1))
      {
	//printf("J[Node:%d]: Output %d", rimeaddr_node_addr.u8[0], countJ);
	if(countJ == 50){
		
		char str[]="printf('%d: Output %d',rimeaddr_node_addr.u8[0], countJ);if(countJ == 50){printf('\n%d exiting. broadcast message sent by %d. J Nodes starting',rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[0]);}printf('\n');";
	      	char text1[27];
	      	
	      	//=========Compressing input string using encryption==========================
		int *freq;
		freq = getShort(str); //returns count of each character(a-z) as string freq in the input string str

		//Convert ASCII value to characters
		
		for(i=0; i<26; i++){
			char c = 58+ *(freq+i);
			text1[i] = c;
		}
		text1[26]='\0';
		//============================================================================

		char buf[SHA256_BLOCK_SIZE];

		//Performing SHA-256
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, text1, strlen((const char*)text1));
		sha256_final(&ctx, buf);

		char text2[100];
		char text3[100];

		//=========Removing null characters from SHA256 output==========
		strcpy(text2,buf);
		j=0;
		for(i = 0; i < strlen(text2); i++){
		    if(text2[i]!='\0'){
		    	text3[j]=text2[i];
		    	j++;
			}
		}
		//==============================================================
		
		printf("Output %d || Hash: '", countJ);
		
		for(i=0;i<32;i++){
			printf("%.2x", text3[i]);
		}
		printf("' || Timestamp: %d || Service id: %d", timeTrack, 'B');
		const void *message= text3;
		packetbuf_copyfrom(message, 32);
		broadcast_send(&broadcast);
		printf("\nJ[Node: %d] exiting. broadcast message sent. A nodes starting",rimeaddr_node_addr.u8[0]);
	}
	printf("\n");
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/



