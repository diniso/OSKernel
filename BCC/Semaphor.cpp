/*
 * Semaphor.cpp
 *
 *  Created on: Apr 15, 2020
 *      Author: OS1
 */
#include "Semaphor.h"
#include "KernelSe.h"
#include "Thread.h"
#include "Lists.h"
#include <stdio.h>

extern Thread* running;
extern Semaphore_list all_semaphore;

#define lock asm {pushf; \
		cli;}
#define unlock asm popf

Semaphore::Semaphore (int init) {
	lock
	myImpl = new KernelSem(this ,init);
	all_semaphore.add(this);
	unlock
}

int Semaphore::val () const {
	return myImpl->val;
}

Semaphore::~Semaphore (){
	lock
	all_semaphore.brisi(this->myImpl->id);
	unlock
}

int Semaphore::wait (Time maxTimeToWait){
	lock
	int ret ;
	if (myImpl->fleg) {
		myImpl->dodaj_val(myImpl->blocked->smanji());
		myImpl->set_fleg(0);
		ret = 1;
	}else {
	if (myImpl->val <= 0) {
		myImpl->blocked->add(running->getId(), maxTimeToWait);
		myImpl->dodaj_val(-1);
		dispatch();
		ret = Thread::getThreadById(Thread::getRunningId())->myPCB->SemRet;
		}else {
			myImpl->dodaj_val(-1);
			ret = 1;
		}
	}
	unlock
	return ret;
}

int Semaphore::signal(int n) {
	lock
	int ret;
	if (n < 0) {
		ret = n;
	}

	if (n == 0) {
		myImpl->blocked->obrisi();
		myImpl->dodaj_val(1);
		ret = 0;
	}
	else {
	ret = myImpl->blocked->izbaci_prvih(n);
	myImpl->dodaj_val(n);
	}

	unlock
	return ret;
}


void smanjiSemaforeInterruput(){
	Semaphore_list::pozoviWait();
}

