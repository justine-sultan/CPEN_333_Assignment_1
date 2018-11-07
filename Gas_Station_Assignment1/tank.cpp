//Justine Sultan
//Student Number 35880146

#include <stdio.h>
#include "rt.h"
#include "tank.h"

Tank::Tank(void) {
	//printf("Entering default Tank constructor...\n");

	_mutex = new CMutex("tankMutex");
	_dp = new CDataPool("tankDP", sizeof(struct tankData));
	_data = (struct tankData*)(_dp->LinkDataPool());

	for (int i = 0; i < 4; i++) {
		//_data->tankArray[i] = 500; 
		//FOR DEBUGGING/DEMO:
		_data->tankArray[i] = 205;
		_data->_price[i] = 1.00;
	}

	//DONT INTITIALIZE SHARED DATA  - (e.g. for datapool) 
	//or else datatpool data could be overwritten whenever instantiate new object

}

Tank::~Tank() {
	printf("Calling default monitor destructor \n");
}

bool Tank::pumpLiters(double liters, int tank) {
	_mutex->Wait();			// gain access to resource
	
	if (liters <= _data->tankArray[tank]) 
	{
		double num = _data->tankArray[tank];
		num = num - liters;
		_data->tankArray[tank] = num;	// update resource
		_mutex->Signal();		// release resource
		return true; 
	}
	else 
	{
		_data->tankArray[tank] = 0;	// update resource
		_mutex->Signal();		// release resource
		return false;
	}

}

bool Tank::decrement(int tank) {
	_mutex->Wait();			// gain access to resource

	if (0.5 <= _data->tankArray[tank])
	{
		double num = _data->tankArray[tank];
		num = num - 0.5;
		_data->tankArray[tank] = num;	// update resource
		_mutex->Signal();		// release resource
		return true;
	}
	else
	{
		_data->tankArray[tank] = 0;	// update resource
		_mutex->Signal();		// release resource
		return false;
	}

}

void Tank::setAmount(double amount, int tank) {
	_mutex->Wait();			// gain access to resource
	_data->tankArray[tank] = amount;	// update resource
	_mutex->Signal();		// release resource
	return;
}

double Tank::readAmount(int tank) {
	_mutex->Wait();			// gain access to resource
	double num = _data->tankArray[tank];	// update resource
	_mutex->Signal();		// release resource
	return num;
}

void Tank::setPrice(float price, int tank) {
	_mutex->Wait(); 
	_data->_price[tank] = price; 
	_mutex->Signal();
	return; 
}

float Tank::getPrice(int tank) {
	_mutex->Wait();
	float price = _data->_price[tank]; 
	_mutex->Signal();

	return price; 
}