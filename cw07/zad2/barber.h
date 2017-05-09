//
// Created by countess on 5/4/17.
//

#ifndef ZADANIE2_BARBER_H
#define ZADANIE2_BARBER_H
const char * MUTEX = "/mutex";
const char * BARBER = "/barber";
const char * CLIENT ="/client";
const char * SHARED = "shared";
const char  * CHAIRS = "chair";
const char * FIFO = "/fifo";

struct fifo_queue{
    int head;
    int tail;
    pid_t pids[1000];
};
#endif //ZADANIE2_BARBER_H
