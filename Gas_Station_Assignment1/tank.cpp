//Justine Sultan
//Student Number 35880146

//Monitor class for Fuel Tanks

#include <stdio.h>
#include "rt.h"
#include "tank.h"

Tank::Tank(void) {
	_mutex = new CMutex("tankMutex");
	_dp = new CDataPool("tankDP", sizeof(struct tankData));
	_data = (struct tankData*)(_dp->LinkDataPool());

	for (int i = 0; i < 4; i++) {
		//_data->tankArray[i] = 500; 
		//FOR DEBUGGING/DEMO:
		_data->tankArray[i] = 205;
		_data->_price[i] = 1.00;
	}

	//NOTE: Datapool data willl be overwritten when new object instantiated!
	//All tank objects MUST be created before process/thread rendevous at start.
}

Tank::~Tank() {
	printf("Calling default monitor destructor \n");
}

bool Tank::pumpLiters(double liters, int tank) {
	_mutex->Wait();		
	
	if (liters <= _data->tankArray[tank]) 
	{
		double num = _data->tankArray[tank];
		num = num - liters;
		_data->tankArray[tank] = num;	
		_mutex->Signal();		
		return true; 
	}
	else 
	{
		_data->tankArray[tank] = 0;	
		_mutex->Signal();		
		return false;
	}

}

bool Tank::decrement(int tank) {
	_mutex->Wait();			

	if (0.5 <= _data->tankArray[tank])
	{
		double num = _data->tankArray[tank];
		num = num - 0.5;
		_data->tankArray[tank] = num;	
		_mutex->Signal();		
		return true;
	}
	else
	{
		_data->tankArray[tank] = 0;	
		_mutex->Signal();		
		return false;
	}

}

void Tank::setAmount(double amount, int tank) {
	_mutex->Wait();			
	_data->tankArray[tank] = amount;	
	_mutex->Signal();		
	return;
}

double Tank::readAmount(int tank) {
	_mutex->Wait();			
	double num = _data->tankArray[tank];	
	_mutex->Signal();		
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