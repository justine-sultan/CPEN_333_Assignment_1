//Justine Sultan
//Student Number: 35880146

#include <stdio.h>
#include "Pump.h"
#include    "..\GSDataStructures.h"
#include <chrono> 

bool PUMP_DEBUG = false;

Pump::Pump(int myNumber, char* pumpDP, char* pumpPipe)
{
	_myNumber = myNumber;
	_pumpDP = pumpDP;
	_pumpPipe = pumpPipe;

	_tank = new Tank();

	//get y cursor
	switch (_myNumber) {
	case 1:
		_y_cursor = 9;
		break;
	case 2:
		_y_cursor = 18;
		break;
	case 3:
		_y_cursor = 27;
		break;
	case 4:
		_y_cursor = 36;
		break;
	}
}

int Pump::updateDOS(void *ThreadArgs)	//thread for reading/writing to other processes
{
	CMutex  mDOS("pump_DOS_window");
	mDOS.Wait();
	MOVE_CURSOR(0, 4);
	printf("check\n");
	fflush(stdout);
	mDOS.Signal();

	while (1) {
		mDOS.Wait();
		MOVE_CURSOR(0, 4);
		printf("Tank 1: Regular-87   | %.2f              \n", _tank->getPrice(0));
		printf("Tank 2: MidGrade-89  | %.2f              \n", _tank->getPrice(1));
		printf("Tank 3: Premium-91   | %.2f              \n", _tank->getPrice(2));
		printf("Tank 4: Super-99     | %.2f              \n\n", _tank->getPrice(3));
		fflush(stdout);
		mDOS.Signal();
	}
	return true;
}

bool Pump::pumpFuel(int tank, int amount)
{
	CMutex  mDOS("pump_DOS_window");
	bool result = false;
	float price = _tank->getPrice(tank);
	CCondition lowTank("lowTank");

	for (int i = 0; i < amount * 2; i++) {
		Sleep(1000);
		result = _tank->decrement(tank); 

		mDOS.Wait(); 
		MOVE_CURSOR(15, _y_cursor + 6);
		printf("Your total bill: $%.2f \n", price*(i+1)*0.5);
		fflush(stdout);
		mDOS.Signal();

		if (result == false) {
			return result;
		}

		if (_tank->readAmount(tank) <= 200) {
			lowTank.Reset(); 
		}

	}

	return result; 

}

void Pump::getTimeStamp(char* timeString) {
	//getTimeStamp assumes that gas station starts at 00:00:00 hours/min/secs (military time)
	//getTimeStamp adds elapsed seconds on to above start time. at 23:59:59 the time rolls back to 00:00
	//clock_t timeSinceStart = clock();
	long int timeSinceStart = std::chrono::duration_cast< std::chrono::seconds >(std::chrono::system_clock::now().time_since_epoch()).count();
	long int hours = timeSinceStart / 3600;
	long int minutes = (timeSinceStart - (hours * 3600)) / 60;
	long int seconds = timeSinceStart - (hours * 3600) - (minutes * 60);
	hours = (hours % 24) - 8;

	//clock() is not thread safe!!
	//TODO: need alternative
	string timeStamp("11-08-18 "); 

	char hours_buffer[15];
	snprintf(hours_buffer, 10, "%02d", (int)hours);
	timeStamp.append(hours_buffer);
	timeStamp.append(":");
	char minutes_buffer[15];
	snprintf(minutes_buffer, 10, "%02d", (int)minutes);
	timeStamp.append(minutes_buffer);
	timeStamp.append(":");
	char seconds_buffer[15];
	snprintf(seconds_buffer, 10, "%02d", (int)seconds);
	timeStamp.append(seconds_buffer);
	/*timeStamp.append(hours);
	timeStamp.append(":");
	timeStamp.append(std::to_string(minutes));
	timeStamp.append(":");
	timeStamp.append(std::to_string(seconds));*/
	strcpy_s(timeString, 100, timeStamp.c_str());
	//strcpy_s(timeString, 100, "01-01-18 06:00:00"); 
	return; 
}

