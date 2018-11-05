//Justine Sultan
//Student Number 35880146

//Proccess for randomly generating and running customer/car active objects.

#include	<stdio.h>
#include	"..\rt.h"			// change the pathlist to rt.h to suit location on your disk
#include	"..\GSDataStructures.h"
#include	"Customer.h"
#include	<vector>

CRendezvous     r1("MyRendezvous", 3);	//GSC, 4 pumps, and customer
CRendezvous		r2("EndRendezvous", 3); //to synchronize termination of 3 processes



int	main()
{
	printf("Create mutex protecting customer pipeline \n");
	CMutex  m1("customerPipeline");	//create here so that it is set to 1 before customers created
	r1.Wait();	//wait for other processes/threads 

	printf("\nCreating customer objects\n");
	vector<Customer*> myC;

	Customer* customerP; 
	char pump1pipe[] = "Pump1Pipeline";
	while(1){
		Sleep((rand()%5000 + 500));		//pause for 500 to 5500 msec
		customerP = new Customer(1, pump1pipe); 
		customerP->Resume();
		myC.push_back(customerP);
		//TODO: add method to delete old threads from vector
		//...use events, make vector global and a have a "flush" function?
		printf("test \n"); 
	}

	//note: currently the customer threads run forever

	r2.Wait();

	return 0;

}	