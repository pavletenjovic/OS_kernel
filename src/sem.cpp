//
// Created by os on 6/26/23.
//

#include "../h/sem.hpp"



Sem *Sem::createSemaphore(sem_t *handle, unsigned int init) {
    *handle = new Sem(init);
    return *handle;
}
uint64 Sem::semSignal() {
    if(++val<=0)
        deblock();
    return 0;
}
uint64 Sem::semWait() {
    if(--val<0)
        block();
    return 0;
}
void Sem::block(){
    listBlocked.addLast(TCB::running);
    TCB::running->blocked = true;
    TCB::dispatch();
}
void Sem::deblock() {
    TCB* temp = listBlocked.removeFirst();
    if(temp!=nullptr) {
        temp->blocked = false;
        Scheduler::put(temp);
    }
}
uint64 Sem::semClose() {
    while(listBlocked.peekFirst()){
        TCB* temp = listBlocked.removeFirst();
        temp->blocked = false;
        Scheduler::put(temp);
    }
    return 0;
}