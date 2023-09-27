//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
#include "syscall_c.hpp"
#include "list.hpp"
#include "syscall_cpp.hpp"
// Thread Control Block
class TCB
{
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }
    uint64 getTimeSleep() const { return timeSleep; }
    void setTimeSleep(uint64 n) { timeSleep = n; }
    using Body = void (*)(void*);
    static void start(thread_t handle){Scheduler::put(handle);}

    static TCB *createThread(Body body);
    void setBlockedByThread(bool b);
    static TCB *createThread2(Body body, thread_t* handle, uint64* sp, void* arg);

    static void yield();

    TCB *getBlocked();
    void putBlocked(TCB* t);
    static TCB *running;
    static uint64 idINC;
    uint64 id;
    List<TCB> mojiBlokirani;
private:
    TCB(Body body, uint64 timeSlice, uint64* sp, void* arg) :
            body(body),
            stack(body != nullptr ? sp : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            timeSlice(timeSlice),
            finished(false),
            arg(arg)
    {
        blocked = false;
        blockedByThread = false;
        threadAsleep = false;
        timeSleep = 0;
        if (body != nullptr && body != Thread::threadWrapper) { Scheduler::put(this); }
        if(body== nullptr){TCB::running = this;}
        id = idINC++;
    }


public:
    uint64 getId() const;

    void setId(uint64 id);

private:
    struct Context
    {
        uint64 ra;
        uint64 sp;
    };
    bool blocked;
    bool blockedByThread;
    bool threadAsleep;
    uint64 timeSleep;
    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    void* arg;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;
    friend class Sem;
    static uint64 constexpr STACK_SIZE = 1024;
    static uint64 constexpr TIME_SLICE = 2;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
