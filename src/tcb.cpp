//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../test/printing.hpp"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;
uint64 TCB::idINC = 0;
//TCB *TCB::createThread(Body body)
//{
//    return new TCB(body, TIME_SLICE);
//}
TCB *TCB::createThread2(TCB::Body body, thread_t* handle, uint64* sp, void* arg) {
    *handle = new TCB(body, TIME_SLICE, sp, arg);
    return *handle;
}

void TCB::yield()
{
    __asm__ volatile ("ecall");
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished() && !old->blocked && !old->blockedByThread && !old->threadAsleep) { Scheduler::put(old); }

    running = Scheduler::get();
    if (running == nullptr) return;
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    while(running->mojiBlokirani.peekFirst()){

        TCB* nit = running->mojiBlokirani.removeFirst();
        nit->setBlockedByThread(false);
        Scheduler::put(nit);
    }
    TCB::yield();
}

uint64 TCB::getId() const {
    return id;
}

void TCB::setId(uint64 id) {
    TCB::id = id;
}
TCB *TCB::getBlocked()
{
    return mojiBlokirani.removeFirst();
}



void TCB::setBlockedByThread(bool b) {
    blockedByThread = b;
}
