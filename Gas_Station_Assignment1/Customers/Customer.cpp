//Justine Sultan
//Student Number: 35880146

#include <stdio.h>
#include "Customer.h"


Customer::Customer(int myNumber, int pumpNumber)
{
	//TODO: implement better random numbers
	_myNumber = myNumber; 
	char randChar = rand() % 24 + 97;
	int randIndex = rand() % 9; //random between 0 and 8
	char myName[10] = "John_Blah";
	myName[randIndex] = randChar; 
	strcpy_s(_info.name, 18, myName);
	_info.creditCard= rand()%100000000 + 1 + 100000000;
	_info.fuelType = rand()%4+1;	//random of type 1-4 - need to convert to 0 index
	//_info.fuelAmount = rand() % 70 + 1
	//FOR DEBUG
	_info.fuelAmount = rand()%10 + 1; //random between 1 and 10 liters

	_info.pump = pumpNumber;	
	//_info.pump = 1; 
	if (_cust_DEBUG) { printf("Creating customer \n"); }
}


int Customer::main(void)
{
	CMutex  m1("customerPipeline" + std::to_string(_info.pump));
	m1.Wait();			//wait at pump 
	CCondition lowTank("lowTank");
	lowTank.Wait();		//check if pumps closed for fuel refill

	CEvent donePump("Event_donePump" + std::to_string(_info.pump));	//event to let GSC/customer know that pump has finished pumping gas
	CEvent moveHose("Event_moveHose" + std::to_string(_info.pump));	//event to let pump know hose is removed/returned

	std::string pumpPipe("Pump");
	pumpPipe.append(std::to_string(_info.pump));
	pumpPipe.append("Pipeline");

	if (_cust_DEBUG) { printf("\nCustomer %d linking pump/customer pipeline for pump %s....\n", _myNumber, pumpPipe.c_str()); }
	CTypedPipe<struct customerInfo> pipeline(pumpPipe.c_str(), 1024);

	pipeline.Write(&_info);	//send struct with customer info..."swiping" credit card
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