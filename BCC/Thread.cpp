/*
 * Thread.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: OS1
 */

#include"Thread.h"
#include"PCB.h"
#include"SCHEDULE.h"
#include <stdio.h>
#include "Lists.h"

extern Thread *running;
extern Thread_list all_thread;
extern int sistemskiSignali[16];

#define lock asm {pushf; \
		cli;}
#define unlock asm popf


Thread::Thread(StackSize stackSize , Time timeSlice){
	lock
	this->myPCB = new PCB(this , stackSize , timeSlice );
	all_thread.add(this);
//	printf("Konst id:%d \n", this->myPCB->id);
	unlock
}

ID Thread::getId() {
	return this->myPCB->id;
}

ID Thread::getRunningId(){
	return running->getId();
}

Thread * Thread::getThreadById(ID id){
	return all_thread.search(id);
}


void Thread::start(){
	Scheduler::put(this->myPCB);
}

void Thread::waitToComplete(){
	lock
	this->myPCB->deleted = 1;
	if (!this->myPCB->finished){
	this->myPCB->waiting->add(getRunningId());
	unlock
	dispatch();
	}
	else {
		all_thread.brisi(this->myPCB->id);
		unlock
	}
}

void Thread::signal(SignalId signal){
	lock
	if (sistemskiSignali[signal] == 1) {
		unlock
		return ;
	}
	myPCB->pristigliSignali->add(signal);
	unlock
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	lock
	if (myPCB->signali[signal] == 0) {
		myPCB->signali[signal] = new Signal_list();
	}
		myPCB->signali[signal]->add(handler);
	unlock
}

void Thread::unregisterAllHandlers(SignalId id) {
	lock
	if (myPCB->signali[id] == 0) {
		unlock
		return;
	}
		myPCB->signali[id]->izbaci();

		unlock
}


void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
	lock
	if (myPCB->signali[id] == 0) {
		unlock
		return;
	}
		myPCB->signali[id]->zameni(hand1 , hand2);

		unlock
}

void Thread::blockSignal(SignalId signal){
	lock
	if (myPCB->signali[signal] == 0) {
		unlock
		return;
	}
		myPCB->signali[signal]->blokiraj();

	unlock
}


void Thread::unblockSignal(SignalId signal){
	lock
	if (myPCB->signali[signal] == 0){
		unlock
		return;
	}
		myPCB->signali[signal]->odblokiraj();

	unlock
}

void Thread::blockSignalGlobally(SignalId signal){
	lock
	Signal_list::blocked[signal] = 1;
	unlock
}


void Thread::unblockSignalGlobally(SignalId signal){
	lock
	Signal_list::blocked[signal] = 0;
	unlock
}


void signalId0() {
	lock
	if (running == 0) return;
	running->myPCB->deleted = 1;
	running->myPCB->finished = 1;
	unlock
	dispatch();
}

void signalId1() { // prosledjuje se roditelju
	lock
		printf("Za roditeljsku nit.Ova nit je zavrsena.\n");
	unlock
}

void signalId2() { // prosledjuje se niti
	lock
	 printf("Trenutna nit je zavrsena.\n");
	unlock
}

volatile int i;
extern int zabraniPromenu;

void odradiSveSignale() {
	zabraniPromenu = 1;
	asm {
		pushf
		sti
	}
	i = 0;

	while (Thread::getThreadById(Thread::getRunningId())->myPCB->pristigliSignali->size() != i) {
		//	all_thread.pisi();
		//	printf("Running nit pre promene:%d\n" ,Thread::getRunningId());
		//	dispatch();
		//	printf("Running nit posle promene:%d\n", Thread::getRunningId());
			int idSignala = Thread::getThreadById(Thread::getRunningId())->myPCB->pristigliSignali->izbaci();
			if (Signal_list::blocked[idSignala] || Thread::getThreadById(Thread::getRunningId())->myPCB->signali[idSignala]->ifBlocked()) {
				Thread::getThreadById(Thread::getRunningId())->myPCB->pristigliSignali->add(idSignala);
				i++;
				continue;
			}
			if (Thread::getThreadById(Thread::getRunningId())->myPCB->signali[idSignala]!= 0)
				Thread::getThreadById(Thread::getRunningId())->myPCB->signali[idSignala]->pozoviSveFunkcije();
	}

	asm popf
	zabraniPromenu = 0;
}

