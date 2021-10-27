/*
 * Thread.h
 *
 *  Created on: Apr 10, 2020
 *      Author: OS1
 */

#ifndef BCC_THREAD_H_
#define BCC_THREAD_H_

#include "PCB.h"

typedef unsigned  StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned  Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef int ID;
class PCB;

typedef void (*SignalHandler)();

typedef unsigned SignalId;


//Kernel's implementation of a user's thread
class Thread {
  public:

	void start(); // zavrseno
	void waitToComplete();
	virtual ~Thread(){} // zavrseno

	ID getId(); // zavrseno
	static ID getRunningId(); // zavrseno
	static Thread * getThreadById(ID id); // zavrseno


	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);




	Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice); // napravljeno
	PCB* myPCB;
 protected:
	friend class PCB;
//	Thread(unsigned Stack = defaultStackSize , Time defaultTimeSlice = 2); // napravljeno


	virtual void run() {} // test

 //	 private: 	PCB* myPCB;

};

void dispatch();



#endif /* BCC_THREAD_H_ */
