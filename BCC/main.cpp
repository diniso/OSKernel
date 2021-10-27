/*
 * main.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: OS1
 */

#include <stdio.h>
#include "Thread.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include "Lists.h"
#include "Semaphor.h"
#include "KernelSe.h"
#include <dos.h>
#include "IVTEntry.h"

#define MAX 256
volatile Thread* running= 0;
Thread_list all_thread ;
Semaphore_list all_semaphore;
volatile Thread* pom = 0;
IVTEntry *all_entry[MAX];
int sistemskiSignali[16]= {0};

#define lock asm {pushf; \
		cli;}
#define unlock asm popf


// stara prekidna rutina
unsigned oldTimerOFF, oldTimerSEG;

// deklaracija nove prekidne rutine
void interrupt timer();
void signalId0();
void signalId1();
void signalId2();
void odradiSveSignale();


// postavlja novu prekidnu rutinu
void inic(){
	asm{
		cli
		push es
		push ax

		mov ax,0
		mov es,ax // es = 0

		// pamti staru rutinu
		mov ax, word ptr es:0022h
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		// postavlja novu rutinu
		mov word ptr es:0022h, seg timer
		mov word ptr es:0020h, offset timer

		// postavlja staru rutinu na int 60h
		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

// vraca staru prekidnu rutinu
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;

volatile PCB* iz = 0;

volatile int cntr = 0;
volatile int zabraniPromenu = 0;
volatile int context_switch_on_demand = 0;

// nova prekidna rutina tajmera
void interrupt timer(){
	if (!context_switch_on_demand && running->myPCB->time != 0) cntr--;
	if (((cntr == 0 && running->myPCB->time != 0)|| context_switch_on_demand)&& !zabraniPromenu ){
		asm {
			// cuva sp
			mov tsp, sp
			mov tss, ss
		}

		running->myPCB->sp = tsp;
		running->myPCB->ss = tss;

		if (!zabraniPromenu) {
				odradiSveSignale();
			}

		// scheduler
		if (!running->myPCB->finished && !running->myPCB->status && Thread::getRunningId()!= 1) Scheduler::put(running->myPCB);
		// izbrisi nit ako je zavrsila a pozvan je destruktor te niti
		if (running->myPCB->finished && running->myPCB->deleted) {
			all_thread.brisi(Thread::getRunningId());
			zabraniPromenu = 0;
		//	printf("Brisanje niti.\n");
		//	all_thread.pisi();
		//	printf("\n");
		}

		iz = Scheduler::get();
		//running= Scheduler::get()->myThread;
		if (iz != 0) running = iz->myThread;
		else running = 0;
		if (running == 0) {
	//		printf("Stavi na nit koja nista ne radi\n");
			running = pom;
		}

		tsp = running->myPCB->sp;
		tss = running->myPCB->ss;

		cntr = running->myPCB->time;

		asm {
			// restaurira sp
			mov sp, tsp
			mov ss, tss
		}
	}

	// poziv stare prekidne rutine
	// koja se nalazila na 08h, a sad je na 60h;
	// poziva se samo kada nije zahtevana promena konteksta
	// tako da se stara rutina poziva
	// samo kada je stvarno doslo do prekida
	if(!context_switch_on_demand) {
		asm int 60h;
		smanjiSemaforeInterruput();
	//	all_semaphore.pisi();

	}
//	printf("U dispatch id count:%d %d\n", running->getId() , cntr);
//	all_thread.pisi();
	context_switch_on_demand = 0;
	if (!zabraniPromenu) {
			odradiSveSignale();
		}

}

// sinhrona promena konteksta
void dispatch(){
	lock
	context_switch_on_demand = 1;
	timer();
	unlock
}

void do_something() {
	while (1);
}



int userMain (int argc, char* argv[]);

int main(){

	Thread* main = new Thread(5000 , 0);

	main->myPCB->signali[0] = new Signal_list();
	main->myPCB->signali[0]->add(signalId0);

	main->myPCB->signali[1] = new Signal_list();
	main->myPCB->signali[1]->add(signalId1);

	main->myPCB->signali[2] = new Signal_list();
	main->myPCB->signali[2]->add(signalId2);

	running = main;
	cntr = main->myPCB->time;

	pom = new Thread(4000 , 1);
	pom->myPCB->stack[pom->myPCB->stack_size - 2] = FP_SEG(do_something);
	pom->myPCB->stack[pom->myPCB->stack_size - 3] = FP_OFF(do_something);

	sistemskiSignali[1] = sistemskiSignali[2] = 1;

	inic();

	lock
	char* arg[4] = {"Ime programa", "150", "100", "0"};
	unlock
	userMain(4 , arg);



	lock
	restore();
	unlock


	pom->myPCB->finished = 1;
	pom->waitToComplete();

	return 0;
}



