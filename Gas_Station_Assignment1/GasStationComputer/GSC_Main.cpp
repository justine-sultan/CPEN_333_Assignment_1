//Justine Sultan
//Student Number 35880146

//Gas Station Computer process
//Parent Process for Pump Process and Customer (Car) process
//Will link to 4 datapools with the 4 pumps

#include	<stdio.h>
#include	<stdlib.h>
#include	<string> 
#include	"..\rt.h"			
#include    "..\GSDataStructures.h"
#include	"..\tank.h"
#include	<vector>

//add global rendesvous here and to all processes
CRendezvous     r1("MyRendezvous", 3);	//TODO update to 6 to start GSC,4 pumps, and customer 
CRendezvous		r2("EndRendezvous", 3); //to synchronize termination of 3 processes
CRendezvous     gscR("gscRendezvous", 3);	//internal rendesvous for GSC's 5 threads - need to eventually be 6 (5 threads plus parent process)

bool GSC_DEBUG = false; 
vector<transaction*> tr_history;	//vector for storing customer transactions

//thread class for reading pump datapools. TODO: move datapool creation into here as well?
UINT __stdcall pumpThread(void *ThreadArgs)
{	
	CMutex  mDOS("gsc_DOS_window");			//mutex to protect writing to the window
	CMutex  mTr("transaction_history");		//mutex to protect access to transaction_history buffer
	//GSC to pump comm
	int pumpID = *(int *)(ThreadArgs); 
	if (GSC_DEBUG) { printf("GSC thread for reading pump DP %i created \n", pumpID); }
	CSemaphore		ps("producerPump"+ std::to_string(pumpID), 0, 1);    // semaphore with initial value 0 and max value 1
	CSemaphore		cs("consumerPump" + std::to_string(pumpID), 1, 1);    // semaphore with initial value 1 and max value 1

	CDataPool datapool_Pump("PumpDP"+ std::to_string(pumpID), sizeof(struct pumpDatapool));
	struct pumpDatapool *dpPump = (struct pumpDatapool *)(datapool_Pump.LinkDataPool());
	if (GSC_DEBUG) { printf("Parent created datapool Pump 1 at address %p.....\n", dpPump); }

	CEvent startPump("Event_startPump" + std::to_string(pumpID));	//event to authorise pump to start pumping gas for customer
	CEvent donePump("Event_donePump");							//event to let GSC/customer know that pump has finished pumping gas

	gscR.Wait(); 

	// Testing code for Lab3/4---------------------
	//struct pumpDatapool *dpPumps[4]; 
	while (1) {
		//printf("Waiting for customer to arrive...\n");

		ps.Wait();
		//printf("Recieved customer input...\n");
		
		//printf("Name: %s \n", dpPump->name);
		//printf("Credit Card #: %d \n", dpPump->creditCard);
		//printf("Fuel Type: %d \n", dpPump->fuelType);
		//printf("Fuel Amount: %d \n", dpPump->fuelAmount);
		//printf("Time of Transaction: %s \n", dpPump->timeStamp);
		//printf("Verifying and recieving credit card transaction...\n");
		Sleep(2000);	//TODO: replace this sleep with prompt for user to authorize pump
		
		//record transaction here
		//TODO: this block of code causing GSC process to crash right now!! (if commented out it works)
		struct transaction customerTr; 
		strcpy_s(customerTr.name, 20, dpPump->name);
		customerTr.creditCard = dpPump->creditCard; 
		customerTr.fuelType = dpPump->fuelType;
		customerTr.fuelAmount = dpPump->fuelAmount;
		strcpy_s(customerTr.timeStamp, 1000, dpPump->timeStamp);
		customerTr.pump = pumpID; 
		mTr.Wait();
		tr_history.push_back(&customerTr);
		mTr.Signal(); 
		//done recording

		cs.Signal();

		startPump.Signal();			//authorise pump to start pumping to customer
		donePump.Wait();			//wait for pump to stop pumping gas

		mDOS.Wait();
		MOVE_CURSOR(0, 55);
		printf("finished transaction\n ");
		fflush(stdout);
		mDOS.Signal();
		
	}

	return 0;									// thread ends here
}

UINT __stdcall tankThread(void *ThreadArgs)
{
	CMutex  mDOS("gsc_DOS_window");			//mutex to protect writing to the window
	if (GSC_DEBUG) { printf("Creating fuel tanks....\n"); }
	Tank fuelTanks;			//all instantiations of the Tank monitor link to same datapool

	//GSC to pump comm
	gscR.Wait();

	// Testing code for Lab3/4---------------------
	if (GSC_DEBUG) { printf("Viewing levels of fuel in tank 1...\n"); }
	while (1) {
		//printf("Level in fuel tank: %f \n", fuelTanks.readAmount(0));
		//Sleep(500);
	}

	return 0;									// thread ends here
}

