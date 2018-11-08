//Justine Sultan
//Student Number 35880146

//Gas Station Computer process
//Parent Process for Pump process and Customer (Car) process

#include	<stdio.h>
#include	<stdlib.h>
#include	<string> 
#include	"..\rt.h"			
#include    "..\GSDataStructures.h"
#include	"..\tank.h"
#include	<vector>
#include	<ctime>
#include	<chrono>
//#include    <windows.h>

CRendezvous     r1("MyRendezvous", 6);			//Rendesvous to synchronize GSC, 4 pumps, and customer 
CRendezvous		r2("EndRendezvous", 3);			//Rendesvous to synchronize termination of 3 processes
CRendezvous     gscR("gscRendezvous", 6);		//Internal rendesvous for GSC: parent process and 5 child threads (for pumps and fuelt tanks)

bool GSC_DEBUG = false; 
vector<transaction*> tr_history;				//Vector for storing customer transactions
bool thDisplayed = false;						//Global flag indicating if th is displayed
int long thSize = 0;							//Global for storing number of displayed transactions
//int refillTank;									
//int priceTank;									
//float price; 

int timeStamp(char* timeString) {				//Return time and copy timeStamp in military time (00:00:00 hours/min/secs)
	long int timeSinceStart = std::chrono::duration_cast< std::chrono::seconds >(std::chrono::system_clock::now().time_since_epoch()).count();
	long int hours = timeSinceStart / 3600;
	long int minutes = (timeSinceStart - (hours * 3600)) / 60;
	long int seconds = timeSinceStart - (hours * 3600) - (minutes * 60);
	hours = (hours % 24) - 8;

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
	
	strcpy_s(timeString, 100, timeStamp.c_str());

	int retVal = timeSinceStart - 1541500000; 
	return retVal;
}

