#include <stdio.h>
#include "../zad1/contactlib.h"
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
struct timespec startRealTime, endRealTime;
void printTime(struct rusage *endSysUsr, struct rusage *startSysUsr){
    printf("User time: %010ld ms, System Time: %010ld ms, Real Time: %010ld\n",
           (long) ( (endSysUsr->ru_utime.tv_usec - startSysUsr->ru_utime.tv_usec)),
           (long) ((endSysUsr->ru_stime.tv_usec - startSysUsr->ru_stime.tv_usec)),
           (endRealTime.tv_sec - startRealTime.tv_sec) * 1000000000 + (endRealTime.tv_nsec - startRealTime.tv_nsec)
    );
}

void beginMeasurement(struct rusage *startSysU){
    getrusage(RUSAGE_SELF, startSysU);
    clock_gettime(CLOCK_MONOTONIC_RAW, &startRealTime);

}
void finishMeasurement(struct rusage *endSysU){
    getrusage(RUSAGE_SELF, endSysU);
    clock_gettime(CLOCK_MONOTONIC_RAW, &endRealTime);
}
char * getRandomString(){
    char chars [] ={'a','b','c','d','e','f','g', 'h','i', 'j', 'k','l', 'm', 'n', 'o', 'p', 'r','s','t','q','u', 'v','w', 'x', 'y','z'};
    int length = rand()%10;
    char * result = malloc((size_t) length);
    for (int i = 0; i < length; ++i) {
        result[i] = chars[rand()%23];
    }
    return result;
}

birthdate getRandomBday(){
    birthdate bday;
    bday.day = rand()%28 + 1;
    bday.month = rand()%12 + 1;
    bday.year = rand()%100 + 1917;
    return bday;
}



int main() {
    contact contacts[1000];
    for (int i = 0; i < 1000; ++i) {
        contacts[i].adress = getRandomString();
        contacts[i].phonenumber = getRandomString();
        contacts[i].name = getRandomString();
        contacts[i].surname = getRandomString();
        contacts[i].email = getRandomString();

    }
    struct rusage * endSysU = malloc(sizeof(struct rusage));
    struct rusage * startSysU = malloc(sizeof(struct rusage));

    srand((unsigned int) time(NULL));
    printf("Start counting time\n\n");

    printf("Creating new list \n");
    struct list *list = createNewList();
    beginMeasurement(startSysU);
    for(int i = 0; i < 1000; i++){
        addToList(createNewNode(contacts[i]),list);
    }

    finishMeasurement(endSysU);
    printTime(endSysU, startSysU);

    printf("Creating new tree \n");
    tree tree1=createNewTree(surname);
    beginMeasurement(startSysU);
    for (int j = 0; j < 1000; ++j) {
        tree1 = addToTree(contacts[j],tree1);
    }
    finishMeasurement(endSysU);
    printTime(endSysU, startSysU);

    printf("Adding one contact to list \n");
    contact c1 = createContact("Adam", "Kowalski","adk@gmail.com", 1977,12,7,"585695","Bakery Str");
    beginMeasurement(startSysU);
    struct list * list1 = createNewList();
    addToList(createNewNode(c1),list1);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);
    printf("Adding one contact to tree \n");
    beginMeasurement(startSysU);
    tree tree2;
    tree2 = createNewTree(surname);
    addToTree(c1, tree2);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);
    printf("Removing one contact from list best case \n");
    beginMeasurement(startSysU);
    removeFromList(list,contacts[1]);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);

    printf("Removing one contact from list worst case \n");
    beginMeasurement(startSysU);
    removeFromList(list,contacts[999]);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);
    printf("Removing one contact from tree worst case \n");
    beginMeasurement(startSysU);
    deleteFromTree(tree1,contacts[888]);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);
    printf("Removing one contact from tree best case \n");
    beginMeasurement(startSysU);
    deleteFromTree(tree1,contacts[0]);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);
    printf("Finding one contact from list worst case \n");
    beginMeasurement(startSysU);
    findInList(list,contacts[999]);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);
    printf("Finding one contact from list best case \n");
    beginMeasurement(startSysU);
    findInList(list,contacts[0]);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);
    printf("Finding one contact from tree\n");
    beginMeasurement(startSysU);
    searchInTree(tree1,contacts[4]);
    finishMeasurement(endSysU);
    printTime(endSysU,startSysU);
    printf("Sorting list by name\n");

    beginMeasurement(startSysU);
    sortList(list,name);
    finishMeasurement(endSysU);
    printTime(endSysU, startSysU);

    beginMeasurement(startSysU);
    printf("Refactoring tree by email \n");
    changeComparationTree(tree1,email);
    finishMeasurement(endSysU);
    printTime(endSysU, startSysU);



    return 0;

}