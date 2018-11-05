//Justine Sultan
//Student Number: 35880146

#include <stdio.h>
#include "Pump.h"
#include    "..\GSDataStructures.h"
#include <chrono> 

Pump::Pump(int myNumber, char* pumpDP, char* pumpPipe)
{
	_myNumber = myNumber;
	_pumpDP = pumpDP; 
	_pumpPipe = pumpPipe; 

	_tank = new Tank();
}

int Pump::pipeCommThread(void *ThreadArgs)	//thread for reading/writing to other processes
{
	while (1) {}
	return true; 
}

bool Pump::pumpFuel(int tank, int amount)
{
	printf("Pump %d pumping %d of fuel from tank %d....\n", _myNumber, amount, tank);
	bool result = false; 

	for (int i = 0; i < amount * 2; i++) {
		Sleep(1000);
		result = _tank->decrement(tank); 
		if (result == false) {
			return result; 
		}
	}

	return result; 

}

void Pump::getTimeStamp(char* timeString) {
	//getTimeStamp assumes that gas station starts at 00:00:00 hours/min/secs (military time)
	//getTimeStamp adds elapsed seconds on to above start time. at 23:59:59 the time rolls back to 00:00
	/*clock_t timeSinceStart = clock();
	int hours = timeSinceStart / 3600;
	int minutes = (timeSinceStart - (hours * 3600)) / 60;
	int seconds = timeSinceStart - (hours * 3600) - (minutes * 60); 
	hours = hours % 24;*/

	//clock() is not thread safe!!
	//TODO: need alternative

	strcpy_s(timeString, 100, "01-01-18 06:00:00"); 
	return; 
}

int Pump::main(void)
{
	CRendezvous  r1("MyRendezvous", 3);	//GSC, 4 pumps, and customer

	//pump to GSC comm set-up (pump producer, GSC consumer)
	//note that the GSC also uses the donePump event under the customer to pump comm set-up section
	CSemaphore		ps("producerPump" + std::to_string(_myNumber), 0, 1);    // semaphore with initial value 0 and max value 1
	CSemaphore		cs("consumerPump" + std::to_string(_myNumber), 1, 1);    // semaphore with initial value 1 and max value 1
	printf("Pump %d linking to pump/GCS datapool, name: %s....\n", _myNumber, _pumpDP);
	CDataPool datapool_Pump(_pumpDP, sizeof(struct pumpDatapool));
	struct pumpDatapool *dpPump = (struct pumpDatapool *)(datapool_Pump.LinkDataPool());
	printf("Pump linked to datapool: %s at address %p.....\n", _pumpDP, dpPump);

	//customer to pump comm set-up
	CEvent donePump("Event_donePump");	//event to let GSC/customer know that pump has finished pumping gas. note that the GSC also uses this
	printf("Pump %d creating pump/customer pipeline, name: %s....\n", _myNumber, _pumpPipe);
	CTypedPipe<struct customerInfo>	pipeline(_pumpPipe, 1024);
	struct customerInfo custInfo; 

	//GSC to pump comm set-up
	CEvent startPump("Event_startPump" + std::to_string(_myNumber));	//event to signal pump to start pumping gas for customer
	//TODO: do i still need this class thread for comm?
	//maybe eventually use it for displaying to DOS window
	ClassThread<Pump> pipeComm(this, &Pump::pipeCommThread, ACTIVE, NULL);
	
	r1.Wait();	//wait for other processes before starting 

	//Testing area for lab 3/4--------

	//Main routine for a customer cycle. Should be one customer transaction/cycle per loop
	while (1) {
		pipeline.Read(&custInfo);	
		//TODO add data struct to recieve data
		printf("\nPump %d recieved info from customer \n", _myNumber);

		cs.Wait();
		strcpy_s(dpPump->name, 20, custInfo.name); //names must be 19 characters or less (this accounts for null character)
		dpPump->creditCard = custInfo.creditCard;
		dpPump->fuelType = custInfo.fuelType;	//type 0-4
		dpPump->fuelAmount = custInfo.fuelAmount;
		getTimeStamp(dpPump->timeStamp);
		ps.Signal();

		startPump.Wait();	//wait for GSC authorization to pump gas
		/*printf("fuelAmount %d\n ", custInfo.fuelAmount);
		int amount = custInfo.fuelAmount; 
		printf("fuelAmount %d \n", custInfo.fuelAmount);*/
		pumpFuel(custInfo.fuelType, custInfo.fuelAmount);
		//TODO: for now, i don't think that the pumpFuel needs to be concurrent with this thread? may need to change if problems arise
		donePump.Signal();	//signal that pump done pumping gas
	}
	//end test area-----------

	pipeComm.WaitForThread(); 
	//TODO: need to add rendesvous here??

	return(0);
}