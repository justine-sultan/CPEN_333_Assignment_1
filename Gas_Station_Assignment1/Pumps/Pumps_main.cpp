//Justine Sultan
//Student number 35880146

//Proccess for running 4 Pump active objects

#include 	<stdio.h>
#include 	"..\rt.h"		
#include    "..\GSDataStructures.h"
#include	"Pump.h"

CRendezvous		r2("EndRendezvous", 3); //to synchronize termination of 3 processes

struct 	    mydatapooldata {
	int studentNumber;
	int age;
	int courses[3];
};

int   main()
{
	printf("Creating pump active objects\n");
	char pump1dp[] = "PumpDP1";
	char pump1pipe[] = "Pump1Pipeline"; 
	Pump pump1(1, pump1dp, pump1pipe); 
	pump1.Resume(); 

	//test area for lab3/4 ------------------
	
	//end test area ---------------
	pump1.WaitForThread(); 

	r2.Wait(); 

	return 0;
}