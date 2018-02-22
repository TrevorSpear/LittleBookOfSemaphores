#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "mt19937ar.c"

struct concurentNumber { 
	int randomNumber;
	int waitTime;
};

struct concurentNumberArray{
	int ProducerIndex;
	int ConsumerIndex;
	int Turn;
	struct concurentNumber numberArray[64];
	pthread_cond_t ProducerCondThread;
	pthread_cond_t ConsumerCondThread;
	pthread_mutex_t MutexThread;
};

//Global Variable
struct concurentNumberArray concArray;

//Functions in order
void *Producer();
void *Consumer();
int RandomNumberGenerator(int Max, int Min);
bool ArrayIsEmpty();
bool ArrayIsFull();

int main(int argc, char *argv){
	pthread_t ProducerThread, ConsumerThread;

	//Sets up ints and 
	int i;
	concArray.ProducerIndex = 0;
	concArray.ConsumerIndex = 0;
	concArray.Turn = 0;

	for(i = 0; i < 64; i++){
		concArray.numberArray[i].randomNumber = 0;
	}
	
	//Start the threads
	pthread_cond_init( &concArray.ProducerCondThread, NULL );
	pthread_cond_init( &concArray.ConsumerCondThread, NULL );
	pthread_mutex_init( &concArray.MutexThread, NULL );
	
	//Add a producer
	pthread_create( &ProducerThread, NULL, Producer, NULL );
	
	for(i = 0; i < 10; i++){
		//Add a consumer
		pthread_create( &ConsumerThread, NULL, Consumer, NULL );
		pthread_join( ConsumerThread, NULL );
	}

	return 0;
}

void *Producer(){
	//If a producer thread has an item to put in the buffer while the buffer is full, it blocks until a consumer removes an item.

	while(1){

		//Locks a mutex
		pthread_mutex_lock(&concArray.MutexThread);

		//Make sure the int array isn't empty
		while( ArrayIsFull() || concArray.Turn == 2){
			//if it is then wait until it has space (Block)
			pthread_cond_wait(&concArray.ProducerCondThread, &concArray.MutexThread);
		}
		
		//Producer's wait time
		int waitTime = RandomNumberGenerator(7, 3);

		//Wait
		sleep(waitTime);

		//Gets random number for the randomNumebr and waitTime for consumer
		concArray.numberArray[concArray.ProducerIndex].randomNumber = RandomNumberGenerator(100, 1);
		concArray.numberArray[concArray.ProducerIndex].waitTime = RandomNumberGenerator(8, 3);
	
		//Prints out the number that was just made	
		//printf("Produced: %d\n", concArray.numberArray[concArray.ProducerIndex].randomNumber);

		//Goes to next index (If over 32 then restart)
		concArray.ProducerIndex++;
		if( concArray.ProducerIndex > 31 ){
			concArray.ProducerIndex = 0;
		}

		//The Producer is limited to the amount of turns it gets to go before the consumer can go	
		concArray.Turn++;
	
		//Unlock mutex and waking up thread
		pthread_cond_signal(&concArray.ConsumerCondThread);
		pthread_mutex_unlock(&concArray.MutexThread);

		//Print out the random number array
		/*int i;
		for(i = 0; i < 64; i++){
			printf("%d: %d\n", i, concArray.numberArray[i].randomNumber);
		}*/
		
	}
}

void *Consumer(){
	//If a consumer thread arrives while the buffer is empty, it blocks until a producer adds a new item.
	//Sleeps for the amount of seconds that are in the struct
	
	while(1){

		//Locks a mutex
		pthread_mutex_lock(&concArray.MutexThread);

		//Make sure the int array isn't empty
		while( ArrayIsEmpty() ){
			//If it is then wait until it isn't empty (Block)
			pthread_cond_wait(&concArray.ConsumerCondThread, &concArray.MutexThread);
		}
	
		//Gets Numbers
		int waitTime = concArray.numberArray[ concArray.ConsumerIndex ].waitTime;
		int randNum = concArray.numberArray[ concArray.ConsumerIndex ].randomNumber;

		//Resets
		concArray.numberArray[concArray.ConsumerIndex].waitTime = 0;
		concArray.numberArray[concArray.ConsumerIndex].randomNumber = 0;

		//Goes to next index (If over 32 then restart)
		concArray.ConsumerIndex++;
		if( concArray.ConsumerIndex > 31 ){
			concArray.ConsumerIndex = 0;
		}

		//Unlock mutex and waking up thread
		pthread_cond_signal(&concArray.ProducerCondThread);
		pthread_mutex_unlock(&concArray.MutexThread);
		
		//Wait
		sleep(waitTime);

		//Print out number
		printf( "Random Number: %d\n", randNum);
		fflush(stdout);

		//End
		concArray.Turn = 0;
		
	}
}

int RandomNumberGenerator(int Max, int Min){
	unsigned int eax, ebx, ecx, edx;
	int randNum = 0;
	
	//Need to do this to eax
	eax = 0x01;
	
	//Sets all registers
	__asm__ __volatile__(
		"cpuid;" 
		: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) 
		: "a"(eax)
	);

	//Checks if you can access the registers in a certain way
    	if (ecx & 0x40000000) {
		//Using rdRand
   		//puts("Using RDRAND.\n");
        	unsigned char qm;
        	__asm__ volatile(
                	"rdrand %0 ; setc %1"
                	: "=r" (randNum), "=qm" (qm)
        	);

	}else {
        	//Using Mersenne Twister (mt199937ar.c)
		//puts("Using Mersenne Twister.\n");
        	randNum = (int)genrand_int32();
    	}

	//Absolute value
	randNum = abs(randNum);

	//Getting a reasonable number
	randNum %= (Max - Min) + Min;

    	if (randNum < Min){	//Because modual can bring the number below the min
		randNum = RandomNumberGenerator(Max, Min);

	}else if( randNum > Max) { //Never be used but we have it
		randNum = RandomNumberGenerator(Max, Min);
    	}

    	return randNum;
}

//Checks if the number array is empty
bool ArrayIsEmpty(){
	int i;
	for(i = 0; i < 32; i++){
		if(concArray.numberArray[i].randomNumber != 0){
			return false;
		}
	}

	return true;
}

//Checks if the array is full
bool ArrayIsFull(){
	int i;
	for(i = 0; i < 32; i++){
		if(concArray.numberArray[i].randomNumber == 0){
			return false;
		}
	}

	return true;
}
