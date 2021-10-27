/*
 * event.h
 *
 *  Created on: May 4, 2020
 *      Author: OS1
 */

#ifndef BCC_EVENT_H_
#define BCC_EVENT_H_

#include "KernelEv.h"

typedef unsigned char IVTNo;
class KernelEv;

class Event {
	public:

	Event (IVTNo ivtNo);
	~Event ();
	void wait  ();

	protected:

	friend class KernelEv;
	void signal();// can call KernelEv

	private:
		KernelEv* myImpl;
};



#endif /* BCC_EVENT_H_ */
