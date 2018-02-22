/*Tanenbaum's solution: philosopher only get forks when both left and right available.
which means philosophers pick forks when their neighbors are not eating,
return both forks when finish eating.
Solution is not starvation-free,Imagine that we are trying to starve Philosopher 0.
Initially, 2 and 4 are at the table and 1 and 3 are hungry. Imagine that 2 gets up and 1 sits down; then 4 gets up and 3 sits down.
Now we are in the mirror image of the starting position.We could repeat the cycle indefinitely and Philosopher 0 would starve.

http://www.cs.grinnell.edu/~weinman/courses/CSC213/2012F/labs/philosophers.html
https://www.cs.indiana.edu/classes/p415-sjoh/hw/project/dining-philosophers/index.htm

But in this implementation thinking and eating time are random, the starvation can be avoid.
*/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include "mt19937ar.c"

#define philos_num 5
#define LEFT (phi_id+philos_num-1)%philos_num
#define RIGHT (phi_id+1)%philos_num

char *philos_names[]= {"Kaiyuan","Trevor","Sophia","Plato","Kevin"};
enum { THINK , HUNGRY , EAT } pflag[philos_num]; // philosopher flag

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER; //initialize mutex only one philosopher can get forks and put forks at one moment
sem_t S[N];

void think(int phi_id){
   printf("philosopher %s is thinking now !\n",philos_names[phi_id]);
   sleep(RandomNumberGenerator(20,1));
}

void eat(int phi_id){
   printf("philosopher %s is eating now, takes fork %d and %d\n",philos_names[phi_id],LEFT+1,phi_id+1);
   sleep(RandomNumberGenerator(9,2));
}

void test(int phi_id){
   //eat when neighbors are not eating
   if(pflag[phi_id]==HUNGRY&&pflag[LEFT]!=EAT&&pflag[RIGHT]!=EAT){
      pflag[phi_id]=EAT;
      printf("philosopher %s is eating now, takes fork %d and %d\n",philos_names[phi_id],LEFT+1,phi_id+1);
      sem_post(&S[phi_id]);
    }
}

void get_forks(int phi_id){
   pthread_mutex_lock(&mutex);
   pflag[phi_id]=HUNGRY;
   test(phi_id);
   pthread_mutex_unlock(&mutex);
   sem_wait(&S[phi_id]);
}

void put_forks(int phi_id){
   pthread_mutex_lock(&mutex);
   pflag[phi_id]=THINK;
   printf("Philosopher %s is putting fork %d and %d down\n",philos_names[phi_id],LEFT+1,phi_id+1);
   printf("philosopher %s is thinking now !\n",philos_names[phi_id]);
   test(LEFT);
   test(RIGHT);
   pthread_mutex_unlock(&mutex);
}


void *philosopher(void *arg){
   int phi_id=*(int *)arg;
   while(1){	  
      //think(phi_id);	   
      sleep(RandomNumberGenerator(20,1));	   
      get_forks(phi_id);	   
      sleep(RandomNumberGenerator(9,2));	   
      //eat(phi_id);	   
      put_forks(phi_id);	
   }
}

int main(int argc, char *argv[]){
   int i;
   pthread_t phi[philos_num];
   int id[philos_num];
   //initialize semaphore
   for(i=0;i<philos_num;i++){
      sem_init(&S[i],0,0);
   }
   for(i=0;i<philos_num;i++){
      id[i]=i;
      pthread_create(&phi[i],NULL,philosopher,(void*)(&id[i]));
      printf("Philosopher %s is thinking\n",philos_names[i]);
   }
   for(i=0;i<philos_num;i++){
      pthread_join(phi[i],NULL);
   }
}

int RandomNumberGenerator(int Max, int Min){
   int randNum = 0;
   randNum = (int)genrand_int32();
   randNum = abs(randNum);
   randNum %= (Max - Min) + Min;
   if (randNum < Min){	//Because modual can bring the number below the min
      randNum = RandomNumberGenerator(Max, Min);
   }
   else if( randNum > Max) { //Never be used but we have it
      randNum = RandomNumberGenerator(Max, Min);
   }
   return randNum;
}