UINT __stdcall pumpThread(void *ThreadArgs)		//Child thread for pump interaction
{	
	CMutex  mDOS("gsc_DOS_window");				
	CMutex  mTr("transaction_history");			
	
	int pumpID = *(int *)(ThreadArgs); 
	if (GSC_DEBUG) { printf("GSC thread for reading pump DP %i created \n", pumpID); }
	CSemaphore		ps("producerPump"+ std::to_string(pumpID), 0, 1);    
	CSemaphore		cs("consumerPump" + std::to_string(pumpID), 1, 1);    

	CDataPool datapool_Pump("PumpDP"+ std::to_string(pumpID), sizeof(struct pumpDatapool));
	struct pumpDatapool *dpPump = (struct pumpDatapool *)(datapool_Pump.LinkDataPool());
	if (GSC_DEBUG) { printf("Parent created datapool Pump 1 at address %p.....\n", dpPump); }

	CEvent startPump("Event_startPump" + std::to_string(pumpID));	//Event to authorise pump to start pumping gas for customer
	CEvent donePump("Event_donePump" + std::to_string(pumpID));		//Event to let GSC/customer know that pump has finished pumping gas

	int y_cursor;
	switch (pumpID) {
	case 1:
		y_cursor = 10;
		break;
	case 2:
		y_cursor = 17;
		break;
	case 3:
		y_cursor = 25;
		break;
	case 4:
		y_cursor = 33;
		break;
	}

	gscR.Wait(); 

	while (1) {

		ps.Wait(); 

		mDOS.Wait();
		MOVE_CURSOR(0, y_cursor +2);
		printf("Customer waiting. Press 'e' + %d to authorize \n", pumpID);
		MOVE_CURSOR(15, y_cursor + 3);
		printf("Name: %s \n", dpPump->name);
		MOVE_CURSOR(15, y_cursor + 4);
		printf("Credit Card #: %d \n", dpPump->creditCard);
		MOVE_CURSOR(15, y_cursor + 5);
		printf("Fuel Type and Amount: %d, %d \n", dpPump->fuelType, dpPump->fuelAmount);
		MOVE_CURSOR(15, y_cursor + 6);
		printf("Time of Transaction: %s \n", dpPump->timeStamp);
		fflush(stdout);
		mDOS.Signal();

		cs.Signal();		

		startPump.Wait();	

		mDOS.Wait();
		MOVE_CURSOR(0, y_cursor + 1);
		TEXT_COLOUR(10, 0);
		printf("Pump enabled              \n");
		TEXT_COLOUR(7, 0);
		MOVE_CURSOR(0, y_cursor + 2);
		printf("Transaction authorized...                                       \n");
		fflush(stdout);
		mDOS.Signal();

		struct transaction customerTr;
		strcpy_s(customerTr.name, 20, dpPump->name);
		customerTr.creditCard = dpPump->creditCard;
		customerTr.fuelType = dpPump->fuelType;
		customerTr.fuelAmount = dpPump->fuelAmount;
		strcpy_s(customerTr.timeStamp, 100, dpPump->timeStamp);
		customerTr.pump = pumpID;
		mTr.Wait();
		tr_history.push_back(&customerTr);
		mTr.Signal();

		donePump.Wait();			

		mDOS.Wait();
		MOVE_CURSOR(0, y_cursor+1);
		TEXT_COLOUR(14, 0);
		printf("Pump disabled                                                    \n");
		TEXT_COLOUR(7, 0);
		MOVE_CURSOR(0, y_cursor + 2);
		printf("Customer returning hose and driving away...                      \n");
		MOVE_CURSOR(15, y_cursor + 3);
		printf("--                                                               \n");
		MOVE_CURSOR(15, y_cursor + 4);
		printf("--                                                               \n");
		MOVE_CURSOR(15, y_cursor + 5);
		printf("--                                                               \n");
		MOVE_CURSOR(15, y_cursor + 6);
		printf("--                                                               \n");
		fflush(stdout);
		mDOS.Signal();

		donePump.Wait();
		
		mDOS.Wait();
		MOVE_CURSOR(0, y_cursor + 2);
		printf("No customer at pump.						                     \n");
		fflush(stdout);
		mDOS.Signal();
		
	}

	return 0;									
}

UINT __stdcall tankThread(void *ThreadArgs)			//Child thread to monitor fuel tanks
{
	CMutex  mDOS("gsc_DOS_window");			
	CCondition lowTank("lowTank");
	if (GSC_DEBUG) { printf("Creating fuel tanks....\n"); }
	Tank fuelTanks;									
	bool flashToggle = false; 
	char mytimeStamp[100];
	
	gscR.Wait();
	
	int prevTime = timeStamp(mytimeStamp);

	while (1) {
		mDOS.Wait();

		MOVE_CURSOR(40, 0);
		timeStamp(mytimeStamp);
		printf("%s", mytimeStamp);

		MOVE_CURSOR(0, 5); 
		printf("Tank 1: Regular-87   |  %.2f          |\n", fuelTanks.getPrice(0));
		MOVE_CURSOR(0, 6);
		printf("Tank 2: MidGrade-89  |  %.2f          |\n", fuelTanks.getPrice(1));
		MOVE_CURSOR(0, 7);
		printf("Tank 3: Premium-91   |  %.2f          |\n", fuelTanks.getPrice(2));
		MOVE_CURSOR(0, 8);
		printf("Tank 4: Super-99     |  %.2f          |\n", fuelTanks.getPrice(3));
		fflush(stdout);
		mDOS.Signal();

		int i = 0; 
		while(i < 4){
			if (fuelTanks.readAmount(i) <= 200) {

				if( (timeStamp(mytimeStamp) - prevTime) >=1){
					prevTime = timeStamp(mytimeStamp);
					flashToggle = !flashToggle; 
				}
				if (flashToggle) {
					mDOS.Wait();
					MOVE_CURSOR(38, (5 + i));
					printf("|                         \n ", fuelTanks.readAmount(i));
					fflush(stdout);
					mDOS.Signal();
				}
				else {
					mDOS.Wait();
					MOVE_CURSOR(38, (5 + i));
					TEXT_COLOUR(12, 0);
					printf("| %.2f  RE-FUEL             ", fuelTanks.readAmount(i));
					fflush(stdout);
					TEXT_COLOUR(7, 0);
					mDOS.Signal();
				}
			}
			else {
				if (1) {
					mDOS.Wait();
					MOVE_CURSOR(38, (5 + i));
					printf("| %.2f                      \n", fuelTanks.readAmount(i));
					fflush(stdout);
					mDOS.Signal();
				}
				
			}
			
			i++; 
		}

		if ( (fuelTanks.readAmount(0) > 200) && (fuelTanks.readAmount(1) > 200) && (fuelTanks.readAmount(2) > 200) && (fuelTanks.readAmount(3) > 200)) {
		lowTank.Signal();
		}
	}

	return 0;									
}

