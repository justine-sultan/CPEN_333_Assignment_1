//Justine Sultan
//Student Number

//This header file contains data structures shared across the gas station architecture

#ifndef   __GSDataStructures__
#define   __GSDataStructures__

#include <stdio.h>

struct pumpDatapool{
	char name[20];	
	int creditCard;
	int fuelType;		//Type selection: 0-4
	int fuelAmount; 
	char timeStamp[100]; 
};

struct customerInfo {
	char name[20];	
	int creditCard;
	int fuelType;		//Type selection: 0-4
	int fuelAmount;
	int pump;			//Pump selection: 1-4
};

struct transaction {
	char name[20];	
	int creditCard;
	int fuelType;		//Type selection: 0-4
	int fuelAmount;
	int pump;			//Pump selection: 1-4
	char timeStamp[100];
};


#endif 
