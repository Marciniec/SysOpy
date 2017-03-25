#include  <stdlib.h>
#include  <stdio.h>
#include  <string.h>
#include  "contactlib.h"

contact createContact(char * name, char* surname, char * email, int year,  //creates new contact and returns it
                      int month, int day, char* phonenumber, char *adress){
    birthdate bday;
    bday.day = day;
    bday.month = month;
    bday.year = year;
    contact contact;
    contact.phonenumber = phonenumber;
    contact.bday =bday;
    contact.adress = adress;
    contact.email = email;
    contact.name = name;
    contact.surname = surname;
    return contact;
}
int compareBirthDate( birthdate bday1,  birthdate bday2){
    if (bday1.year == bday2.year && bday1.month == bday2.month && bday1.day == bday2.day) return 0;
    else if (bday1.year == bday2.year && bday1.month == bday2.month
             && bday1.day != bday2.day) return bday1.day - bday2.day;
    else if (bday1.year == bday2.year && bday1.month != bday2.month
             && bday1.day != bday2.day ) return  bday1.month - bday2.month;
    return  bday1.year - bday2.year;
}
int compareContacts(contact contact1, contact contact2){
    if(strcmp(contact1.surname ,contact2.surname) == 0
       && strcmp(contact1.name ,contact2.name) == 0
       && strcmp(contact1.adress ,contact2.adress) ==0
       && strcmp(contact1.email ,contact2.email)==0
       && contact1.phonenumber == contact2.phonenumber
       && compareBirthDate(contact1.bday,contact2.bday) ==0){
        return  0;
    }
    return 1;
}
int compare( contact cnt1,  contact cnt2, comparation cmp){
    int comparationresult;
    switch (cmp){
        case name:
            comparationresult = strcmp(cnt1.name, cnt2.name);
            break;
        case surname:
            comparationresult = strcmp(cnt1.surname, cnt2.surname);
            break;
        case email:
            comparationresult = strcmp(cnt1.email, cnt2.email);
            break;
        case adress:
            comparationresult = strcmp(cnt1.adress, cnt2.adress);
            break;
        case phonenumber:
            comparationresult = strcmp(cnt1.phonenumber, cnt2.phonenumber);
            break;
        case birth:
            comparationresult = compareBirthDate(cnt1.bday, cnt2.bday);
            break;
        default:
            break;
    }
    return  comparationresult;
}
/*          LIST                   */
struct node * createNewNode(contact contact1){ //creates new node and returns it
    struct node *p = malloc(sizeof(struct node));
    p->contact = contact1;
    p->next = NULL;
    p->prev = NULL;
    return  p;
}
struct list  * createNewList(){  //creates empty list
    struct list * list1 = malloc(sizeof(list1));
    list1->head = NULL;
    list1->head = NULL;
    return list1 ;
}
void addToList(struct node * p, struct list * list1){ //adds contact to the end of the list
    struct node * r;
    if (list1->head == NULL){
        list1->head = p;
        p -> prev = NULL;
        p -> next = NULL;
        list1->tail = p;
    }
    else {
        r = list1->head;
        while (r->next!=NULL) {
            r = r-> next;
        }
        r->next = p;
        p->prev = r;
        p->next = NULL;
        list1->tail = p;
    }
}

void removeFromList(struct list *list1, contact contact1){ //removes particular contact from list
    struct node * p;
    struct node * q;
    if(list1!=NULL && list1->head!=NULL && compareContacts(list1->head->contact,contact1)==0){
        p = list1->head;
        list1->head = list1->head->next;
        free(p);
    }

    p = list1->head->next;
    q = list1->head;
    while (p!=NULL && p->next != NULL && compareContacts(p->contact,contact1)!=0){
        p = p->next;
        q = q->next;
    }
    if(compareContacts(p->contact, contact1)==0){
        if(p->next!=NULL) p->next->prev = q;
        q->next =p->next;
        free(p);
    }
    return;
}
void deleteList(struct list * list1){ //deletes whole list
    struct node * p;
    struct node * q;
    q = list1->head;
    while (q!=NULL && q->next != NULL){
        p = q->next;
        free(q);
        q = p;
    }
    free(q);
    free(list1);
}
struct node * findInList(struct list * list1, contact contact1){ //returns pointer to the  contact in list
    struct node * p = list1->head->next;
    while  (p!=NULL && p->next!=NULL && compareContacts(p->contact, contact1)!=0){
        p=p->next;
    }
    return p;
}
struct node* merge(struct node *list1, struct node *list2, comparation cmp){

    if(list1==NULL && list2 !=NULL) return list2;
    else if(list1!=NULL && list2==NULL) return list1;
    else if(list1==NULL && list2==NULL) return NULL;
    struct node *list3=NULL;

