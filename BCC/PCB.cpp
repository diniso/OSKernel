/*
 * PCB.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: OS1
 */

#include "PCB.h"
#include"Thread.h"
#include <dos.h>
#include<stdio.h>
#include"SCHEDULE.h"
#include "Lists.h"

extern Thread_list all_thread;
extern Thread* running;
int IDEN = 0;

#define lock asm {pushf; \
		cli;}
#define unlock asm popf


PCB::PCB(Thread* nit , unsigned stackSize , unsigned timeSlice){
	pristigliSignali = new Queue();
	waiting = new List_wait_block();
	this->myThread = nit;
	this->stack_size = stackSize /2;
	if (stack_size < 100) this->stack_size = 100;
	if (stack_size > 30000) this->stack_size = 30000;
	this->time = timeSlice;
	this->id = IDEN++;

	this->finished = 0;
	this->status = 0;
	this->deleted = 0;

	this->stack = new unsigned[stack_size];
	this->stack[stack_size-1] = 0x200;
	this->stack[stack_size-2]= FP_SEG(PCB::wrapper);
	this->stack[stack_size-3]= FP_OFF(PCB::wrapper);
	// ax , bx , cx , dx , es , ds , si , di , bp
	this->ss = FP_SEG(stack + (stack_size-12));
	this->sp = FP_OFF(stack + (stack_size-12));
	stack[stack_size-12]= sp;

	if (running == 0) {
		parent = 0;
	}
	else {
		parent = Thread::getThreadById(Thread::getRunningId())->myPCB;
		for (int i = 0 ; i < 16;i++) {
			if (parent->myThread->myPCB->signali[i] != 0) signali[i] = parent->myThread->myPCB->signali[i]->copy();
			else signali[i] = 0;
		}
	}
}


PCB::~PCB() {
	lock
	delete [] this->stack;
	delete waiting;
	delete pristigliSignali;
	myThread = 0;
	parent = 0;
	for (int i = 0 ; i < 16;i++)
		delete signali[i];

	delete signali;
	unlock
}

void sistem_signal(Thread* nit , unsigned idSignala) {
	nit->myPCB->pristigliSignali->add(idSignala);
}


void PCB::wrapper() {
//	printf("Usao u wrapper fju\n");
	running->run() ;
	sistem_signal(running->myPCB->parent->myThread , 1);
	sistem_signal(running , 2);
	lock
	running->myPCB->finished = 1;
	running->myPCB->unblock();
//	printf("\n");
//	all_thread.pisi();
//	printf("\n");
	unlock
	dispatch();
}

void PCB::unblock() {
	this->waiting->isprazni();
}






