#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>

using namespace std;

// boolean for stopping pthreads from master thread , when the matrix (airplane) is full !
bool STOP = false;
// value of turn , starts with 1
int TURN_VALUE = 1;
// occupied seat count
int seat = 0;
// Row and Column numbers
int row = 2;
int column = 50;
//vector<vector<int> > matrix(row,vector<int>(column));
// Matrix is created globally to be sure that all pthreads uses the shared memory!
vector<vector<int> > matrix(row, vector<int>(column));

// initializes the matrix , assigning every value in the matrix to 0 ,
// vector nested matrix is already declared as global value , this assigns values to 0.
void createMatrix()
{
	matrix.resize(row);
	for (int i = 0; i < row; i++)
	{
		matrix[i].resize(column, 0);
	}
}

// converts to seat number , takes selected row and column for the reservation
// and converts into a seat number
int find_seat_number(int randrow, int randcol)
{
	int seatnumber = 0;
	if (randrow == 0)
	{
		seatnumber = (randcol * 2) + 1;
		return seatnumber;
	}
	else
	{
		seatnumber = (randcol * 2) + 2;
		return seatnumber;
	}
}

// returns the string of travelagencyTAI according to their id
string find_travel_agent(int id)
{
	if (id == 1)
	{ // if id equals to 1 , this means it is TravelAgency1
		return "TravelAgency1";
	}
	else if (id == 2)
	{ // if id equals to 2 , this means it is TravelAgency2
		return "TravelAgency2";
	}
	else
	{ // if id not equals to 1 or 2 , this means it is TravelAgency3
		return "TravelAgency3";
	}
}

// This function changes the value of the turn ,
// so that pthreads can know who's turn is now .
void change_the_turn()
{
	// if the turn value equals to 1 , changes to 2
	if (TURN_VALUE == 1)
	{
		cout << "TravelAgency1 exits the shared memory area . . ." << endl
			 << endl;
		TURN_VALUE = 2;
		cout << "TURN CHANGED TO 2 . . . " << endl;
		cout << endl;
	}
	else if (TURN_VALUE == 2)
	{ // if the turn equals to 2 , changes to 3
		cout << "TravelAgency2 exits the shared memory area . . ." << endl
			 << endl;
		TURN_VALUE = 3;
		cout << "TURN CHANGED TO 3 . . . " << endl;
		cout << endl;
	}
	else
	{ // if the turn is not equal to 1 or 2 , this means turn was 3 , so changes to 1
		cout << "TravelAgency3 exits the shared memory area . . ." << endl
			 << endl;
		TURN_VALUE = 1;
		cout << "TURN CHANGED TO 1 . . . " << endl;
		cout << endl;
	}
}

int randomSeat_row()
{
	// i used srand in order to have random numbers in every execution of the code
	// if it is not needed , i commented rand() method for randomization.
	srand(time(NULL));
	int finalNum = rand() % (1 - 0 + 1) + 0; // range of randomization min = 0 , max = 1 inclusive
	return finalNum;

	//int randNum = rand()%(1-0 + 1) + 0;
	//return randNum;
}

int randomSeat_column()
{
	// i used srand in order to have random numbers in every execution of the code
	// if it is not needed , i commented rand() method for randomization.
	srand(time(NULL));
	int finalNum = rand() % (49 - 0 + 1) + 0; // range of randomization min = 0 , max = 49 inclusive
	return finalNum;

	//int randNum = rand()%(49-0 + 1) + 0;
	//return randNum;
}

// Checks whether the seat is empty or not
bool seat_is_empty(int randrow, int randcol)
{
	if (matrix[randrow][randcol] == 0)
	{				 // checks the random indexed row and column in matrix , whether the value is 0
		return true; // if empty returns true
	}
	else
	{
		return false; // if not empty returns false
	}
}

