//
// Created by os on 6/29/23.
//

#include "../h/syscall_cpp.hpp"
#include "../h/riscv.hpp"

Thread::~Thread() {
    myHandle->setFinished(true);
    delete myHandle;
}