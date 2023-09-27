//
// Created by os on 6/22/23.
//
#include "syscall_c.hpp"


#ifndef PROJECT_BASE_SYSCALL_CPP_HPP
#define PROJECT_BASE_SYSCALL_CPP_HPP

class Thread {
public:
    Thread (void (*body)(void*), void* arg){
        thread_create(&myHandle, body, arg);
    }
    virtual ~Thread ();
    int start (){ thread_start(myHandle); return 0;}
    void join(){
        thread_join(this->myHandle);
    }
    static void threadWrapper(void *thread){((Thread*)thread)->run();}
    static void dispatch (){thread_dispatch();}
    static int sleep (time_t t){return time_sleep(t);}
protected:
    Thread (){ thread_create(&myHandle, Thread::threadWrapper, (void*)this);}
    virtual void run () {}
private:
    thread_t myHandle;
    void (*body)(void*); void* arg;
};


class Semaphore {
public:
    Semaphore (unsigned init = 1){
        sem_open(&myHandle, init);
    }
    virtual ~Semaphore (){ sem_close(myHandle);}
    int wait (){return sem_wait(myHandle);}
    int signal (){return sem_signal(myHandle);}
private:
    sem_t myHandle;
};


class PeriodicThread : public Thread {
public:
    void terminate (){term = false;}
protected:
    PeriodicThread (time_t period): Thread(), period(period){
        term = true;
    }
    virtual void periodicActivation () {}
    void run() override {
        while(term){
            periodicActivation();
            time_sleep(period);
        }
    }
private:
    time_t period;
    bool term;
};


class Console {
public:
    static char getc (){return ::getc();}
    static void putc (char a){ ::putc(a);}
};


#endif //PROJECT_BASE_SYSCALL_CPP_HPP
