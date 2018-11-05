//Justine Sultan
//Student Number

//This header file contains data structures shared across the gas station architecture

#ifndef   __GSDataStructures__
#define   __GSDataStructures__

#include <stdio.h>

struct pumpDatapool{
	char name[20];	//names must be 19 characters or less (this accounts for null character)
	int creditCard;
	int fuelType;	//type 0-4
	int fuelAmount; 
	char timeStamp[100]; 
};

struct customerInfo {
	char name[20];	//names must be 19 characters or less (this accounts for null character)
	int creditCard;
	int fuelType;
	int fuelAmount;
	int pump;	//chose which pump they go to: 1,2,3 or 4
};

struct transaction {
	char name[20];	//names must be 19 characters or less (this accounts for null character)
	int creditCard;
	int fuelType;
	int fuelAmount;
	int pump;	//chose which pump they go to: 1,2,3 or 4
	char timeStamp[100];
};


#endif 
