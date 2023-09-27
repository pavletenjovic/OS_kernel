//
// Created by os on 6/29/23.
//

#ifndef PROJECT_BASE_LIST2_HPP
#define PROJECT_BASE_LIST2_HPP

#include "../lib/hw.h"

template<typename T>
class List2
{
private:
    struct Elem
    {
        T *data;
        uint64 timeSlice;
        Elem *next;

        Elem(T *data, uint64 time, Elem *next) : data(data),timeSlice(time), next(next) {}
    };
    Elem *head, *tail;
    friend class Riscv;
public:
    List2() : head(0), tail(0) {}

    List2(const List2<T> &) = delete;

    List2<T> &operator=(const List2<T> &) = delete;

    void addFirst(T *data)
    {
        Elem *elem = new Elem(data, head);
        head = elem;
        if (!tail) { tail = head; }
    }
    Elem* getHead(){
        if(head) return head;
        else return 0;
    }
    void addLast(T *data)
    {
        Elem *elem = new Elem(data, 0);
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
    }
    void addToList(T* data, uint64 time){
        uint64 timeTemp = time;
        Elem* temp = head;
        if(temp){
            while(temp->timeSlice<timeTemp && temp->next!= nullptr){
                timeTemp -= temp->timeSlice;
                temp = temp->next;
            }
            if(temp->next==nullptr){
                Elem* elem = new Elem(data, timeTemp, 0);
                temp->next = elem;
                tail = elem;
            }
            else{
                Elem* temp2 = temp->next;
                Elem* elem = new Elem(data, timeTemp, 0);
                temp->next = elem;
                elem->next = temp2;
                while(temp2){
                    temp2->timeSlice -= timeTemp;
                    temp2 = temp2->next;
                }
            }
        }
        else{
            Elem* elem = new Elem(data, timeTemp, 0);
            head = elem;
            tail = elem;
        }
    }
    T *removeFirst()
    {
        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }


    T *removeLast()
    {
        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = elem->data;
        delete elem;
        return ret;
    }
    uint64 getTimeSlice(){
        return head->timeSlice;
    }
    void setTimeSlice(uint64 time){
        head->timeSlice = time;
    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }
};

#endif //PROJECT_BASE_LIST2_HPP
