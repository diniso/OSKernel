/*
 * KernelEv.h
 *
 *  Created on: May 4, 2020
 *      Author: OS1
 */

#ifndef BCC_KERNELEV_H_
#define BCC_KERNELEV_H_

#include "IVTEntry.h"

class KernelEv{

public:
	KernelEv(unsigned char ulaz);
	~KernelEv();

	void signal();
	void wait();

private :
	unsigned char brUlaza;
};



#endif /* BCC_KERNELEV_H_ */
