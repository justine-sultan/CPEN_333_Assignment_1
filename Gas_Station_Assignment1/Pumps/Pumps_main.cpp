//Justine Sultan
//Student number 35880146

//Proccess for running 4 Pump active objects

#include 	<stdio.h>
#include 	"..\rt.h"		
#include    "..\GSDataStructures.h"
#include	"Pump.h"

//CRendezvous   r1("MyRendezvous", 6) dont need this here - it is inside pump.cpp mains
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

	char pump2dp[] = "PumpDP2";
	char pump2pipe[] = "Pump2Pipeline";
	Pump pump2(2, pump2dp, pump2pipe);
	pump2.Resume();

	char pump3dp[] = "PumpDP3";
	char pump3pipe[] = "Pump3Pipeline";
	Pump pump3(3, pump3dp, pump3pipe);
	pump3.Resume();

	char pump4dp[] = "PumpDP4";
	char pump4pipe[] = "Pump4Pipeline";
	Pump pump4(4, pump4dp, pump4pipe);
	pump4.Resume();

	//test area for lab3/4 ------------------
	
	//end test area ---------------
	pump1.WaitForThread(); 

	r2.Wait(); 

	return 0;
}