//Justine Sultan
//Student Number 35880146

#ifndef   __Tank__
#define   __Tank__

#include <stdio.h>
#include "rt.h"

class Tank {
private:
	//mutex will protect entire datapool 
	struct tankData{	

		float _price[4]; //array of prices
		double tankArray[4]; 
		//have tanks 0 through 3
		
	};

	CDataPool *_dp;
	CMutex *_mutex;
	tankData *_data;
	
public:
	Tank(void);
	~Tank(void);
	bool pumpLiters(double liters, int tank); //decrements liters from tank. return 1 if successful, return 0 if fuel tank ran empty
	bool decrement(int tank);		//decrement 0.5 liters from tank. return 1 if succesful, return 0 if fuel tank ran empty 
	void setAmount(double amount,int tank);		//set tank to amount
	double readAmount(int tank);		//reads literAmount in tank
	void setPrice(float price, int tank);			//set price per liter for specified tank
	float getPrice(int tank);		//returns price of specified tank
};

#endif 
