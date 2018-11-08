//Justine Sultan
//Student Number: 35880146

//Active object class for customers of gas station

#include <stdio.h>
#include "Customer.h"


Customer::Customer(int myNumber, int pumpNumber)
{
	_myNumber = myNumber; 
	char randChar = rand() % 24 + 97;
	int randIndex = rand() % 9; 
	char myName[10] = "John_Blah";
	myName[randIndex] = randChar; 
	strcpy_s(_info.name, 18, myName);
	_info.creditCard= rand()%100000000 + 1 + 100000000;
	_info.fuelType = rand()%4+1;	
	_info.fuelAmount = rand() % 70 + 1;
	//FOR DEBUG:To save waiting time for 
	//_info.fuelAmount = rand()%10 + 1; //random between 1 and 10 liters
	_info.pump = pumpNumber;	

	if (_cust_DEBUG) { printf("Creating customer \n"); }
}


int Customer::main(void)
{
	CMutex  m1("customerPipeline" + std::to_string(_info.pump));			//Mutex to queue at pump
	m1.Wait();																
	CCondition lowTank("lowTank");											//Condition to check if pumps closed for fuel refill
	lowTank.Wait();															

	CEvent donePump("Event_donePump" + std::to_string(_info.pump));			//GSC/customer event   (Pump done pumping gas)
	CEvent moveHose("Event_moveHose" + std::to_string(_info.pump));			//Pump/custmomer event (Customer removed/returned hose)

	std::string pumpPipe("Pump");
	pumpPipe.append(std::to_string(_info.pump));
	pumpPipe.append("Pipeline");

	if (_cust_DEBUG) { printf("\nCustomer %d linking pump/customer pipeline for pump %s....\n", _myNumber, pumpPipe.c_str()); }
	CTypedPipe<struct customerInfo> pipeline(pumpPipe.c_str(), 1024);

	pipeline.Write(&_info);	
	Sleep(3000);
	moveHose.Signal();														

	donePump.Wait();										

	if (_cust_DEBUG) { printf("Customer %d returning hose and is driving out of gas station...\n", _myNumber); }
	Sleep(2000);
	moveHose.Signal();

	Sleep(1000);
	m1.Signal(); 
	if (_cust_DEBUG) { printf("Customer %d has left gas station \n", _myNumber); } 
	
	return(0); 
}