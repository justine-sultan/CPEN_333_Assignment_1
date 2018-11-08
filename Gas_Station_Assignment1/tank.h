//Justine Sultan
//Student Number 35880146

//Monitor class for Fuel Tanks

#ifndef   __Tank__
#define   __Tank__

#include <stdio.h>
#include "rt.h"

class Tank {
private:
	struct tankData{	
		float _price[4]; 
		double tankArray[4]; 
		
	};

	CDataPool *_dp;
	CMutex *_mutex;
	tankData *_data;
	
public:
	Tank(void);
	~Tank(void);
	bool pumpLiters(double liters, int tank);		//Decrements liters from tank. Return 1 if successful, return 0 if fuel tank ran empty
	bool decrement(int tank);						//Decrement 0.5 liters from tank. Return 1 if succesful, return 0 if fuel tank ran empty 
	void setAmount(double amount,int tank);			//Set tank to amount
	double readAmount(int tank);					//Reads amount in tank. Returns amount
	void setPrice(float price, int tank);			//Set price per liter for specified tank
	float getPrice(int tank);						//Returns price of specified tank
};

#endif 
