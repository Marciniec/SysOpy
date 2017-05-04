//
// Created by countess on 5/4/17.
//

#ifndef ZADANIE1_SLEEEPINGBARBER_H
#define ZADANIE1_SLEEEPINGBARBER_H
#define CLIENTS 0
#define BARBER 1
#define MUTEX 2

union semnum {

    int val;

    struct semid_ds *buf;

    unsigned short *array;

};

#endif //ZADANIE1_SLEEEPINGBARBER_H
