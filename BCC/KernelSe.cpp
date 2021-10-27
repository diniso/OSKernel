/*
 * KernelSe.cpp
 *
 *  Created on: Apr 15, 2020
 *      Author: OS1
 */

#include "KernelSe.h"

int uk=0;

KernelSem::KernelSem(Semaphore* sem ,int val):blocked() {
	blocked = new List_wait_block();
	this->val = val;
	mySem = sem;
	id = uk++;
	fleg = 0;
}


void KernelSem::set_fleg(int n) {
	fleg = n;
}

void KernelSem::dodaj_val(int val) {
	this->val += val;
}
