//
// Created by countess on 5/19/17.
//

#ifndef ZADANIE1_READERSWRITERS_H
#define ZADANIE1_READERSWRITERS_H
typedef enum {
    writer, reader
}human;
struct reader{
    int ID, divider;
};
struct writer {
    int ID;
};
struct node {
    int ID;
    human h;
    struct  node * next;
};
struct FIFO {
    struct node *head, *tail;
};
#endif //ZADANIE1_READERSWRITERS_H
