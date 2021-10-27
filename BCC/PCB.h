/*
 * PCB.h
 *
 *  Created on: Apr 10, 2020
 *      Author: OS1
 */

#ifndef BCC_PCB_H_
#define BCC_PCB_H_


#include "Thread.h"
#include "Lists.h"


//status : 0 - ready
//         1- blocked

class List_wait_block ;
class Signal_list;
class Queue;
class Thread ;

class PCB {
	friend class Thread;
	friend class List_wait_block;

public:

	List_wait_block* waiting;
	PCB* parent; // za signale
	Signal_list *signali[16];
	Queue *pristigliSignali;

	int SemRet;
	unsigned ss ,sp ;
	unsigned* stack ;
	unsigned stack_size , time ;
	unsigned id ;
	int finished , status , deleted;

	Thread *myThread;


	void unblock();
	PCB(Thread *nit , unsigned stackSize , unsigned timeSlice );
	~PCB();

	static void wrapper() ;

};



#endif /* BCC_PCB_H_ */
