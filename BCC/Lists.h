/*
 * Lists.h
 *
 *  Created on: Apr 14, 2020
 *      Author: OS1
 */

#ifndef BCC_LISTS_H_
#define BCC_LISTS_H_

#include "Thread.h"
#include "PCB.h"
#include "Semaphor.h"

class Thread;
typedef void (*SignalHandler)();
class List_wait_block{
	struct Node{
		Node* next ;
		unsigned id , timeslice , time;

	Node(unsigned _id , unsigned _time = 0, Node* = 0) {id = _id ; time = timeslice = _time ; next = 0;}
	};
	Node *first , *last;

public:
	List_wait_block() {first = last = 0;}
	~List_wait_block();

	void add(unsigned id , unsigned timeslice = 0); // dodaje 1 na kraj
	void obrisi(); // brise prvu nit
	void isprazni(); // brise sve niti , za waiting


	int smanji(); // smanjuje time i brise sve koje su dosli na 0 a moze da se odblokira zbog vremena i vraca koliko ih je izbaceno
	int izbaci_prvih(unsigned n); // izbaciju max(sve ili n) i vraca koliko ih je izbaceno
	void pisi();

};

class Thread_list {

	struct Node{
		Thread* info ;
		Node* next;
	Node(Thread* inf ) {info = inf ; next = 0;}
	};
public:
	Node *first , *last;

	Thread_list() {first = last = 0;}
	~Thread_list();
	void add(Thread* info) {
		if (first == 0) first = last = new Node(info);
		else last = last->next = new Node(info);
	}

	void brisi(unsigned i); // brise nit iz liste svih niti i kontekst te niti

	Thread* get_first() {
		if (first != 0) return first->info;
		else return 0;
	}

	void pisi();

	Thread* search(unsigned i); //  nit

	int empty() { return first == 0;}


};

class Semaphore_list {

	struct Node{
		Semaphore* info ;
		Node* next;
	Node(Semaphore* inf ) {info = inf ; next = 0;}
	};
public:
	Node *first , *last;

	Semaphore_list() {first = last = 0;}
	~Semaphore_list();
	void add(Semaphore* info) {
		if (first == 0) first = last = new Node(info);
		else last = last->next = new Node(info);
	}

	Node* get_first() {
		return first;
	}

	void pisi();

	int empty() { return first == 0;}

	void brisi(int id); // brise iz liste svih i pojedinacni semafor sa zadatim id

	static void pozoviWait();


};

class Signal_list{
	struct Node {
		Node* next;
		SignalHandler funkcija;

		Node(SignalHandler fun ) {
			funkcija = fun;
			next = 0;
		}
	};

	int blockSignal;

public:
	static int blocked[16];
	Node* first , *last;

	Signal_list() ;
	~Signal_list();

	void add(SignalHandler fun); // za registterHendler
	void izbaci();// za unregiterAll, izbacuje sve iz liste
	void pozoviSveFunkcije();
	int zameni(SignalHandler s1 , SignalHandler s2);
	void blokiraj() {blockSignal = 1;}
	void odblokiraj() {blockSignal = 0;}
	int ifBlocked() {return blockSignal;}

	Signal_list* copy();
};


class Queue {
	struct Node {
		Node* next;
		int info;

		Node (int in) {
			info = in;
			next = 0;
		}
	};

	Node* first , *last;
	int ukupno;

public:
	Queue() {
		first = last = 0;
		ukupno = 0;
	}
	~Queue();

	void add(int val) {
		if (first == 0) last = first = new Node(val);
		else last = last->next = new Node(val);
		ukupno++;
	}

	int izbaci() {
		int ret = first->info;
		Node *tek = first;
		first = first->next;
		if (first == 0) last = 0;
		ukupno--;

		delete tek;
		return ret;
	}

	int top(){
		return first->info;
	}

	int isEmpty() {
		return ukupno == 0;
	}

	int size() {
		return ukupno;
	}
};


#endif /* BCC_LISTS_H_ */