// Prints the matrix
void print_matrix()
{
	for (int i = 0; i < matrix.size(); i++)
	{
		for (int j = 0; j < matrix[0].size(); j++)
		{
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
}

// Function for the master thread in order to check whether matrix is full or not
// Master thread calls this function for Stop pthreads ,if there is no available seat
bool is_matrix_full()
{
	for (int i = 0; i < matrix.size(); i++)
	{
		for (int j = 0; j < matrix[0].size(); j++)
		{
			if (matrix[i][j] == 0)
			{				  // if the value in selected row and column equals to 0 ,
				return false; // this means that matrix is not full , so returns false
			}
		}
	}
	return true; // if no 0 value occur it means , all the seats are already reserved
}

// Reserves the selected seat for travelagency , it assigns the id of the travelagency to
// the selected row and column
void take_the_seat(int randrow, int randcol, int id)
{
	matrix[randrow][randcol] = id;
}

// This function is for reserving the seats in given order and requirements
// start_routine of the excutable threads
void *start_race(void *param)
{									  // takes the parameter as pthread syntax
	int *paramPointer = (int *)param; // creates a integer that points to the id of thread
	while (!STOP)
	{ // This is for Main thread , checks the global value STOP , it goes out of the loop , if reservation finished and matrix is full
		if (TURN_VALUE == *paramPointer)
		{															 // Checks which threads turn is it , checks with equality of unique id's of each thread
			string travel_agency = find_travel_agent(*paramPointer); // creates string , finds which travelAgency is it , using id
			cout << travel_agency << " enters the shared memory area . . ." << endl
				 << endl;
			int randrow = randomSeat_row();	   // creates a randomized row index
			int randcol = randomSeat_column(); // creates a randomized column index
			// This loop is for to find empty seat in matrix ,
			// if the selected indexes of row and column is empty and matrix is not full yet it goes out of the loop.
			//if the selected row and column is not empty , then it tries to find empty seat with randomization.
			while (seat_is_empty(randrow, randcol) == false && !STOP)
			{
				randrow = randomSeat_row();	   // changes row index to a new random row index , because it was not empty .
				randcol = randomSeat_column(); // changes row index to a new random column index , because it was not empty .
			}
			// generates the seat number
			// converts row and column calue to seat number as wanted .
			int seat_number = find_seat_number(randrow, randcol);
			// Trace that shows the reserver of the seat and which seat is reserved.
			cout << "Seat Number " << seat_number << " is reserved by " << travel_agency << endl
				 << endl;
			// reserves the empty seat .
			take_the_seat(randrow, randcol, *paramPointer);
			// occupied seat counter incrementation .
			seat += 1;
			// shows the remaining seats available in matrix .
			cout << 100 - seat << " seats left in airplane . . ." << endl
				 << endl;
			// changes the turn for the next thread , which is waiting busy.
			change_the_turn();
		}
	}
}

int main()
{

	// threads of travelAgencies uniquely identified
	pthread_t travel_agent_1;
	pthread_t travel_agent_2;
	pthread_t travel_agent_3;

	// id's for travelagencies
	int id1 = 1; // TravelAgency1
	int id2 = 2; // TravelAgency2
	int id3 = 3; // TravelAgency3

	// creates a new thread and makes it executable for each travel agency 1 to 3
	// pthread_create takes , thread handle(unique id) , NULL (default values), start_routine as start_race function , and unique id as arg.
	pthread_create(&travel_agent_1, NULL, start_race, (void *)&id1);
	cout << "created pthread1 . . ." << endl;
	pthread_create(&travel_agent_2, NULL, start_race, (void *)&id2);
	cout << "created pthread2 . . ." << endl;
	pthread_create(&travel_agent_3, NULL, start_race, (void *)&id3);
	cout << "created pthread3 . . ." << endl;

	// Loop for Main thread
	// When Main thread sees that the Matrix is full,
	// it will first finish the execution of the other 3 threads.
	while (!is_matrix_full())
		;

	// changes the globally setted variable STOP , from false to true to finish the execution .
	STOP = true;

	// subroutines that blocks the calling thread until the specified thread terminates.
	// joins all of them .
	pthread_join(travel_agent_1, NULL);
	cout << "joined pthread1 . . ." << endl;
	pthread_join(travel_agent_2, NULL);
	cout << "joined pthread2 . . ." << endl;
	pthread_join(travel_agent_3, NULL);
	cout << "joined pthread3 . . ." << endl;
	cout << endl
		 << endl;

	// prints the matrix after the reservation is finished.
	cout << "Airplane : " << endl
		 << endl;
	print_matrix();

	return 0;
}