long int displayTH(void) {					//Function to display transaction history
	CMutex  mDOS("gsc_DOS_window");			
	CMutex  mTr("transaction_history");		
	mTr.Wait();
	long int size = tr_history.size();
	mTr.Signal(); 
	char name[20];	
	int creditCard;
	int fuelType;
	int fuelAmount;
	int pump;	
	char timeStamp[100];

	mDOS.Wait();
	MOVE_CURSOR(0, 55);
	printf("Displaying transaction history (first to last) \npress 'th' + spacebar to hide. %i", size);
	fflush(stdout);
	mDOS.Signal();
	
	long int i = 0;
	while(i<size)
	{
		mTr.Wait();
		strcpy_s(name, 20, tr_history[i]->name);	
		creditCard = tr_history[i]->creditCard;
		fuelType = tr_history[i]->fuelType;
		fuelAmount= tr_history[i]->fuelAmount;
		pump= tr_history[i]->pump;	
		strcpy_s(timeStamp, 100, tr_history[i]->timeStamp); 
		mTr.Signal(); 
		
		mDOS.Wait();
		MOVE_CURSOR(0, (58 + i * 7));
		printf("Name: %s \n", name);
		printf("Credit Card #: %d \n", creditCard);
		printf("Fuel Type: %d \n", fuelType);
		printf("Fuel Amount: %d \n", fuelAmount);
		printf("Pump: %d \n", pump);
		printf("Time of Transaction: %s \n\n", timeStamp);
		fflush(stdout);
		mDOS.Signal();

		i++; 
	}

	thDisplayed = true; 
	return size; 
}

void hideTH(long int size) {				//Function to display transaction history
	CMutex  mDOS("gsc_DOS_window");			
	CMutex  mTr("transaction_history");		

	mDOS.Wait();
	MOVE_CURSOR(0, 55);
	printf("                                                                                \n");
	MOVE_CURSOR(0, 56);
	printf("                                                                                \n");
	fflush(stdout);
	mDOS.Signal();

	long int i = 0;
	while(i < size) {
		mDOS.Wait();
		MOVE_CURSOR(0, (58+i*7)) ;
		printf("                                                     \n");
		printf("                                                     \n"); 
		printf("                                                     \n");
		printf("                                                     \n");
		printf("                                                     \n");
		printf("                                                     \n\n");
		fflush(stdout);
		mDOS.Signal();

		i++; 
	}

	thDisplayed = false;
	return;
}

