//
// Created by os on 6/27/23.
//
#include "../h/_buffer.hpp"

void _Buffer::append(char c){
    if(count<buffSize){
        spaceAvailable->semWait();
        buffer[tail]=c;
        tail = (tail+1)%buffSize;
        count++;
        itemAvailable->semSignal();
    }

}

char _Buffer::take() {
    itemAvailable->semWait();
    char c = buffer[head];
    head = (head+1)%buffSize;
    count--;
    spaceAvailable->semSignal();
    return c;
}