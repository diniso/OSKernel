/*
 * KernelEv.cpp
 *
 *  Created on: May 4, 2020
 *      Author: OS1
 */

#include "KernelEv.h"
#include "IVTEntry.h"
#include <stdio.h>

extern IVTEntry *all_entry[256];


void KernelEv::signal(){
	all_entry[brUlaza]->signal();
}

void KernelEv::wait(){
	all_entry[brUlaza]->wait();
}

KernelEv::KernelEv(unsigned char ulaz){
	brUlaza = ulaz;
	all_entry[brUlaza]->sem = new Semaphore(0);
	all_entry[brUlaza]->IDThread = Thread::getRunningId();

}

KernelEv::~KernelEv(){
}
