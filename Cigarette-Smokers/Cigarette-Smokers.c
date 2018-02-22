#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>


//Global Varibles
pthread_mutex_t MutexThread;
pthread_cond_t agentThreadCon;
sem_t someTobacco, somePaper, someMatches;

//Functions
void *Agent();
void *Tobacco();
void *Paper();
void *Matches();

int main(int argc, char *argv[]){
	pthread_t TobaccoThread[1], PaperThread[1], MatchesThread[1], AgentThread[1];

	//Getting time set up for random numbers
	time_t t;
	srand( (unsigned) time(&t) );

	//Start the threads
	pthread_cond_init( &agentThreadCon, NULL );
	pthread_mutex_init( &MutexThread, NULL );

	//Semaphore init
	sem_init( &someTobacco, 0, 1 );
    	sem_init( &somePaper, 0, 1 );
    	sem_init( &someMatches, 0, 1 );

	//Creating thread
	pthread_create( &AgentThread[0], NULL, Agent, NULL );
	pthread_create( &TobaccoThread[0], NULL, Tobacco, NULL );
	pthread_create( &PaperThread[0], NULL, Paper, NULL );
	pthread_create( &MatchesThread[0], NULL, Matches, NULL );
	
	//Threads join
	pthread_join( AgentThread[0], NULL );
	pthread_join( TobaccoThread[0], NULL );
	pthread_join( PaperThread[0], NULL );
	pthread_join( MatchesThread[0], NULL );

	return 0;
}

void *Agent(){

	int Random, start = 0;

	while( 1 ){
		
		if( start == 0 ){

			sem_wait( &someTobacco );
			sem_wait( &somePaper );
			sem_wait( &someMatches );
			start = 1;

		}else{

			pthread_cond_wait( &agentThreadCon , &MutexThread );

		}

		//Grab Random Number
		Random = rand() % 3;

		if( Random == 1 ){

			printf( "---------- Agent Has Supplies: Paper & Maches ----------\n" );            		
			fflush( stdout );
			sem_post( &someTobacco );

			sleep( 2 );

			sem_wait( &someTobacco );
			
	
		}else if( Random == 2 ){

			printf( "---------- Agent Has Supplies: Tobacco & Maches ----------\n" );            		
			fflush( stdout );
			sem_post( &somePaper );

			sleep( 2 );

			sem_wait( &somePaper );

		}else if( Random == 3 ){

			printf( "---------- Agent Has Supplies: Tobacco & Paper ----------\n" );            		
			fflush( stdout );
			sem_post( &someMatches );

			sleep( 2 );

			sem_wait( &someMatches );

		}

	}

}

void *Tobacco(){

	while( 1 ){
		
		sleep( 2 );
		
		//Print Wait
		printf( "---------- Tobacco Waiting For Supplies ----------\n" );            		
		fflush( stdout );
		
		//Wait for the supplier
		sem_wait( &someTobacco );

		//Locking Mutex
		pthread_mutex_lock( &MutexThread );

		//Print got supplies
		printf( "---------- Tobacco Got Supplies Now Making Cigarettes\n" );
		fflush( stdout );

		//Making
		sleep( 5 );

		//Print smoking
		printf( "Tobacco Smoking Cigarettes ----------\n" );
		fflush( stdout );

		//Smoking
		sleep( 5 );
	
		//Unlocks
		sem_post( &someTobacco );
		pthread_cond_signal( &agentThreadCon );
		pthread_mutex_unlock( &MutexThread );
	
	}

}

void *Paper(){

	while( 1 ){
		
		sleep( 2 );
		
		//Print Wait
		printf( "---------- Paper Waiting For Supplies ----------\n" );            		
		fflush( stdout );
		
		//Wait for the supplier
		sem_wait( &somePaper );

		//Locking Mutex
		pthread_mutex_lock( &MutexThread );

		//Print got supplies
		printf( "---------- Paper Got Supplies Now Making Cigarettes\n" );
		fflush( stdout );

		//Making
		sleep( 5 );

		//Print smoking
		printf( "Paper Smoking Cigarettes ----------\n" );
		fflush( stdout );

		//Smoking
		sleep( 5 );
	
		//Unlocks
		sem_post( &somePaper );
		pthread_cond_signal( &agentThreadCon );
		pthread_mutex_unlock( &MutexThread );
	
	}

}

void *Matches(){

	while(1){
		
		sleep( 2 );
		
		//Print Wait
		printf( "---------- Maches Waiting For Supplies ----------\n" );            		
		fflush( stdout );
		
		//Wait for the supplier
		sem_wait( &someMatches );

		//Locking Mutex
		pthread_mutex_lock( &MutexThread );

		//Print got supplies
		printf( "---------- Matches Got Supplies Now Making Cigarettes\n" );
		fflush( stdout );

		//Making
		sleep( 5 );

		//Print smoking
		printf( "Matches Smoking Cigarettes ----------\n" );
		fflush( stdout );

		//Smoking
		sleep( 5 );
	
		//Unlocks
		sem_post( &someMatches );
		pthread_cond_signal( &agentThreadCon );
		pthread_mutex_unlock( &MutexThread );
	
	}

}