void displayTH(void) {
	CMutex  mDOS("gsc_DOS_window");			//mutex to protect writing to the window
	CMutex  mTr("transaction_history");		//mutex to protect access to transaction_history buffer
	long int size = tr_history.size();

	mDOS.Wait();
	MOVE_CURSOR(0, 55);
	printf("Displaying transaction history (first to last) \npress 'tr' + spacebar to hide. ");
	fflush(stdout);
	mDOS.Signal();
	
	for (long int i = 0; i++; i < size) {
		mDOS.Wait();
		MOVE_CURSOR(0, 58);
		printf("Name: %s \n", tr_history[i]->name);
		printf("Credit Card #: %d \n", tr_history[i]->creditCard);
		printf("Fuel Type: %d \n", tr_history[i]->fuelType);
		printf("Fuel Amount: %d \n", tr_history[i]->fuelAmount);
		printf("Time of Transaction: %s \n", tr_history[i]->timeStamp);
		fflush(stdout);
		mDOS.Signal();
	}

	return; 
}

bool doCommand(std::string commandBuff) {
	bool valid = false; 
	int index = -1; 
	float price = -1; 
	CMutex  mDOS("gsc_DOS_window");			//mutex to protect writing to the window

	if (commandBuff.size() == 2) {
		if (commandBuff == "th") {
			valid = true; 
			//do th command
		}
		else if ((commandBuff[0] == 'e' || commandBuff[0] == 'f') && (commandBuff[1] == '1' ||
			commandBuff[1] == '2' || commandBuff[1] == '3' || commandBuff[1] == '4'))
		{
			valid = true; 
			if (commandBuff[0] == 'e') {
				//do e stuff
			}
			if (commandBuff[0] == 'f') {
				//do f stuff
			}
		}
	}
	else if ((commandBuff[0] == 'p') && (commandBuff[1] == '1' ||
		commandBuff[1] == '2' || commandBuff[1] == '3' || commandBuff[1] == '4'))
	{
		index = commandBuff[1]; 
		//commandBuff.erase(0,2);		//delete first two characters, leaving just the price
		try {
			price = std::stof(commandBuff.substr(2, string::npos));
			valid = true; 
			//deal with p command
		}
		catch (std::exception& ex) {
			mDOS.Wait();
			MOVE_CURSOR(0, 53);
			//printf("%s", ex.what());
			printf("Error: Invalid price");
			fflush(stdout);
			mDOS.Signal();
		}
	}

	if (valid) {
		mDOS.Wait();
		MOVE_CURSOR(0, 53);
		//printf("%s", ex.what());
		printf("Command %s proccessed            ", commandBuff.c_str());
		fflush(stdout);
		mDOS.Signal();
	}
	else {
		mDOS.Wait();
		MOVE_CURSOR(0, 53);
		//printf("%s", ex.what());
		printf("Error: %s is invalid command      ", commandBuff.c_str());
		fflush(stdout);
		mDOS.Signal();
	}

	return valid; 
	
}

