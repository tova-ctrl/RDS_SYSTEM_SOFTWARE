#include "syncdata.h"

#include <thread>
#include <chrono>
#include <QAtomicInt>

extern "C"
{
#include "SDK/stkAPI.h"
}

QAtomicInt isCommunicationInit(0);
QAtomicInt isConnectedToHive(0);
QAtomicInt isFirstSuccessCommunication(0);
QAtomicInt shouldRun(1);

void SyncData::process()
{
    int unsuccessfulCommCount = 0;
    int ret = 0;
    bool firstExec = true;

    while (true)
    {
        if (!shouldRun.load())
            break;

        if (isCommunicationInit.load() > 0)
        {
            if (firstExec)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                firstExec = false;
            }

            ret = STKSyncLocal(20); // recieve data via UDP and update the structs.
            // we should not worry about this thread hogging the cpu because the STKSyncLocal will wait until the next packet,
            // thus matching it's frequency.
            if (ret != 0) {
                // If UDP read was a unsuccessful.
                //
                // the first couple of 'STKSyncLocal(20)' calles return -1,
                // So, set 100 cycle before checking and handling the unsuccessful
                // connection event.


                if (unsuccessfulCommCount == 10)
                {
                    unsuccessfulCommCount = 11;
                    isConnectedToHive.store(0);
                }
                else
                {
                    if(unsuccessfulCommCount < 11)
                        ++unsuccessfulCommCount;
                }
            }
            else
            {
                if (isFirstSuccessCommunication.load() == 0)
                    isFirstSuccessCommunication.store(1);
                isConnectedToHive.store(1);
                unsuccessfulCommCount = 0;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
