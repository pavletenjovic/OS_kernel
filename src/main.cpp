//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/workers.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/_buffer.hpp"

extern void userMain();
thread_t a,b,cc;
void consoleWrite(void* t) {

    while(true){
        while((*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT)){
            char c = Riscv::bufferOutput->take();
            *((char*)CONSOLE_TX_DATA) = c;
        }
    }


}
bool flag = false;
void test(void*) {
    userMain();
    flag = true;
}


int main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::bufferInput = new _Buffer();
    Riscv::bufferOutput = new _Buffer();

    thread_t thread;
    thread_t consoleThread;
    thread_t test1;


    thread_create(&consoleThread, &consoleWrite, nullptr);
    thread_create(&test1, &test, nullptr);
    thread_create(&thread, nullptr, nullptr);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
//    Riscv::setMode(true);
//    __asm__ volatile ("ecall");
//    printString("UserMode Started \n");
//    userMode();

    while (!flag) {
        thread_dispatch();
    }

//    systemMode();




//    printString("UserMode Ended\n");
//    Riscv::setMode(false);
//    __asm__ volatile ("ecall");

    delete thread;
    delete consoleThread;
    delete test1;
//    delete handle;
//    delete threads[1];
//    for (auto &thread: threads)
//    {
//        delete thread;
//    }
    printString("Finished\n");

    return 0;
}
