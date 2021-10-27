/*
 * event.cpp
 *
 *  Created on: May 4, 2020
 *      Author: OS1
 */

#include "event.h"
#include "KernelEv.h"

#define lock asm {pushf; \
		cli;}
#define unlock asm popf

Event::Event (IVTNo ivtNo){
	lock
	myImpl = new KernelEv(ivtNo);
	unlock
}

Event::~Event (){
	lock
	delete myImpl;
	unlock
}

void Event::wait  (){
	myImpl->wait();
}

void Event::signal(){
	myImpl->signal();
}




