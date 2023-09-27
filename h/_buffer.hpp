//
// Created by os on 6/27/23.
//

#ifndef PROJECT_BASE__BUFFER_HPP
#define PROJECT_BASE__BUFFER_HPP

#include "../h/sem.hpp"
//#include "../h/riscv.hpp"

class _Buffer{

private:

    static const int buffSize = 256;
    char buffer [buffSize];
    friend class Riscv;
    sem_t itemAvailable;
    sem_t spaceAvailable;
    int head, tail, count;
public:
    _Buffer():head(0),tail(0),count(0){
        sem_open(&itemAvailable, 0);
        sem_open(&spaceAvailable, 256);
    }
    void append(char c);
    char take();
};


#endif //PROJECT_BASE__BUFFER_HPP
