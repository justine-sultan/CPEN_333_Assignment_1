//Justine Sultan
//Student Number: 35880146

//Active object class for customers of gas station

#ifndef   __Customer__
#define   __Customer__

#include <stdio.h>
#include "..\rt.h"
#include "..\GSDataStructures.h"

class Customer : public ActiveClass{

private:
	int _myNumber;
	struct customerInfo _info; 
	bool _cust_DEBUG = false; 
	
	int main(void);

public:
	Customer(int myNumber, int pumpNumber);

};

#endif 
