/*
 * lists.cpp
 *
 *  Created on: Apr 14, 2020
 *      Author: OS1
 */
#include"Lists.h"
#include"PCB.h"
#include"SCHEDULE.h"
#include "Thread.h"
#include "KernelSe.h"
#include<stdio.h>

#define lock asm {pushf; \
		cli;}
#define unlock asm popf

extern Thread_list all_thread;
extern Semaphore_list all_semaphore;

Signal_list::blocked[16] = {0};

List_wait_block::~List_wait_block(){
	while (first != 0) {
		Node* tek = first;
		first = first->next;
		delete tek;
	}
	last = 0;
}


void List_wait_block::add(unsigned id , unsigned timeslice){
	lock
	if (first == 0) first = last = new Node(id , timeslice);
	else last = last->next = new Node(id, timeslice);
	all_thread.search(id)->myPCB->status = 1;
	unlock
}

void List_wait_block::obrisi(){
	if (first == 0) return ;
	Node* tek = first;
	first = first->next;
	all_thread.search(tek->id)->myPCB->status = 0;
	all_thread.search(tek->id)->myPCB->SemRet = 1;
	Scheduler::put(all_thread.search(tek->id)->myPCB);
	delete tek;
	if (first == 0) last = 0;
}

void List_wait_block::isprazni() {
	while (first != 0) {
		all_thread.search(first->id)->myPCB->status = 0;
		Scheduler::put(all_thread.search(first->id)->myPCB);
		Node* tek = first;
		first = first->next;
		delete tek;
	}
	last = 0;
}


int List_wait_block::smanji() {
	int uk = 0;
	Node* tek = first , *pret = first;
	while (tek != 0) {
		if (tek->time != 0) {
			tek->timeslice--;
			if (tek->timeslice == 0) {
				uk++;
				all_thread.search(tek->id)->myPCB->status = 0;
				all_thread.search(tek->id)->myPCB->SemRet = 0;
				Scheduler::put(all_thread.search(tek->id)->myPCB);
				if (first == tek) {
					first = first->next;
					if (last == tek) last = 0;
					delete tek;
					tek = first;
				}
				else {
					pret->next = tek->next;
					if (last == tek) last = pret;
					delete tek;
					tek = pret->next;
				}
			}else {
				pret = tek;
				tek = tek->next;
			}
		}
	else {
		pret = tek;
		tek = tek->next;
	}
}
	return uk;
}

int List_wait_block::izbaci_prvih(unsigned n) {
	int uk = 0;
	while (first != 0 && n >0) {
		n--;
		uk++;
		all_thread.search(first->id)->myPCB->status = 0;
		Scheduler::put(all_thread.search(first->id)->myPCB);
		Node* tek = first;
		first = first->next;
		delete tek;
	}
	if (first == 0) last = 0;
	return uk;
}

void List_wait_block::pisi() {
	lock
	Node* tek = first;
	while (tek  != 0) {
		printf("id, time , timeslice:%d %d %d\n", tek->id , tek->time , tek->timeslice);
		tek = tek->next;
	}
	unlock
}

void Thread_list::pisi(){
	lock
	Node* tek = first;
	while (tek!= 0) {
		printf("Lista sadrzi id , finish , status , deleted:%d %d %d %d\n", tek->info->getId() , tek->info->myPCB->finished , tek->info->myPCB->status , tek->info->myPCB->deleted);
		tek = tek->next;
	}
	unlock
}


Thread* Thread_list::search(unsigned i){
	Node* tek = first;
			while(tek!= 0) {
				if (tek->info->getId() == i) return tek->info;
				tek = tek->next;
			}
			return 0;
}

void Thread_list::brisi(unsigned i) {
		if (first == 0) return ;
		Node *tek = first, *prev = 0;
		while (tek != 0) {
			if (tek->info->getId() == i) {
				if (tek == first) {
					first = first->next;
					if (last == tek) {
						last = 0;
					}

					delete tek;
					return ;
				}
				prev->next = tek->next;
				if (tek == last) last = prev;
				delete tek;
				return ;
			}
			prev = tek;
			tek = tek->next;
		}
		return ;
	}

Thread_list::~Thread_list(){
	lock
	while (first != 0) {
		Node* tek = first;
		first = first->next;
//		tek->info = 0;
		delete tek;
	}
	last = 0;
	unlock
}

void Semaphore_list::pisi(){
	Node* tek = first;
		while (tek  != 0) {
			printf("semafor,id val:%d %d\n", tek->info->myImpl->id , tek->info->myImpl->val);
			tek = tek->next;
		}
}

Semaphore_list::~Semaphore_list(){
	lock
	while (first != 0) {
		Node* tek = first;
		first = first->next;
	//	tek->info = 0;
		delete tek;
	}
	last = 0;
	unlock
}


void Semaphore_list::brisi(int id) {
		if (first == 0) return ;
		Node *tek = first, *prev = 0;
		while (tek != 0) {
			if (tek->info->myImpl->id == id) {
				if (tek == first) {
					first = first->next;
					if (last == tek) {
						last = 0;
					}

					delete tek;
					return ;
				}
				prev->next = tek->next;
				if (tek == last) last = prev;
				delete tek;
				return ;
			}
			prev = tek;
			tek = tek->next;
		}
		return ;
	}


void Semaphore_list::pozoviWait(){
	Node* tek = all_semaphore.get_first();
	while (tek!= 0) {
		tek->info->myImpl->set_fleg(1);
		tek->info->wait(0);
		tek = tek->next;
	}
}


Signal_list::Signal_list() {
	first = last = 0;
	blockSignal = 0;
}

Signal_list::~Signal_list() {
	while (first != 0) {
		last = first;
		first = first->next;
		last->funkcija = 0;
		delete last;
	}
	last = 0;
}

void Signal_list::add(SignalHandler fun){
	if (first == 0) last = first = new Node(fun);
	else last = last->next = new Node(fun);
}

void Signal_list::pozoviSveFunkcije(){
	Node* tek = first;
	while (tek != 0) {
		tek->funkcija();
		tek = tek->next;
	}
}


void Signal_list::izbaci(){
	lock
		while (first != 0) {
				last = first;
				first = first->next;
				last->funkcija = 0;
				delete last;
			}
			last = 0;
	unlock
}


int Signal_list::zameni(SignalHandler s1 , SignalHandler s2){
	if (s1 == s2) return 0;
	Node* tek = first, *sig1 = 0, *sig2 = 0;
	while (tek != 0) {
		if (s1 == tek->funkcija) sig1 = tek;
		if (s2 == tek->funkcija) sig2 = tek;
		tek = tek->next;
	}
	if (s1 == 0 || s2 == 0) return 0;
	SignalHandler pom = sig1->funkcija;
	sig1->funkcija = sig2->funkcija;
	sig2->funkcija = pom;

	return 1;
}



Signal_list* Signal_list::copy(){
	Signal_list *ret = new Signal_list();
//	if (blocked) ret->blokiraj();
//	else
		ret->odblokiraj();
	Node* tek = first;
	while (tek != 0){
		ret->add(tek->funkcija);
		tek = tek->next;
	}
	return ret;
}


Queue::~Queue() {
		while (first != 0) {
			last = first;
			first = first->next;
			delete last;
		}
		last = 0;
	}

