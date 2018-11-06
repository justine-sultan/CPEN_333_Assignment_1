//Justine Sultan
//Student Number: 35880146

#ifndef   __Customer__
#define   __Customer__

#include <stdio.h>
#include "..\rt.h"
#include "..\GSDataStructures.h"

class Customer : public ActiveClass{

private:
	int _myNumber;
	//int _pumpNumber; //memory leak? since only the pointer is ever getting passed, i dont think so?
	struct customerInfo _info; 
	
	int main(void);

public:
	Customer(int myNumber, int pumpNumber);

};

#endif 
