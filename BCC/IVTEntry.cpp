/*
 * IVTEntry.cpp
 *
 *  Created on: May 4, 2020
 *      Author: OS1
 */

#include "IVTEntry.h"
#include "Semaphor.h"
#include "Thread.h"
#include <dos.h>
#include <stdio.h>

extern IVTEntry *all_entry[256]  ;

//typedef void interrupt (*interupt)(...);

#define lock asm {pushf; \
		cli;}
#define unlock asm popf


void IVTEntry::pozovi(){
	lock
	if (pozoviPrethodnu == 1) {
		//printf("Pozvana je prethodna rutina.\n");
		prethodnaRutina();
		pozoviPrethodnu = 0;
	}
	else {
		pozoviPrethodnu++;
	}
	unlock
}

IVTEntry::IVTEntry(unsigned char ulaz , interupt prekid){
//	sem = new Semaphore(0);
	brUlaza = ulaz;
	callOld = 0;
	prethodnaRutina = getvect(brUlaza);
	all_entry[brUlaza] = this;
	setvect(brUlaza , prekid);
	pozoviPrethodnu= 0;

}


IVTEntry::~IVTEntry(){

	setvect(brUlaza , prethodnaRutina);
	delete sem;
}
