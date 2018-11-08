//Justine Sultan
//Student Number 35880146

//Proccess for randomly generating and running customer/car active objects

#include	<stdio.h>
#include	"..\rt.h"			
#include	"..\GSDataStructures.h"
#include	"Customer.h"
#include	<vector>

CRendezvous     r1("MyRendezvous", 6);		//Syncrhonization for GSC, 4 pumps, and customer
CRendezvous		r2("EndRendezvous", 3);		//Synchronization for termination of GSC, Pump, and Customer proccesses
bool cust_main_DEBUG = false; 

int	main()
{
	CMutex  m1("customerPipeline1");	
	CMutex  m2("customerPipeline2");
	CMutex  m3("customerPipeline3");
	CMutex  m4("customerPipeline4");
	r1.Wait();								

	if (cust_main_DEBUG) { printf("\nCreating customer objects\n"); }
	vector<Customer*> myC;

	Customer* customerP; 
	int i = 0; 
	int pumpNum = 0; 
	while(1){
		Sleep((rand()%10000 + 5000));		
		pumpNum = rand() % 4 + 1;			
		customerP = new Customer(i, pumpNum); 
		customerP->Resume();
		myC.push_back(customerP);
		i++; 
	}

	r2.Wait();

	return 0;

}	