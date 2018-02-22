#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>

//Global Varibles
pthread_cond_t SearchCondThread;
pthread_cond_t InsertCondThread;
pthread_cond_t DeleteCondThread;
sem_t noSearcher, noInserter, noDeleter;

//Struct
struct linkList {
	int Number;
	struct linkList *Next;
} *Head;


//Functions
void *Searches(void *arg);
void *Inserts(void *arg);
void *Deleters(void *arg);
int randomNumber(int Min, int Max);


//Main
int main(int argc, char *argv[]){
	pthread_t SearchThread[3], InsertThread[3], DeleteThread[3];

	//Semaphore init
    	sem_init( &noSearcher, 0, 3 );
    	sem_init( &noInserter, 0, 1 );
    	sem_init( &noDeleter, 0, 1 );

	//Getting time set up for random numbers
   	time_t t;
	srand( (unsigned) time(&t) );

	//Init linkList
	struct linkList *StartLinkList;
	StartLinkList = ( struct linkList * )malloc( sizeof( struct linkList ) );
	StartLinkList->Number = randomNumber( 1, 100 );
	StartLinkList->Next = NULL;
	Head = StartLinkList;

	int i, id[2];
	for( i = 0; i < 3; i++){
		//Gives id numbers
		id[i]=i;

		//Creating thread
		pthread_create( &SearchThread[i], NULL, Searches, (void*)(&id[i]) );
		pthread_create( &InsertThread[i], NULL, Inserts, (void*)(&id[i]) );
		pthread_create( &DeleteThread[i], NULL, Deleters, (void*)(&id[i]) );
	}

	for( i = 0; i < 3; i++){
		//Threads join
		pthread_join( SearchThread[i], NULL );
		pthread_join( InsertThread[i], NULL );
		pthread_join( DeleteThread[i], NULL );
	}

	return 0;
}

//Searchers - merely examine the list; hence they can execute concurrently with each other.
void *Searches(void *arg){

	int searcher_id=*(int *)arg;

	while(1){

		//Checks for if the deleter is going off.
		sem_wait( &noSearcher );

		//Start
		printf( "---------- Searcher %d Starts ----------\n",searcher_id );            		
		fflush( stdout );

		struct linkList *LittleLink = Head;
		int i = 1;

		while( LittleLink->Next != NULL ){

			//Prints what the Searcher is currently looking at
			printf( "Searching looking at link %d: %d\n", i, LittleLink->Number );
			i++;
			LittleLink = LittleLink->Next;
			fflush( stdout );

		}

		//Seacher ends
		printf( "---------- Searcher %d Ends ----------\n" ,searcher_id);     
		fflush( stdout );   
	
		//Unlocks   		
		sem_post( &noSearcher );

		//Sleeps
        	sleep( 2 );

	}
}

//Inserts - add new items to the end of the list; insertions must be mutually exclusive to preclude two inserters from inserting new items at about the same time.
void *Inserts(void *arg){

	int Inserter_id=*(int *)arg;

	while(1){

		//Inserters must wait for each other to finish and wait for the destoyer to stop
		sem_wait(&noInserter);

		int i = 1;
		struct linkList *Check = Head;
		
		while( Check->Next != NULL ){
			Check = Check->Next;
			i++;
		}	

		//Max link list can be
		if(i < 32){

			//Start
			printf( "Insert %d Starts - Blocking other Inserters\n" , Inserter_id);       
			fflush( stdout );     		

			struct linkList *LittleLink = Head;
			i = 1;

			//Getting the last link
			while( LittleLink->Next != NULL ){
				LittleLink = LittleLink->Next;
				i++;
			}

			//Creating new link and adding it to link list
			struct linkList *NewLink;
			NewLink = ( struct linkList * )malloc( sizeof( struct linkList ) );
			NewLink->Number = randomNumber( 1, 100 );
			NewLink->Next = NULL;
			LittleLink->Next = NewLink;

			//Print the new link
			printf( "Inserter %d inserted link %d: %d ------------------------------------------\n" ,Inserter_id , (i + 1) ,NewLink->Number );
			fflush( stdout );

			//End
			printf( "Insert %d Ends\n", Inserter_id ); 
			fflush( stdout );           		

		}

		//Unlock
        	sem_post( &noInserter );
	
		//Sleeps
		sleep( 2 );

	}
}

//Deleters -
void *Deleters(void *arg){
	int De_id=*(int *)arg;

	sleep(20);

	while(1){

		//Check if insert is locked if not then lock it and lock search
		sem_wait( &noDeleter );
		sem_wait( &noInserter );
		sem_wait( &noSearcher );
		sem_wait( &noSearcher );
		sem_wait( &noSearcher );

		int j = 0, sleepTime = 0;
		struct linkList *Check = Head;
		
		//Checks the size of the link list
		while( Check->Next != NULL ){
			Check = Check->Next;
			j++;
		}

		if(j > 1){		

			//Starts
			printf( "Deleter %d Starts - Blocking Inserters and Searches\n", De_id);   
			fflush( stdout );         		

			int i, Spot = randomNumber(1, j);
			struct linkList *LittleLink = Head;
		
			//Gets location of the link that it is deleting
			for( i = 0; i < Spot - 1; i++ ){
				LittleLink = LittleLink->Next;
			}

			struct linkList *DeleteLink = LittleLink->Next, *NextLink;
		
			if( Spot == j ){
				NextLink = NULL;

			}else {
				NextLink = DeleteLink->Next;
			}

			//Prints the link that it is deleting
			printf( "Deleter deletes link %d: %d\n" , (Spot + 1) ,DeleteLink->Number ); 
			fflush( stdout );
           		
			//Deletes a link
			free(DeleteLink);

			//Repoint the new list correctly
			LittleLink->Next = NextLink;

			//End
			printf( "Deleter %d Ends\n", De_id);       
			fflush( stdout );     		
			
		}
		
		//Unlock
        	sem_post(&noSearcher);
        	sem_post(&noSearcher);
        	sem_post(&noSearcher);
        	sem_post(&noInserter);
        	sem_post(&noDeleter);
	
		//Sleeps
		if( j > 16){
			sleep( randomNumber( 1, 2 ) );
		}else{
			sleep( randomNumber( 10, 15 ) );
		}
	}
}

//Extra Functions
int randomNumber(int Min, int Max){
	return rand() % Max + Min;
}