int	main()
{
	//mutex to protect writing to the window
	CMutex  mDOS("gsc_DOS_window");

	if (GSC_DEBUG) { printf("Creating Customer child proccess....\n"); }
	CProcess p1("C:\\Users\\jmvsu\\OneDrive\\Documents\\School_Fall_2018\\cpen_333\\Assignment_1\\Gas_Station_Assignment1\\Debug\\Customers.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	if (GSC_DEBUG) { printf("Creating Pump child proccess....\n"); }
	CProcess p2("C:\\Users\\jmvsu\\OneDrive\\Documents\\School_Fall_2018\\cpen_333\\Assignment_1\\Gas_Station_Assignment1\\Debug\\Pumps.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	//GSC will create all pump and fuel tank data pools to simplify things
	//creating GSC/PUMP datapools and create the corresponding thread object in suspended state
	if (GSC_DEBUG) { printf("Creating GSC/Pump threads....\n"); }
	int num =1;
	CThread	pumpThread1(pumpThread, ACTIVE, &num);
	//gsc create thread to monitor fuel tanks
	CThread	tankThread(tankThread, ACTIVE);

	//set up DOS window:
	int init = -1;	//placeholder to check cursor placement
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
	printf("Pump enabled \n");
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 13);
	printf("-- \n");
	MOVE_CURSOR(15, 14);
	printf("-- \n");
	MOVE_CURSOR(15, 15);
	printf("-- \n\n");

	printf("PUMP 2 STATUS: \n");
	printf("Pump enabled \n");
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 21);
	printf("-- \n");
	MOVE_CURSOR(15, 22);
	printf("-- \n");
	MOVE_CURSOR(15, 23);
	printf("-- \n\n");

	printf("PUMP 3 STATUS: \n");
	printf("Pump enabled \n");
	printf("No Customer at pump \n");
	printf("Customer Info: -- \n");
	MOVE_CURSOR(15, 29);
	printf("-- \n");
	MOVE_CURSOR(15, 30);
	printf("-- \n");
	MOVE_CURSOR(15, 31);
	printf("-- \n\n");

	printf("PUMP 4 STATUS: \n");
	printf("Pump enabled \n");
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
	printf("Enter 'p'+ (Tank Number) + (Price in dollars) to set price per liter of fuel between 0.00 and 9.99. \n");
	printf("       For example 'f31.00' sets the fuel in tank 3 to $1.00 per liter. \n");
	printf("       Note price MUST be entered as [dollar].[cent][cent] e.g. 1.00 or 2.39  \n");
	printf("Enter 'e' + (Pump Number) to authorize/enable a pump to pump gas.\n");
	printf("       Note that pumps will automatically be disabled after pumping \n       the authorized amount of gas, \n");
	printf("       or if the tank being pumped from runs out of gas.\n");
	printf("Enter 'f' + (Tank number) to refill a tank. \n");
	printf("Press spacebar to submit command: ");  //50
	fflush(stdout);
	mDOS.Signal(); 

	if (GSC_DEBUG) { printf("Sleeping for 3 secs to demonstrate rendesvous method for lab Demo \n"); }
	if (GSC_DEBUG) { printf("Will pause for 3 secs before other process's thread's resume \n"); }
	Sleep(3000);		//for lab demo
	gscR.Wait();	//wait for 5 threads to create datapools, semaphores, etc
	r1.Wait();	//wait for other processes/threads to start before begin

	//main routine:check for inputs from gsc computor
	std::string commandBuff; 
	std::string priceBuff;  
	char getChar[2] = { '\0', '\0'};
	int count = 0; 
	int maxSize = 30; 

	while (1) {
		if (TEST_FOR_KEYBOARD() != 0) {
			getChar[0] = _getch(); 
			if (getChar[0] == ' ') {
				if (commandBuff.size() > maxSize) {
					mDOS.Wait();
					MOVE_CURSOR(0, 53);
					printf("Error: command too large");	
					fflush(stdout);
					mDOS.Signal();
					commandBuff.clear();
				}
				else {
					//commandBuff has the string that the user entered
					mDOS.Wait();
					MOVE_CURSOR(0, 53);
					printf("                              ");	//30 white spaces for clearing maxSize
					MOVE_CURSOR(0, 53);
					printf("%s", commandBuff.c_str());
					fflush(stdout);
					mDOS.Signal();
					//parse here
					doCommand(commandBuff); 
					commandBuff.clear();
				}
			}
			else {
				commandBuff.append(getChar);			// read next character from keyboard
			}

		}
	}

	p1.WaitForProcess();	//these can't be replaced by rendesvous
	p2.WaitForProcess();

	r2.Wait(); 

	return 0;
}

/*char KeyData[2] = { '\0', '\0'};	// 2 character keyboard command 
	char Price[5]= { '\0', '\0' , '\0' , '\0', '\0'};   // 5 character string for the price command
	int count = 0; 
	while (1) {
		if (TEST_FOR_KEYBOARD() != 0) {
			count++; 
			KeyData[0] = KeyData[1];			// move up previous character read
			KeyData[1] = _getch();			// read next character from keyboard
			if (count == 2) {
				if (KeyData[0] == 'p') {
					while (count<6){
						Price[count - 2] = _getch();
						count++; 
					}
					mDOS.Wait();
					MOVE_CURSOR(0, 52);
					printf("%c%c", KeyData[0], KeyData[1]);
					MOVE_CURSOR(3, 52);
					printf("%s \n", Price);
					mDOS.Signal();
					doCommand(KeyData, Price);
					count = 0;
				}
				else {
					mDOS.Wait();
					MOVE_CURSOR(0, 52);
					printf("%c%c       ", KeyData[0], KeyData[1]);
					//do whatever else with command??
					mDOS.Signal();
					doCommand(KeyData);
					count = 0;
				}	
			}
		}
	}*/


/*//mutex to protect writing to the window
	CMutex  mDOS("gsc_DOS_window");
	int num = -1; 
	bool valid = false; 
	std::string strPrice(price_string);
	float price = std::stof(strPrice);

	if (command[0] == 't' && command[1] == 'h') {
		valid = true; 
		//send command to view transactions (probably just use another process/thread with own window to display
	}
	else {
		switch (command[1]) {
			case '1':
				valid = true;
				num = 1;
				break;
			case '2':
				valid = true;
				num = 2;
				break;
			case '3':
				valid = true;
				num = 3;
				break;
			case '4':
				valid = true;
				num = 4;
				break;
			default:
				valid = false;
				break;
		}

		if (valid == true) {
			switch (command[0]) {
				case 'p':
					if (price >= 0 && price <= 0.00) {
						//send set price command
					}
					else {
						valid = false; 
					}
					break;
				case 'e':
					//send enable pump command
					break;
				case 'f':
					//send fill pump command
					break;
				default:
					valid = false;
					break;
			}
		}
	}

	if (valid) {
		mDOS.Wait();
		MOVE_CURSOR(0, 53);
		printf("Command %c%c completed........\n", command[0], command[1]);
		mDOS.Signal();
		return true; 
	}
	else {
		mDOS.Wait();
		MOVE_CURSOR(0, 53);
		printf("Error: %c%c is invalid command", command[0], command[1]);
		mDOS.Signal();
		return false; 
	}*/