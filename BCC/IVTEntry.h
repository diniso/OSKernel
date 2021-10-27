/*
 * IVTEntry.h
 *
 *  Created on: May 4, 2020
 *      Author: OS1
 */

#ifndef BCC_IVTENTRY_H_
#define BCC_IVTENTRY_H_

#include "Semaphor.h"
#include "Thread.h"

typedef void interrupt (*interupt)(...);


#define lock asm {pushf; \
		cli;}
#define unlock asm popf


#define PREPAREENTRY(numEntry, callOld)\
void interrupt inter##numEntry(...); \
IVTEntry newEntry##numEntry(numEntry, inter##numEntry); \
void interrupt inter##numEntry(...) {\
newEntry##numEntry.signal();\
if (callOld == 1)\
newEntry##numEntry.pozovi();\
}


class IVTEntry {

public:

	Semaphore* sem;
	int IDThread;
private :
	unsigned char brUlaza;
	unsigned callOld , pozoviPrethodnu; // ovo se ubacuje posle pravljenja klase
	interupt prethodnaRutina;

public:
	//static IVTEntry all_entry[256];
	IVTEntry(unsigned char ulaz , interupt prekid);
	~IVTEntry();

	void signal() {
		sem->signal(0);
	}
	void wait() {
		if (Thread::getRunningId()==IDThread) {
			sem->wait(0);
		}
	}

	void setOld(unsigned old) {callOld = old;}
	unsigned getOld() {return callOld;}

	void pozovi(); // poziva staru rutinu
};



#endif /* BCC_IVTENTRY_H_ */