int Pump::main(void)
{
	CRendezvous  r1("MyRendezvous", 6);	//GSC, 4 pumps, and customer
	CMutex  mDOS("pump_DOS_window");

	//pump to GSC comm set-up (pump producer, GSC consumer)
	//note that the GSC also uses the donePump event under the customer to pump comm set-up section
	CSemaphore		ps("producerPump" + std::to_string(_myNumber), 0, 1);    // semaphore with initial value 0 and max value 1
	CSemaphore		cs("consumerPump" + std::to_string(_myNumber), 1, 1);    // semaphore with initial value 1 and max value 1
	if (PUMP_DEBUG) { printf("Pump %d linking to pump/GCS datapool, name: %s....\n", _myNumber, _pumpDP); }
	CDataPool datapool_Pump(_pumpDP, sizeof(struct pumpDatapool));
	struct pumpDatapool *dpPump = (struct pumpDatapool *)(datapool_Pump.LinkDataPool());
	if (PUMP_DEBUG) { printf("Pump linked to datapool: %s at address %p.....\n", _pumpDP, dpPump); }

	//customer to pump comm set-up
	CEvent donePump("Event_donePump" + std::to_string(_myNumber));	//event to let GSC/customer know that pump has finished pumping gas. note that the GSC also uses this
	CEvent moveHose("Event_moveHose" + std::to_string(_myNumber));	//event to let pump know hose is removed/returned
	if (PUMP_DEBUG) { printf("Pump %d creating pump/customer pipeline, name: %s....\n", _myNumber, _pumpPipe); }
	CTypedPipe<struct customerInfo>	pipeline(_pumpPipe, 1024);
	struct customerInfo custInfo; 

	//GSC to pump comm set-up
	CEvent startPump("Event_startPump" + std::to_string(_myNumber));	//event to signal pump to start pumping gas for customer

	ClassThread<Pump> dosThread(this, &Pump::updateDOS, SUSPENDED, NULL);
	if (_myNumber == 1) {
		//pump object 1 updates pump DOS window with price (don't need 4 versions of this thread to do the same thing)
		dosThread.Resume();
	}

	CCondition lowTank("lowTank");		//check if tanks low before starting
	for (int i = 0; i++; i < 4) {
		if (_tank->readAmount(i) <= 200) {
			lowTank.Reset();
		}
		else {
			lowTank.Signal();
		}
	}
	
	r1.Wait();	//wait for other processes before starting 

	//Testing area for lab 3/4--------

	//Main routine for a customer cycle. Should be one customer transaction/cycle per loop
	while (1) {
		pipeline.Read(&custInfo);	
		//TODO add data struct to recieve data
		if (PUMP_DEBUG) { printf("\nPump %d recieved info from customer \n", _myNumber); }

		//display customer info to Pump Display, then send it
		mDOS.Wait();
		MOVE_CURSOR(0, _y_cursor + 2);
		printf("Customer at pump. Selecting Fuel Grade and removing pump... \n");
		MOVE_CURSOR(15, _y_cursor + 3);
		printf("Name: %s \n", custInfo.name);
		MOVE_CURSOR(15, _y_cursor + 4);
		printf("Credit Card #: %d \n", custInfo.creditCard);
		MOVE_CURSOR(15, _y_cursor + 5);
		//printf("Fuel Type and Amount: %d, %d \n", custInfo.fuelType, custInfo.fuelAmount);
		//MOVE_CURSOR(15, _y_cursor + 6);
		printf("Your total bill: 0.00 \n");
		fflush(stdout);
		mDOS.Signal();

		moveHose.Wait();		//wait for customer to remove house/select fuel

		mDOS.Wait();
		MOVE_CURSOR(0, _y_cursor + 2);
		printf("Customer waiting for authorization...                      \n");
		MOVE_CURSOR(15, _y_cursor + 5);
		printf("Fuel Type and Amount: %d, %d \n", custInfo.fuelType, custInfo.fuelAmount);
		fflush(stdout);
		mDOS.Signal();

		//send customer info to GSC
		cs.Wait();
		strcpy_s(dpPump->name, 20, custInfo.name); //names must be 19 characters or less (this accounts for null character)
		dpPump->creditCard = custInfo.creditCard;
		dpPump->fuelType = custInfo.fuelType;	//type 1-4, need to convert to 0 index
		dpPump->fuelAmount = custInfo.fuelAmount;
		getTimeStamp(dpPump->timeStamp);
		ps.Signal();

		startPump.Wait();	//wait for GSC authorization to pump gas
		
		mDOS.Wait();
		MOVE_CURSOR(0, _y_cursor + 1); 
		TEXT_COLOUR(10, 0);
		printf("Pump enabled                                       \n");
		TEXT_COLOUR(7, 0);
		MOVE_CURSOR(0, _y_cursor + 2);
		printf("Transaction Authorizatized. Pumping fuel....       \n");
		fflush(stdout);
		mDOS.Signal();

		pumpFuel((custInfo.fuelType - 1), custInfo.fuelAmount);
		//TODO: for now, i don't think that the pumpFuel needs to be concurrent with this thread? may need to change if problems arise
		
		mDOS.Wait();
		MOVE_CURSOR(0, _y_cursor + 1); 
		TEXT_COLOUR(14, 0);
		printf("Pump disabled                                      \n");
		TEXT_COLOUR(7, 0);
		MOVE_CURSOR(0, _y_cursor + 2);
		printf("Customer returning hose and driving away...        \n");
		MOVE_CURSOR(15, _y_cursor + 3);
		printf("--                                                 \n");
		MOVE_CURSOR(15, _y_cursor + 4);
		printf("--                                                 \n");
		MOVE_CURSOR(15, _y_cursor + 5);
		printf("--                                                 \n");
		MOVE_CURSOR(15, _y_cursor + 6);
		printf("--                                                 \n");
		fflush(stdout);
		mDOS.Signal();

		donePump.Signal();	//signal that pump done pumping gas
		if (PUMP_DEBUG) { printf("Pump %d is done pumping \n", _myNumber); }

		moveHose.Wait();
		donePump.Signal();
		mDOS.Wait();
		MOVE_CURSOR(0, _y_cursor + 2);
		printf("No customer at pump.						       \n");		
		fflush(stdout);
		mDOS.Signal();
		
	

	}
	//end test area-----------

	//TODO: need to add rendesvous here??

	return(0);
}