bool doCommand(std::string commandBuff, Tank* tanks) {			//Function to parse user commands
	bool valid = false; 
	int index = -1; 
	float price = -1; 
	CMutex  mDOS("gsc_DOS_window");			

	if (commandBuff.size() == 2) {
		if (commandBuff.compare("th") == 0) {
			valid = true; 
			if (!thDisplayed) {
				thSize = displayTH();
			}
			else {	
				hideTH(thSize); 
			}
		}
		else if ((commandBuff[0] == 'e' || commandBuff[0] == 'f') && (commandBuff[1] == '1' ||
			commandBuff[1] == '2' || commandBuff[1] == '3' || commandBuff[1] == '4'))
		{
			valid = true; 
			if (commandBuff[0] == 'e') {
				index = std::stoi(commandBuff.substr(1, string::npos));
				CEvent startPump("Event_startPump" + std::to_string(index));	
				startPump.Signal(); 
			}
			if (commandBuff[0] == 'f') {
				index = std::stoi(commandBuff.substr(1, string::npos));
				tanks->setAmount(500, (index - 1));
			}
		}
	}
	else if ((commandBuff[0] == 'p') && (commandBuff[1] == '1' ||
		commandBuff[1] == '2' || commandBuff[1] == '3' || commandBuff[1] == '4'))
	{
		index = commandBuff[1]; 
		try {
			price = std::stof(commandBuff.substr(2, string::npos));
			//Past exception point
			valid = true; 
			index = std::stoi(commandBuff.substr(1, 1));
			tanks->setPrice(price, (index-1)); 
		}
		catch (std::exception& ex) {
			mDOS.Wait();
			MOVE_CURSOR(0, 53);
			printf("Error: Invalid price");
			fflush(stdout);
			mDOS.Signal();
		}
	}

	if (valid) {
		mDOS.Wait();
		MOVE_CURSOR(0, 53);
		printf("Command %s proccessed                                                          ", commandBuff.c_str());
		fflush(stdout);
		mDOS.Signal();
	}
	else {
		mDOS.Wait();
		MOVE_CURSOR(0, 53);
		printf("Error: %s is invalid command                                                   ", commandBuff.c_str());
		fflush(stdout);
		mDOS.Signal();
	}

	return valid; 
	
}

