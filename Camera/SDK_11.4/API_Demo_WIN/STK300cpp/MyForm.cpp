#include "MyForm.h"
#include "Globals.h"

#include <atomic>

using namespace STK300cpp;

using namespace System::Threading;
using namespace System;
using namespace System::Windows::Forms;

int isCommunicationInit = 0;									// Flag for successful STKInitializeCommunication() call;
int isConnectedToHive = 0;										// Flag for successful communication.
int isNotConnectedToHive = 0;									// Flag for unsuccessful communication.
int isFirstSuccessCommunication = 0;
int isUnSuccessCommunication = 0;								// Counter for unsuccessful connection event.
std::atomic<bool> should_exit{ false };
int counter1 = 0;
int counter2 = 0;
int delayCounter = 0;											// Counter for Initial communication delay.

void system_structs_update()
{
	int ret = 0;
	msgBoxForm msgForm1;

	while (1)
	{
		if (should_exit.load())
			break;

		if (isCommunicationInit > 0)							// Wait for connection to be initialise.
		{
			if (delayCounter > 400000)							// Set Delay between connection initialise and first attempt to read somthin from the UDP, for 400,000 cycles.
			{
				ret = STKSyncLocal(10); // Recive the data via udp and update the structs.
				if (ret != 0)
				{	
					// If UDP read was a unsuccessful.
					// 
					// the first couple of 'STKSyncLocal(10)' calles return -1,
					// So, set 100 cycle before checking and handling the unsuccessful
					// connection event.
					if (isUnSuccessCommunication == 100)
					{
						isUnSuccessCommunication = 101;			// Step here only once and stop increment the counter.
						isNotConnectedToHive = 1;				// Raise the flag for unsuccessful communication.
					}
					else
					{
						// If 100 cycle has not finished - do nothing and increment the counter.
						if(isUnSuccessCommunication != 101)
							++isUnSuccessCommunication;			
					}
				}
				else
				{			
					// If UDP read was successful.
					if (isFirstSuccessCommunication == 0)
					{
						isFirstSuccessCommunication = 1;		// Step here only once.
						isConnectedToHive = 1;					// Raise the flag for successful communication.
					}
				}
				SetKeepAlive();
			}
			else
			{
				delayCounter++;
			}
		}

		// Sleep after 1,000,000 cycles.
		if (++counter1 == 1000)
		{
			counter1 = 0;
			if (++counter2 == 1000)
			{
				counter2 = 0;
				_sleep(1);
			}
		}
	}
}

void Main(void)
{
	std::thread thread_obj(system_structs_update);

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	STK300cpp::MyForm form;
	Application::Run(% form);

	should_exit.store(true);

	thread_obj.join();

	return;
}


