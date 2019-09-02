/*
compile command : g++ -pthread -O pth.cpp -o pth
*/


#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
using namespace std;

#define BARRIER_COUNT 100

	/*Global variable boolean check*/
	bool End=false;
	
	/*declare initial iteration number*/	
	int iter_num =1;
	
	/*Read input from console*/
	int PROBLEMSIZE, MAX;
	

	int** cur_gen;
	int** next_gen;
	/*Read input Thread Number from parameter*/
	int thread_count;
	
	// initial thread count
	int barrier_thread_count = 0;
	
	// Barrier //
	pthread_mutex_t barrier_mutex;
	pthread_cond_t ok_to_proceed;
	

double gettime (void){
	struct timeval tvalue;
	
	gettimeofday (&tvalue, NULL);
	
	
	return ((double) tvalue.tv_sec + (double)tvalue.tv_usec/1000000.0);
}
// Initial pointer function

void *gameoflife(void* rank);



/*********************************************************************/	
int main(int argc, char* argv[])
{

	PROBLEMSIZE = atoi(argv[1]);	
	
/*Store MAX from argv[2], atoi = is the function to convert array to integer*/
	MAX = atoi(argv[2]);

/*-----------------Thread---------------*/
	
	thread_count = atol(argv[3]);
	
	long thread;
	pthread_t* thread_handles;

//cout << "Thread_count "<< thread_count <<endl;
	
	thread_handles = (pthread_t*) malloc (thread_count * sizeof(pthread_t));
	
	
	pthread_mutex_init(&barrier_mutex, NULL);
 	pthread_cond_init(&ok_to_proceed, NULL);
	
/*row and column index*/
	int x,y;
/*reset time for random 0,1*/
	srand( time(NULL) );
/*starttime, endtime*/
	double starttime,endtime;
	

/**********************Initial 2 dimensional arrays**********************************/

/*First create the first column of 2D array*/	 	 
/*Declare Global variable*/
	  cur_gen = new int*[PROBLEMSIZE+2];
	  next_gen = new int*[PROBLEMSIZE+2]; 
	  
	  
	  for (x=0;x<PROBLEMSIZE+2;x++)
		{
/*Create 2nd dimension for the current generation and the next generation*/
			cur_gen[x] = new int[(PROBLEMSIZE+2)];
			next_gen[x] = new int[(PROBLEMSIZE+2)];
		}
		
	
/*Use 2 for loop to assigned 0 to every cell*/
	for (x=0;x<PROBLEMSIZE+2;x++){
		for(y=0;y<PROBLEMSIZE+2;y++){
/* assign every cells as 0*/
			cur_gen[x][y] = 0;
			next_gen[x][y] = 0;
			}
		}
/*Random the 1st iteration of the current generation*/	
	for (x=1;x<PROBLEMSIZE+1;x++){
		for(y=1;y<PROBLEMSIZE+1;y++){
					
/*Store random binary to an array*/
			cur_gen[x][y] = rand()%2;
		}	
	}
	
/*************************End Initial 2 dimensional array***************************************/
	
	
/**********************Print First Generation************************/	

#ifdef p1
	cout<<"######################\nTHE FIRST GENERATION"<<
	"\n######################\n";
	for (x=1;x<PROBLEMSIZE+1;x++)
		{
		for(y=1;y<PROBLEMSIZE+1;y++)
			{
			/*print initial array*/
			cout<<cur_gen[x][y]<<" ";
			}
			cout<<endl;
		}	
#endif
/*********************************************************************/	
		
	

/*Record starttime*/
	starttime = gettime();
		

/*----------------------------------------------------------------------------------*/

for (thread =0; thread < thread_count; thread++)
{
	//cout<<"thread# "<< thread <<endl;
	pthread_create(&thread_handles[thread], NULL, gameoflife, (void*) thread);

}


for (thread = 0; thread < thread_count; thread++){
	pthread_join(thread_handles[thread],NULL);
	
}


/*----------------------------------------------------------------------------------*/
	

/*Record endtime*/
	endtime= gettime();
	
/*Print time*/
	cout << "Time taken (seconds): " << endtime-starttime << endl;

/*Delete 2 dimensional array*/

	 for (x = 0; x < PROBLEMSIZE+2;x++ ) {
      delete[] cur_gen[x];
      delete[] next_gen[x];

   	}
  
  		delete[] cur_gen;
		delete[] next_gen;
	/****************************************/
		
	pthread_mutex_destroy(&barrier_mutex);
	pthread_cond_destroy(&ok_to_proceed);	
	free(thread_handles);

		
	return 0;
}




