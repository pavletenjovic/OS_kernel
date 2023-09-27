//
// Created by os on 6/26/23.
//

#ifndef PROJECT_BASE_SEM_HPP
#define PROJECT_BASE_SEM_HPP
#include "syscall_c.hpp"
#include "tcb.hpp"
#include "list.hpp"
#include "../test/printing.hpp"
#include "../lib/hw.h"
#include "../lib/mem.h"
#include "../test/buffer.hpp"

class Sem
{
public:
    static Sem *createSemaphore(sem_t* handle, unsigned int init);


private:
    Sem(unsigned int init): val(init)
    {
    }
    List<TCB> listBlocked;
    int val;
    friend class Riscv;
    void block();
    void deblock();
    uint64 semSignal();
    uint64 semWait();
    uint64 semClose();
    friend class Buffer;
    friend class _Buffer;
};



#endif //PROJECT_BASE_SEM_HPP
