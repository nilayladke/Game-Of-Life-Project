/*
compile command : mpic++ -o mpi mpi.cpp -g -Wall -O
print debug board command: mpic++ -o mpi mpi.cpp -g -Wall -O -DDEBUG
Execute command: mpiexec -n <number of process> ./mpi <PROBLEMSIZE> <ITERATION> <number of process>
*/


#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <malloc.h>
using namespace std;

/*declare initial iteration number*/
int iter_num =1;
/*Read input from console*/
int PROBLEMSIZE, MAX;
/*Declare pointer for current and next iteration*/
int** cur_gen;
int** next_gen;

int REMAIN,N = 0;
int* start;
int* stop;
// Initiate End Boolean
bool End=false;

long thread_count;
int rank;
int rank_size;

// Initate buffers
int       *send_buf, *rec_buf;
MPI_Status   status, statuses[2];
MPI_Request  r[2];



double gettime (void){
	struct timeval tvalue;

	gettimeofday (&tvalue, NULL);


	return ((double) tvalue.tv_sec + (double)tvalue.tv_usec/1000000.0);
}

int main(int argc, char* argv[])
{

	PROBLEMSIZE = atoi(argv[1]);
	MAX = atoi(argv[2]);
  thread_count = atol(argv[3]);

	/*row and column index*/
	int x,y;
	/*reset time for random 0,1*/
	srand( time(NULL) );
	/*starttime, endtime*/
	double starttime,endtime;


	  /*INDEX*/
		/*Size of start and stop = total number of threads*/
		start = new int [thread_count];
		stop = new int [thread_count];

		/*REMAIN = remaining of PROBLEMSIZE/thread_cout-1 s*/
		REMAIN = PROBLEMSIZE%(thread_count-1);

		/*N = number of rows per thread_id; start from 1*/
		N = PROBLEMSIZE/(thread_count-1);


		if (REMAIN == 0) {
			for (int k=1; k < thread_count; k++){

				start[k] = ((k-1)*N)+1;

				stop[k] = (start[k] + N) -1 ;
			}
		}

		else{ // remain != 0
			start[1] = 1; /*thread_id 1 must starts with row# 1*/
			stop[1] = 1+N; /*thread_id 1 must stops at row# 1+N-1 +1= 1+N*/
			REMAIN--; /*REMAIN will be decrease after splits remaining to each thread_id*/

			for(int k=2;k<thread_count;k++){ /*Start from thread_id 2 to thread_count-1*/

					start[k]=stop[k-1]+1;
					stop[k] = start[k] + N -1;

					if(REMAIN >0){
							stop[k]++;
							REMAIN--;/*Reduce remain after split remaining to this thread_id*/

					}

			}

		}



	/*END INDEX*/

	/**********************Initial 2 dimensional arrays**********************************/

	/*First create the first column of 2D array*/
	cur_gen = new int*[PROBLEMSIZE+2];
	next_gen = new int*[PROBLEMSIZE+2];


	for (x=0;x<PROBLEMSIZE+2;x++)
	{
		/*Create 2nd dimension for the current generation and the next generation*/
		cur_gen[x] = new int[(PROBLEMSIZE+2)];
		next_gen[x] = new int[(PROBLEMSIZE+2)];
	}

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




/*Record starttime*/
	starttime = gettime();



/*Start MPI Part*/
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank );
  MPI_Comm_size(MPI_COMM_WORLD,&rank_size);


  sbuf = (int *) malloc( N+10 * sizeof(double) );
  rbuf = (int *) malloc( N+10 * sizeof(double) );

/**********************Print First Generation************************/

  	# ifdef DEBUG
    if(rank==0){
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
  }
  	# endif
/*********************************************************************/

/*game of life part*/

    int Change[thread_count];


/*create variable to count neighbors*/
  	int neighbors;

/*Start Loop with iteration number =2*/
  	int z=2;

    while(z<=MAX&&!End)/*This loop will end either number of iteration is exceed
  											or the current board is the same as the next iteration*/
  	{
/*Master Job*/
      if(rank==0){
      MPI_Isend( sbuf, N+10, MPI_INT, rank, z, MPI_COMM_WORLD,
           &r[0] );
      MPI_Irecv( rbuf, N+10, MPI_INT, rank, z, MPI_COMM_WORLD,
           &r[1] );
      MPI_Waitall( 2, r, statuses );
    }

/*Slave job*/
      if(rank!=0){

        MPI_Irecv( rbuf, N+10, MPI_INT, 0, z, MPI_COMM_WORLD,
            &r[0] );


        Change[rank] =0;

    		for(x=start[rank];x<=stop[rank];x++)

    		{


    			for(y=1;y<=PROBLEMSIZE;y++)
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

    				/*Create Flag to check 2d arrays that it is the same or not by using XOR.
    				If 2 current cell and the next iteration cell are the same,
    				the result should be 0
    				If the summation of result of XOR of every cell = 0,
    				it means that both iteration are exactly the same for the same thread_id*/

    					Change[thread_id]+= cur_gen[x][y]^next_gen[x][y];


    			}

    		}




    		if(Change[rank]==0){


    			cout<<"Start-Stop: "<<start[thread_id]<<"-"<< stop[thread_id]<<" Thread_id" <<thread_id
    			<<" iteration#: "<<z<<" Change = "<< Change[thread_id] <<endl;


    		}

    		cond |= Change[rank];



    			if(cond==0) {
    				//cout << "duplication" << endl;

    				cout << "The game is end, the current generation is the same as"
    				<<" the next generation" << endl;

    				End=true;

    			}



    			/*Create swap variable */
    			int **temp;
    			temp = next_gen;
    			next_gen = cur_gen ;
    			cur_gen =temp;

    			cond = 0;

          MPI_Isend( sbuf, N+10, MPI_INT, 0, z, MPI_COMM_WORLD,
              &r[1] );
         MPI_Waitall( comm_sz, r, statuses );

    		}




      }

      if(rank==0){// Master Job

        # ifdef DEBUG // if the game isn't end, print the next iteration.
              if(!End){

                cout<<"######################\nTHE"<<z<<" GENERATION"<<"\n######################\n";
                for (x=1;x<PROBLEMSIZE+1;x++)
                {
                  for(y=1;y<PROBLEMSIZE+1;y++)
                  {
                    /*print the next generation*/
                    cout<<next_gen[x][y]<<" ";
                  }
                  cout<<endl;
                }

              }
        # endif



        z++;

      }


    } // end while
  if (rank == 0) {
  /*Record endtime*/
	endtime= gettime();

	/*Print time*/
	cout << "Time taken (seconds): " << endtime-starttime << endl;
  }
  MPI_Finalize();


	/*Delete 2 dimensional array*/

	for (x = 0; x < PROBLEMSIZE+2;x++ ) {
		delete[] cur_gen[x];
		delete[] next_gen[x];

	}

	delete[] cur_gen;
	delete[] next_gen;
  return 0;
}
