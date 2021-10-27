/*
 * Semaphor.h
 *
 *  Created on: Apr 15, 2020
 *      Author: OS1
 */

#ifndef BCC_SEMAPHOR_H_
#define BCC_SEMAPHOR_H_

typedef unsigned int Time;

class KernelSem;

class Semaphore {
	friend class KernelSem;
	public:

	Semaphore (int init=1);
	virtual ~Semaphore ();
	virtual int wait (Time maxTimeToWait);
	virtual int signal(int n=0);
	int val () const;  // Returns the current value of the semaphore


	KernelSem* myImpl;
	private:
	//KernelSem* myImpl;
};

void smanjiSemaforeInterruput();


#endif /* BCC_SEMAPHOR_H_ */