int	main()
{
	CMutex  mDOS("gsc_DOS_window");

	if (GSC_DEBUG) { printf("Creating Customer child proccess....\n"); }
	CProcess p1("C:\\Users\\jmvsu\\OneDrive\\Documents\\School_Fall_2018\\cpen_333\\Assignment_1\\Gas_Station_Assignment1\\Debug\\Customers.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		PARENT_WINDOW,					// process has its own window					
		ACTIVE							// process is active immediately
	);

	if (GSC_DEBUG) { printf("Creating Pump child proccess....\n"); }
	CProcess p2("C:\\Users\\jmvsu\\OneDrive\\Documents\\School_Fall_2018\\cpen_333\\Assignment_1\\Gas_Station_Assignment1\\Debug\\Pumps.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	//Creating pump and fuel tank communication threads/paramaters
	if (GSC_DEBUG) { printf("Creating fuelTanks and GSC/Pump threads....\n"); }
	Tank *tanks = new Tank();
	int num1 =1;
	int num2 = 2;
	int num3 = 3;
	int num4 = 4;
	CThread	pumpThread1(pumpThread, ACTIVE, &num1);
	CThread	pumpThread2(pumpThread, ACTIVE, &num2);
	CThread	pumpThread3(pumpThread, ACTIVE, &num3);
	CThread	pumpThread4(pumpThread, ACTIVE, &num4);
	CThread	tankThread(tankThread, ACTIVE);

	//Setting up DOS window
	int init = -1;		
	mDOS.Wait();
	MOVE_CURSOR(0, 0);
	printf("GAS STATION COMPUTER \n");
	printf("For menu, scroll to bottom of screen. \n\n");

	printf("FUEL STATUS: \n");
	printf("Tank and Fuel Grade  | Cost per Liter | Amount left in Tank\n");
	printf("Tank 1: Regular-87   |                | \n");
	MOVE_CURSOR(23, 5); 
	printf("%d\n", init);
	MOVE_CURSOR(40, 5);
	printf("%d\n", init);
	printf("Tank 2: MidGrade-89  |                | \n");
	printf("Tank 3: Premium-91   |                | \n"); 
	printf("Tank 4: Super-99     |                | \n\n");

	printf("PUMP 1 STATUS: \n");
	TEXT_COLOUR(14, 0);
	printf("Pump disabled \n");
	TEXT_COLOUR(7, 0);
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 13);
	printf("-- \n");
	MOVE_CURSOR(15, 14);
	printf("-- \n");
	MOVE_CURSOR(15, 15);
	printf("-- \n\n");

	printf("PUMP 2 STATUS: \n");
	TEXT_COLOUR(14, 0);
	printf("Pump disabled \n");
	TEXT_COLOUR(7, 0);
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 21);
	printf("-- \n");
	MOVE_CURSOR(15, 22);
	printf("-- \n");
	MOVE_CURSOR(15, 23);
	printf("-- \n\n");

	printf("PUMP 3 STATUS: \n");
	TEXT_COLOUR(14, 0);
	printf("Pump disabled \n");
	TEXT_COLOUR(7, 0);
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 29);
	printf("-- \n");
	MOVE_CURSOR(15, 30);
	printf("-- \n");
	MOVE_CURSOR(15, 31);
	printf("-- \n\n");

	printf("PUMP 4 STATUS: \n");
	TEXT_COLOUR(14, 0);
	printf("Pump disabled \n");
	TEXT_COLOUR(7, 0);
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 37);
	printf("-- \n");
	MOVE_CURSOR(15, 38);
	printf("-- \n");
	MOVE_CURSOR(15, 39);
	printf("-- \n\n");

	printf("ACTION MENU: \n");
	printf("Enter 't' + 'h' to view transaction history \n");
	printf("Enter 'p'+ (Tank Number) + (Price in dollars) to set price per liter of fuel \n");
	printf("       For example 'p31.00' sets the fuel in tank 3 to $1.00 per liter. \n");
	printf("       Price can be any whole or decimal number.  \n");
	printf("Enter 'e' + (Pump Number) to authorize/enable a pump to pump gas.\n");
	printf("       Note that pumps will automatically be disabled after pumping \n       the authorized amount of gas, \n");
	printf("       or if the tank being pumped from runs out of gas.\n");
	printf("Enter 'f' + (Tank number) to refill a tank. \n");
	printf("Press spacebar to submit command: ");  //50
	fflush(stdout);
	mDOS.Signal(); 

	if (GSC_DEBUG) { printf("Sleeping for 3 secs to demonstrate rendesvous method for lab Demo \n"); }
	if (GSC_DEBUG) { printf("Will pause for 3 secs before other process's thread's resume \n"); }
	Sleep(2000);		
	gscR.Wait();	
	r1.Wait();	

	//Paramaters for getting user input
	std::string commandBuff; 
	std::string priceBuff;  
	char getChar[2] = { '\0', '\0'};
	int count = 0; 
	int maxSize = 30; 

	//Main routine: parse user input
	while (1) {
		if (TEST_FOR_KEYBOARD() != 0) {
			getChar[0] = _getch(); 
			if (getChar[0] == ' ') {
				if (commandBuff.size() > maxSize) {
					mDOS.Wait();
					MOVE_CURSOR(0, 53);
					printf("Error: command too large                                                      ");	
					fflush(stdout);
					mDOS.Signal();
					commandBuff.clear();
				}
				else {
					mDOS.Wait();
					MOVE_CURSOR(0, 53);
					printf("                              ");	
					MOVE_CURSOR(0, 53);
					printf("%s", commandBuff.c_str());
					fflush(stdout);
					mDOS.Signal();
					//parse here
					doCommand(commandBuff, tanks); 
					commandBuff.clear();
				}
			}
			else {
				commandBuff.append(getChar);			
			}

		}
		Sleep(50); 
	}

	p1.WaitForProcess();	
	p2.WaitForProcess();

	r2.Wait(); 

	return 0;
}

