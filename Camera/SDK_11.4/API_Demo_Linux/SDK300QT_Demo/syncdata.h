#ifndef SYNCDATA_H
#define SYNCDATA_H

#include <QObject>
#include <QAtomicInt>

extern QAtomicInt isCommunicationInit;
extern QAtomicInt isConnectedToHive;
extern QAtomicInt isFirstSuccessCommunication;
extern QAtomicInt shouldRun;

class SyncData
{
public:
    static void process();

};

#endif // SYNCDATA_H
