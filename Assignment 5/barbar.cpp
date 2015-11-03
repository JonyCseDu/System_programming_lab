//----------------------------------------------------------------------------
// BarberTest
//  sleeping barber problem
//----------------------------------------------------------------------------

/* only one customer can occupy the barber chair at a time */
Lock * barberchair = new Lock("barber chair");

/* the condition that the barber has a customer sitting in the barber chair -
   binary semaphore, so 1 = customer in chair and 0 = no customer in chair */
Semaphore * inchair = new Semaphore("customer in chair",0);

/* waiting thread is customer waiting for the barber to complete their
   haircut */
Semaphore * barber = new Semaphore("barber",0);

/* mutual exclusion for updating customer counts in waiting room */
Lock * waitingroom = new Lock("waiting room");

const int HAIRCUT_DELAY = 1000;
const int NUM_WAITING_CHAIRS = 5;

int numWaiting = 0;

void Barber ( void * ) {

    for ( ; true ; ) {
	/* wait until customer in chair */
	cout << kernel->currentThread->getName() << " waiting for customers"
	     << endl;
	inchair->P();

	/* cut hair */
	cout << kernel->currentThread->getName() << " cutting hair"
	     << endl;
	kernel->alarm->WaitUntil(HAIRCUT_DELAY);

	/* signal customer that haircut is done */
	cout << kernel->currentThread->getName() << " finished haircut"
	     << endl;
	barber->V();
    }

}

void Customer ( void * ) {

    /* if no room in waiting room, then leave */
    /* else wait until barber chair is available and leave when hair is
       cut */
    waitingroom->Acquire();
    if ( numWaiting >= NUM_WAITING_CHAIRS ) {
	cout << kernel->currentThread->getName()
	     << ": no room in waiting room; leaving without haircut" << endl;
	waitingroom->Release();
	return;
    } else {
	numWaiting++;
	waitingroom->Release();
    }

    /* attempt to occupy barber chair */
    cout << kernel->currentThread->getName() << " waiting in waiting room"
	 << endl;
    barberchair->Acquire();

    /* in barber chair, so no longer in waiting room */
    cout << kernel->currentThread->getName() << " in barber chair"
	 << endl;
    waitingroom->Acquire();
    numWaiting--;
    waitingroom->Release();
    cout << kernel->currentThread->getName() << " waking barber" << endl;
    inchair->V();      /* wake up barber as needed */

    /* wait for haircut to complete */
    cout << kernel->currentThread->getName()
	 << " waiting for haircut to finish" << endl;
    barber->P();

    /* all done - time to leave */
    cout << kernel->currentThread->getName() << " haircut complete; leaving"
	 << endl;
    barberchair->Release();
}

void BarberTest () {

    Thread * barber = new Thread("barber");
    barber->Fork((VoidFunctionPtr)Barber,NULL);

    /* create a bunch of customers */
    for ( int ctr = 0 ; ctr < 20 ; ctr++ ) {
	char * name = new char[12];
	sprintf(name,"customer %d",ctr);
	Thread * customer = new Thread(name);
	customer->Fork((VoidFunctionPtr)Customer,NULL);
	kernel->alarm->WaitUntil(rand()%500 + 100);
    }


}
