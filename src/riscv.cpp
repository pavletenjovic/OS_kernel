//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/print.hpp"
#include "../h/sem.hpp"
#include"../test/printing.hpp"

_Buffer* Riscv::bufferInput = nullptr;
_Buffer* Riscv::bufferOutput = nullptr;
List2<TCB> Riscv::sleepingThreads;
bool Riscv::userMode = false;

void Riscv::popSppSpie()
{
    if (userMode) {
        mc_sstatus(Riscv::SSTATUS_SPP);
    }
    else {
        ms_sstatus(Riscv::SSTATUS_SPP);
    }
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {

        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        uint64 x;
        __asm__ volatile("ld t1, 80(fp)");
        __asm__ volatile("mv %0, t1" : "=r" (x));
        switch (x) {
            case THREAD_CREATE: {
                thread_t *handle = 0;
                TCB::Body start_routine;
                void *arg = nullptr;
                uint64 *sp = nullptr;
                __asm__ volatile("ld t1, 112(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (sp));
                __asm__ volatile("ld t1, 104(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (arg));
                __asm__ volatile("ld t1, 96(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (start_routine));
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (handle));
                TCB::createThread2(start_routine, handle, sp, arg);
                break;
            }

            case THREAD_DISPATCH: {
                TCB::dispatch();
                break;
            }
            case THREAD_EXIT:{
                TCB::running->setFinished(true);
                TCB::dispatch();
                break;
            }
            case THREAD_JOIN:{
                thread_t thread;
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (thread));
                TCB::running->setBlockedByThread(true);
                thread->mojiBlokirani.addLast(TCB::running);
                TCB::dispatch();
                break;
            }
            case THREAD_START:{
                thread_t thread;
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (thread));
                TCB::start(thread);
                break;
            }
            case SEM_OPEN:{
                sem_t* sem;
                unsigned int init;
                __asm__ volatile("ld t1, 96(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (init));
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (sem));
                Sem::createSemaphore(sem, init);
//                *sem = new Sem(init);
                break;
            }
            case SEM_WAIT:{
                sem_t sem;
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (sem));
                sem->semWait();
                break;
            }
            case SEM_SIGNAL:{
                sem_t sem;
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (sem));
                sem->semSignal();
                break;
            }
            case SEM_CLOSE:{
                sem_t sem;
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (sem));
                sem->semClose();
                break;
            }
            case PUTC:{
                char c;
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (c));
                bufferOutput->append(c);
                break;
            }
            case GETC: {
                char* ret;
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (ret));
                *ret = bufferInput->take();
                break;
            }
            case TIME_SLEEP: {
                uint64 t;
                __asm__ volatile("ld t1, 88(fp)");
                __asm__ volatile("mv %0, t1" : "=r" (t));
                TCB::running->threadAsleep = true;
//                TCB::running->setTimeSleep(t);
                Riscv::sleepingThreads.addToList(TCB::running, t);
                TCB::dispatch();
                break;
            }
//            default:{
//                TCB::dispatch();
//                break;
//            }


        }
        //printInteger(x);
        //TCB::dispatch();

        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        if(Riscv::sleepingThreads.peekFirst()){
             uint64 time = Riscv::sleepingThreads.getTimeSlice();
             if(time<=0){
                 TCB* thread = Riscv::sleepingThreads.removeFirst();
                 thread->threadAsleep = false;
                 Scheduler::put(thread);
             }
             else{
                 time -= 1;
                 Riscv::sleepingThreads.setTimeSlice(time);
             }
        }
        mc_sip(SIP_SSIP);
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
    }
    else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        int irq = plic_claim();
        while (*((char*)(CONSOLE_STATUS)) & CONSOLE_RX_STATUS_BIT) {

            char c = (*(char*)CONSOLE_RX_DATA);
            bufferInput->append(c);

        }
        plic_complete(irq);
        //console_handler();
    }
    else
    {
        // unexpected trap cause
    }
}