    if (compare(list1->contact,list2->contact,cmp) > 0) {
        list3 = list2;
        list2 = list2->next;
        list3->next = NULL;
        struct node *p = list3;
        while (list1 != NULL && list2 != NULL) {
            if (compare(list1->contact,list2->contact,cmp)  < 0) {
                p->next = list1;
                list1->prev = p;
                list1 = list1->next;
                p = p->next;
                p->next = NULL;
            } else {
                p->next = list2;
                list2->prev = p;
                list2 = list2->next;
                p = p->next;
                p->next = NULL;
            }
        }
        if (list2 == NULL) {
            p->next = list1;
            p->next->prev = p;
        }
        else {
            p->next = list2;
            p->next->prev = p;
        }
    } else {
        list3 = list1;
        list1 = list1->next;
        list3->next = NULL;
        struct node *p = list3;
        while (list1 != NULL && list2 != NULL) {
            if (compare(list1->contact,list2->contact,cmp)  < 0) {
                p->next = list1;
                list1->prev = p;
                list1 = list1->next;
                p = p->next;
                p->next = NULL;
            } else {
                p->next = list2;
                list2->prev = p;
                list2 = list2->next;
                p = p->next;
                p->next = NULL;
            }

        }
        if (list2 == NULL) {
            p->next = list1;
            p->next ->prev = p;
        }
        else {
            p->next = list2;
            p ->next->prev = p;
        }
    }
    return list3;
}
struct node *left_half(struct node *list){  //rozlacz pierwsza polowe
    struct node *p=list;
    struct node *q=list;
    while(p->next->next!=NULL && p->next->next->next!=NULL){
        q=q->next;
        p=p->next->next;
    }
    q->next=NULL;
    return list;
}
struct node *right_half(struct node *list){ //rozlacz druga polowe
    struct node *p=list;
    struct node *q=list;
    while(p->next->next!=NULL && p->next->next->next!=NULL){
        q=q->next;
        p=p->next->next;
    }
    q=q->next;
    q->prev = NULL;
    return q;
}
struct node*Merge_Sort(struct node *list, comparation cmp){
    if(list==NULL) return NULL;
    if(list->next==NULL) return list;
    struct node *prawy =right_half(list);
    struct node *lewy =left_half(list);
    struct node *list1=Merge_Sort(lewy, cmp);
    struct node*list2=Merge_Sort(prawy, cmp);
    return merge(list1, list2, cmp);
}
struct list * sortList(struct list * list1, comparation cmp){
    list1->cmp = cmp;
    list1->head = Merge_Sort(list1->head,cmp);
    struct node  * p= list1->head;
    while (p!=NULL && p->next!=NULL){
        p=p->next;
    }
    list1->tail = p;
    return list1;
}

/*     TREE        */
tree createNewTree(comparation cmp){
    tree tree;
    tree.cmp = cmp;
    tree.root = NULL;
    return tree;
}
void deleteBST(struct BSTNode* root){
    if (root ==NULL) return;
    deleteBST(root->left);
    deleteBST(root->right);
    free(root);
}
void deleteTree(tree tree){
    deleteBST(tree.root);
}
void addToBST(contact cnt,  struct BSTNode ** root, comparation cmp){
    struct BSTNode * x = malloc(sizeof( struct BSTNode));
    x -> left =NULL;
    x->right = NULL;
    x ->cnt = cnt;
    x -> parent =NULL;
    struct BSTNode * p = *root;
    struct BSTNode * y = p;
    while(p!=NULL){
        y=p;
        if(compare(p->cnt, cnt, cmp)>0) p= p->left;
        else p=  p->right;
    }
    if(y==NULL) *root= x;
    else if(compare(y->cnt, cnt, cmp)>0) {
        y->left=x;
        x->parent=y;
    }
    else{
        y->right=x;
        x->parent=y;
    }
}
tree addToTree(contact cnt, tree tree){
    addToBST(cnt, &tree.root, tree.cmp);
    return tree;
}
struct BSTNode * searchInBST(struct BSTNode * root, comparation cmp, contact cont){
    while (root!=NULL && compareContacts(cont, root->cnt)){
        if(compare(root->cnt,cont,cmp)>0) root = root->left;
        else root=root->right;
    }
    return root;

}
struct BSTNode * searchInTree(tree tree, contact cnt){

    return searchInBST(tree.root, tree.cmp,cnt);

}
struct BSTNode * findmin(struct BSTNode * root){
    while (root->left!=NULL){
        root=root->left;
    }
    return root;
}
struct BSTNode * deleteFromBST( struct BSTNode *root,contact data, comparation cmp) {
    if (root == NULL) {
        return NULL;
    }
    if (compare(data,root->cnt, cmp)<0) {
        root->left = deleteFromBST(root->left, data, cmp);
    } else if (compare(data,root->cnt, cmp)>0) {
        root->right = deleteFromBST(root->right, data, cmp);
    } else {
        if (root->left == NULL && root->right == NULL) {
            free(root);
            root = NULL;
        }
        else if (root->left == NULL) {
            struct BSTNode *temp = root;
            root = root->right;
            free(temp);
        }

        else if (root->right == NULL) {
            struct BSTNode *temp = root;
            root = root->left;
            free (temp);
        }

        else {
            struct BSTNode *temp = findmin(root->right);
            root->cnt = temp->cnt;
            root->right = deleteFromBST(root->right, temp->cnt, cmp);
        }
    }
    return root;
}
tree deleteFromTree(tree tree, contact cnt){
    tree.root = deleteFromBST(tree.root,cnt,tree.cmp);
    return tree;
}
void changeComparationBST(struct BSTNode *root, struct BSTNode **newTree, comparation cmp){
    if (root ==NULL) return;
    changeComparationBST(root->left, newTree, cmp);
    addToBST(root->cnt, newTree, cmp);
    changeComparationBST(root->right, newTree, cmp);
}
tree changeComparationTree(tree tree, comparation cmp){
    tree.cmp = cmp;
    struct BSTNode * newTree=NULL;
    changeComparationBST(tree.root,&newTree,cmp);
    tree.root = newTree;
    return tree;
}