/*Declare of Life*/
void *gameoflife(void*  rank){



long thread_id = (long)rank;

//cout << "thread id : "<< thread_id<<endl;

int start;
int stop;

int x,y;
/*while the game is not end*/
	while (!End)
	{
		
/*If number of iteration = maximum number of iteration, game end*/
		if(iter_num==MAX )
		{

		    End=true;
		}
		

		else
		{
		
		int i;
		
/*create variable to count neighbors*/
		int neighbors;
		
/*Create variable to store the summation of XOR between cur_gen and next_gen
If the summation of every cell of XOR between cur_gen and next_gen = 0
the game is ended*/
		int noChange=0;
		
		int NUMOFROW = PROBLEMSIZE/thread_count;
		
		start = (int)((thread_id*(NUMOFROW))+1);
	
		
		stop = (int)(start + ((NUMOFROW) -1)) ;
		
		for (x=start ; x< stop; x++)

			{
				for(y=1;y<PROBLEMSIZE+1;y++)
				{
						/*Reset neighbors = 0*/	
						neighbors=0;
						
						/*Count every neighbors for every cell*/
						neighbors = (cur_gen[x-1][y-1])+(cur_gen[x][y-1])
						+(cur_gen[x+1][y-1])+(cur_gen[x-1][y])
						+(cur_gen[x+1][y])+(cur_gen[x-1][y+1])
						+(cur_gen[x][y+1])+(cur_gen[x+1][y+1]);
																
					
							
							/*Checking condition in each cell*/
							
							if(cur_gen[x][y]==1){
							
								if (neighbors<=1 || neighbors >=4){
									next_gen[x][y] =0;
								}
								
								//else if(neighbors==2 || neighbors ==3){
								else{
									next_gen[x][y] =1;
								}							
							}
							
							else{
								if(neighbors==3){
									next_gen[x][y] =1;
								}
								else{
									next_gen[x][y] =0;
								}
							
							}
							
							
							/*Create Flag to check 2d arrays that , Is it the same or not
							If 2 arrays are the same, the result should be 0*/
							noChange+= cur_gen[x][y]^next_gen[x][y];
						
							
				}
			}
			
	
						


/**************************************Barrier****************************************/


			
		pthread_mutex_lock(&barrier_mutex);
		
/*===================Critical Section========================*/
      	barrier_thread_count++;
			
		
// If barrier count = thread count, then it is ready to go to the next iteration.
		 if (barrier_thread_count == thread_count) 
		 	 {

		        barrier_thread_count = 0;
        
				pthread_cond_broadcast(&ok_to_proceed);
         
       		  	  if(noChange==0){ /*If the summation equals to 0, both generation are the same*/
				
						cout << "\nThe game is end : the previous iteration is the same as the next iteration"<<endl;
						End = true;
						}
		
          		/* 	Increase number of iteration; when every thread meet the barrier*/
				iter_num++;
      		} 
      
      else {
      
  
         // Wait unlocks mutex and puts thread to sleep.
         //    Put wait in while loop in case some other
         // event awakens thread.
         
         while (pthread_cond_wait(&ok_to_proceed,&barrier_mutex) != 0);
         // Mutex is relocked at this point.

      }
      
 /*===================END-Critical Section========================*/     

      	pthread_mutex_unlock(&barrier_mutex); // UNLOCK MUTEX


/*******************************************************************************/
		
/*Create swap variable */
				int **temp;
				temp = next_gen;
				next_gen = cur_gen ;
				cur_gen =temp;	

							
		}// end else
	

	}
	  
	
return NULL;
} // end void function 