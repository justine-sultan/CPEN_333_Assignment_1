//Justine Sultan
//Student Number: 35880146

#include <stdio.h>
#include "Customer.h"


Customer::Customer(int myNumber, char* pumpPipe)
{
	_myNumber = myNumber; 
	_pumpPipe = pumpPipe;

	char myName[] = "John_Ab";
	strcpy_s(_info.name, 20, myName);
	//_info.name= "John_Ab";	//names must be 19 characters or less (this accounts for null character)
	_info.creditCard= 123123123;
	_info.fuelType = 0;	//type 0-4
	_info.fuelAmount = 10;
	_info.pump = 1; 
	printf("Creating customer \n"); 
}


int Customer::main(void)
{
	CEvent donePump("Event_donePump");	//event to let GSC/customer know that pump has finished pumping gas
	CMutex  m1("customerPipeline");
	m1.Wait();	//wait at pump 

	printf("\nCustomer %d linking pump/customer pipeline, name: %s....\n", _myNumber, _pumpPipe);
	CTypedPipe<struct customerInfo> pipeline(_pumpPipe, 1024);

	printf("(Testing only) hit enter to start \n");
	getchar();
	pipeline.Write(&_info);	//send struct with customer info..."swiping" credit card
	donePump.Wait(); 
	printf("Customer %d is driving out of gas station...\n", _myNumber);
	Sleep(2000);

	m1.Signal(); 
	printf("Customer %d has left gas station \n", _myNumber); 
	
	return(0); 
}