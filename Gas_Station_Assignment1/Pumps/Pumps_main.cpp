//Justine Sultan
//Student number 35880146

//Proccess for running 4 Pump active objects

#include 	<stdio.h>
#include 	"..\rt.h"		
#include    "..\GSDataStructures.h"
#include	"Pump.h"

CRendezvous		r2("EndRendezvous", 3);		//Event to synchronize termination of 3 processes (GSC, pump, and customer)

struct 	    mydatapooldata {
	int studentNumber;
	int age;
	int courses[3];
};

int   main()
{
	CMutex  mDOS("pump_DOS_window");

	//Set up DOS window:
	int init = -1;	
	mDOS.Wait();
	MOVE_CURSOR(0, 0);
	printf("CUSTOMER PUMP DISPLAY \n\n");

	printf("FUEL STATUS: \n");
	printf("Tank and Fuel Grade  | Cost per Liter \n");
	printf("Tank 1: Regular-87   |                \n");
	MOVE_CURSOR(23, 4);
	printf("%d\n", init);
	printf("Tank 2: MidGrade-89  |                \n");
	printf("Tank 3: Premium-91   |                \n");
	printf("Tank 4: Super-99     |                \n\n");

	printf("PUMP 1 STATUS: \n");
	TEXT_COLOUR(14, 0);
	printf("Pump disabled \n");
	TEXT_COLOUR(7, 0);
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n"); //12
	MOVE_CURSOR(15, 13);
	printf("-- \n");
	MOVE_CURSOR(15, 14);
	printf("-- \n");
	MOVE_CURSOR(15, 15);
	printf("-- \n");
	MOVE_CURSOR(15, 16);
	printf("-- \n\n");

	printf("PUMP 2 STATUS: \n");
	TEXT_COLOUR(14, 0);
	printf("Pump disabled \n");
	TEXT_COLOUR(7, 0);
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 22);
	printf("-- \n");
	MOVE_CURSOR(15, 23);
	printf("-- \n");
	MOVE_CURSOR(15, 24);
	printf("-- \n");
	MOVE_CURSOR(15, 25);
	printf("-- \n\n");

	printf("PUMP 3 STATUS: \n");
	TEXT_COLOUR(14, 0);
	printf("Pump disabled \n");
	TEXT_COLOUR(7, 0);
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 31);
	printf("-- \n");
	MOVE_CURSOR(15, 32);
	printf("-- \n");
	MOVE_CURSOR(15, 33);
	printf("-- \n");
	MOVE_CURSOR(15, 34);
	printf("-- \n\n");

	printf("PUMP 4 STATUS: \n");
	TEXT_COLOUR(14, 0);
	printf("Pump disabled \n");
	TEXT_COLOUR(7, 0);
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 40);
	printf("-- \n");
	MOVE_CURSOR(15, 41);
	printf("-- \n");
	MOVE_CURSOR(15, 42);
	printf("-- \n");
	MOVE_CURSOR(15, 43);
	printf("-- \n\n");

	fflush(stdout);
	mDOS.Signal();

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

	pump1.WaitForThread(); 

	r2.Wait(); 

	return 0;
}