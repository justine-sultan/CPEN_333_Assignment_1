//Justine Sultan
//Student Number: 35880146

#ifndef   __Pump__
#define   __Pump__

#include <stdio.h>
#include "..\rt.h"
#include "..\tank.h"

class Pump : public ActiveClass {

private:
	int _myNumber; 
	char* _pumpDP; 
	char* _pumpPipe; 

	Tank *_tank;

	int main(void);
	int pipeCommThread(void *ThreadArgs); 
	bool pumpFuel(int tank, int amount);	//pumps amount from tank, returns 1 if success, 0 if ran out of fuel in tank
	void getTimeStamp(char *timeString); 

public:
	Pump(int myNumber, char* pumpDP, char* pumpPile);
	
};

#endif 

