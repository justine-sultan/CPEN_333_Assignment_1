//Justine Sultan
//Student Number: 35880146

//Active class for pumps of the gas station

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
	int _y_cursor; 
	Tank *_tank;
	
	int main(void);
	int updateDOS(void *ThreadArgs);			//Update DOS window
	bool pumpFuel(int tank, int amount);		//Pumps amount from tank, returns 1 if success, 0 if ran out of fuel in tank
	void getTimeStamp(char *timeString);		//Return time and copy timeStamp in military time

public:
	Pump(int myNumber, char* pumpDP, char* pumpPile);
};

#endif 

