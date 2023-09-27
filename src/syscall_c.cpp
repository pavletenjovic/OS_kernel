//
// Created by os on 6/21/23.
//
#include "../h/syscall_c.hpp"
#include "../lib/mem.h"
#include "../h/konstante.hpp"

void* mem_alloc (size_t size){
    return __mem_alloc(size);
}
int mem_free (void* ptr){
    return __mem_free(ptr);
}

class TCB;
typedef TCB* thread_t;



int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg){
    //treba smestiti u registre kod operacije i argumente i onda poziv ecall
    void* sp = __mem_alloc(sizeof (uint64)*DEFAULT_STACK_SIZE);
//    sp = (uint64*)sp; //100
//    sp = 1;
//    sp = &sp - sizeof (uint64) ; // 99
    __asm__ volatile("mv a4, %0" : : "r" (sp));
    __asm__ volatile("mv a3, %0" : : "r" (arg));
    __asm__ volatile("mv a2, %0" : : "r" (start_routine));
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("mv a0, %0" : : "r" (THREAD_CREATE));
    __asm__ volatile("ecall");
    return 0;
}
int thread_exit (){
    __asm__ volatile("mv a0, %0" : : "r" (THREAD_EXIT));
    __asm__ volatile("ecall");
    return 0;
}
void thread_dispatch (){
    __asm__ volatile("mv a0, %0" : : "r" (THREAD_DISPATCH));
    __asm__ volatile("ecall");
}
void thread_join(thread_t handle){
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("mv a0, %0" : : "r" (THREAD_JOIN));
    __asm__ volatile("ecall");
}
void thread_start(thread_t handle){
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("mv a0, %0" : : "r" (THREAD_START));
    __asm__ volatile("ecall");
}

class Sem;
typedef Sem* sem_t;
int sem_open (sem_t* handle, unsigned init){
    __asm__ volatile("mv a2, %0" : : "r" (init));
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("mv a0, %0" : : "r" (SEM_OPEN));
    __asm__ volatile("ecall");
    return 0;
}
int sem_close (sem_t handle){
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("mv a0, %0" : : "r" (SEM_CLOSE));
    __asm__ volatile("ecall");
    return 0;
}
int sem_wait (sem_t id){
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("mv a0, %0" : : "r" (SEM_WAIT));
    __asm__ volatile("ecall");
    return 0;
}
int sem_signal (sem_t id){
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("mv a0, %0" : : "r" (SEM_SIGNAL));
    __asm__ volatile("ecall");
    return 0;
}

typedef unsigned long time_t;
int time_sleep (time_t t){
    __asm__ volatile("mv a1, %0" : : "r" (t));
    __asm__ volatile("mv a0, %0" : : "r" (TIME_SLEEP));
    __asm__ volatile("ecall");
    return 0;
}

// const int EOF = -1;
char getc (){
    char c = 's';
    char* ret = &c;
    __asm__ volatile("mv a1, %0" : : "r" (ret));
    __asm__ volatile("mv a0, %0" : : "r" (GETC));
    __asm__ volatile("ecall");
    return *ret;
}
void putc (char c){
    __asm__ volatile("mv a1, %0" : : "r" (c));
    __asm__ volatile("mv a0, %0" : : "r" (PUTC));
    __asm__ volatile("ecall");
}
