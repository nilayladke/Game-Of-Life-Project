/*
Compile command: g++ -fopenmp -O -o omp omp.cpp -Wall -g
To run program: ./omp <#problemsize> <#of iteration> <#of thread>
*/


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <cstdlib>

using namespace std;


double gettime (void){
	struct timeval tvalue;

	gettimeofday (&tvalue, NULL);


	return ((double) tvalue.tv_sec + (double)tvalue.tv_usec/1000000.0);
}


int **allocarray(int P, int Q) {
  int i;
  int *p, **a;
  
  p = (int *)malloc(P*Q*sizeof(int));
  a = (int **)malloc(P*sizeof(int*));

  if (p == NULL || a == NULL) 
    printf("Error allocating memory\n");

 /* for row major storage */
  for (i = 0; i < P; i++)
    a[i] = &p[i*Q];
  
  return a;
}


/*Global Variable*/
	/*declare initial iteration number*/
	int iter_num =1;
/*Read input from console*/
	int PROBLEMSIZE, MAX, thread_count;
	int nest_thread_count;
/*Declare pointer for current and next iteration*/
	int** cur_gen;
	int** next_gen;
	int** temp;
// Initiate End Boolean
	bool End=false;
// compare board variable
	int cond = 0;
/*End Global Variable*/

int main (int argc, char* argv[]){

//read parameter
//int thread_count = strtol (argv[1],NULL,10);
		
/*Store problem size from argv[1]*/
		PROBLEMSIZE = atoi(argv[1]);

/*Store MAX from argv[2]*/
		MAX = atoi(argv[2]);

/*Store NUMOFTHREAD from argv[3]*/
		thread_count = atol(argv[3]);
		
		if(argv[4]!=NULL){
		nest_thread_count = atol(argv[4]);
		}
		
		
		
/*row and column index*/
		int x,y;
	
/*reset time for random 0,1*/
		srand( time(NULL) );
	
/*starttime, endtime*/
		double starttime,endtime;


/**Initial 2 dimensional arrays***/

		cur_gen = allocarray(PROBLEMSIZE+2,PROBLEMSIZE+2);
		next_gen = allocarray(PROBLEMSIZE+2,PROBLEMSIZE+2);
		temp = allocarray(PROBLEMSIZE+2,PROBLEMSIZE+2);
		

/*************************Start Initial 2 dimensional array***************************/
/*Use 2 for loop to assigned 0 to border and random initial binary in the board*/
	for (x=0;x<PROBLEMSIZE+2;x++){
		for(y=0;y<PROBLEMSIZE+2;y++){
			
			
/*If the cell is not border, then random binary digit*/
			if(x>=1&&x<=PROBLEMSIZE&&y>=1&&y<=PROBLEMSIZE){
			
/*Store 1st iteration random binary to an array*/
				cur_gen[x][y] = rand()%2;
				
// next generation assign as 0
				next_gen[x][y] = 0;
			}
			
/*Border cell = 0*/
			else{
					cur_gen[x][y] = 0;
					next_gen[x][y] = 0;
				}
			}
		}

/*************************End Initial 2 dimensional array***************************/


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

				

/*declare initial iteration number*/	
	int iter_num =2;
/*create variable to count neighbors*/
	int neighbors=0;
		
/*Create variable to store the summation of XOR between cur_gen and next_gen
If the summation of every cell of XOR between cur_gen and next_gen = 0
the game is ended*/
	
	int Change =0;
	
		
	for(iter_num=2;iter_num<=MAX;iter_num++)
	{
		
			/*pragma*/		
			#pragma omp parallel for num_threads(thread_count)\
			reduction(+:Change)\
			default(none)\
			private(x,y,neighbors)\
			shared(iter_num,PROBLEMSIZE,cur_gen,next_gen,temp)	
			for (x=1;x<PROBLEMSIZE+1;x++)
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
							// #pragma omp critical
							Change+= cur_gen[x][y]^next_gen[x][y];
						
							
				}
			}
			
			
	       /**************************************Barrier****************************************/



	
			
			
		/*===================Critical Section========================*/
   

		
		if(Change==0){
				
				cout << "The game is end, the current generation is the same as"
				<<" the next generation" << endl;
				//End=true;
				iter_num=MAX+1;
				}		
			
	/***************************Start print next gen matrix************************************/
				
#ifdef p2			
				cout<<"######################\nTHE"<<iter_num <<" GENERATION"<<"\n######################\n";
					for (x=1;x<PROBLEMSIZE+1;x++)
						{
							for(y=1;y<PROBLEMSIZE+1;y++)
								{
									/*print the next generation*/
									cout<<next_gen[x][y]<<" ";
								}
							cout<<endl;
						}
						
#endif				
				
				
			
/**************************************End print next gen matrix***************************/
				
				/*Swap variable*/
				
				
				temp = next_gen;
				next_gen = cur_gen ;
				cur_gen =temp;	
								
			
			 #pragma omp barrier
			}	// end for iter_num loop				
		
		
		
		
		
/*----------------------------------------------------------------------------------*/
		
	/*Record endtime*/
	endtime= gettime();

	/*Print time*/
	cout << "Time taken (seconds): " << endtime-starttime << endl;

		
		
		return 0;
} // end main
		
		