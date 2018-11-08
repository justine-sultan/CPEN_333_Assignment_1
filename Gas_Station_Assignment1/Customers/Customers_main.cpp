//Justine Sultan
//Student Number 35880146

//Proccess for randomly generating and running customer/car active objects.

#include	<stdio.h>
#include	"..\rt.h"			// change the pathlist to rt.h to suit location on your disk
#include	"..\GSDataStructures.h"
#include	"Customer.h"
#include	<vector>

CRendezvous     r1("MyRendezvous", 6);	//GSC, 4 pumps, and customer
CRendezvous		r2("EndRendezvous", 3); //to synchronize termination of 3 processes
bool cust_main_DEBUG = false; 

int	main()
{
	CMutex  m1("customerPipeline1");	//create here so that it is set to 1 before customers created
	CMutex  m2("customerPipeline2");
	CMutex  m3("customerPipeline3");
	CMutex  m4("customerPipeline4");
	r1.Wait();	//wait for other processes/threads 

	if (cust_main_DEBUG) { printf("\nCreating customer objects\n"); }
	vector<Customer*> myC;

	Customer* customerP; 
	//char pump1pipe[] = "Pump1Pipeline";
	int i = 0; 
	int pumpNum = 0; 
	while(1){
		Sleep((rand()%10000 + 5000));		//random pause 
		pumpNum = rand() % 4 + 1;			//random number from 1 to 4
		customerP = new Customer(i, pumpNum); 
		customerP->Resume();
		myC.push_back(customerP);
		//TODO: add method to delete old threads from vector
		//...use events, make vector global and a have a "flush" function?
		i++; 
	}

	//note: currently the customer threads run forever

	r2.Wait();

	return 0;

}	