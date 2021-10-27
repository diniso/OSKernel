/*
 * KernelSe.h
 *
 *  Created on: Apr 15, 2020
 *      Author: OS1
 */

#ifndef BCC_KERNELSE_H_
#define BCC_KERNELSE_H_

#include "Lists.h"
#include "Semaphor.h"

class KernelSem{
public:
	int val, fleg , id;
	List_wait_block* blocked;
	Semaphore* mySem;

	KernelSem(Semaphore* sem ,int val );
	~KernelSem() {delete blocked;}

	void dodaj_val(int val);
	void set_fleg(int n);

};



#endif /* BCC_KERNELSE_H_ */
