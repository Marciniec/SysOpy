#ifndef SO1_CONTACTLIB_H
#define SO1_CONTACTLIB_H

typedef enum  {
    name, surname, email,adress,phonenumber,birth
} comparation;

typedef struct {
    int day, month, year;
}birthdate;

typedef struct {
    char * name;
    char * surname;
    char * email;
    char * adress;
    char * phonenumber;
    birthdate bday;
}contact;

struct node{
    contact contact;
    struct node * prev;
    struct node * next;
};
struct list{
    comparation cmp;
    struct node * head;
    struct node * tail;
};
struct BSTNode{
    struct BSTNode * parent;
    struct BSTNode * left;
    struct BSTNode * right;
    contact cnt;
};
typedef struct {
    struct BSTNode * root;
    comparation cmp;
}tree;


contact createContact(char * name, char* surname, char * email, int year, int month, int day, char* phonenumber, char *adress);
int compareBirthDate( birthdate bday1,  birthdate bday2);
int compareContacts(contact contact1, contact contact2);
int compare( contact cnt1,  contact cnt2, comparation cmp);
struct node * createNewNode(contact contact1);
struct list * createNewList();
void addToList(struct node * p, struct list * list1);
void removeFromList(struct list *list1, contact contact1);
void deleteList(struct list * list1);
struct node * findInList(struct list *list1, contact contact1);
struct node* merge(struct node *list1, struct node *list2, comparation cmp);
struct node *left_half(struct node *list);
struct node *right_half(struct node *list);
struct node*Merge_Sort(struct node *list, comparation cmp);
struct list * sortList(struct list *list1, comparation cmp);

tree createNewTree(comparation cmp);
void deleteBST(struct BSTNode* root);
void deleteTree(tree tree);
void addToBST(contact cnt,  struct BSTNode ** root, comparation cmp);
tree addToTree(contact cnt, tree tree);
struct BSTNode * findmin(struct BSTNode * root);
struct BSTNode * searchInBST(struct BSTNode * root, comparation cmp, contact cont);
struct BSTNode * searchInTree(tree tree, contact cnt);
struct BSTNode * deleteFromBST( struct BSTNode *root,contact data, comparation cmp);
tree deleteFromTree(tree tree, contact cnt);
void changeComparationBST(struct BSTNode *root, struct BSTNode **newTree, comparation cmp);
tree changeComparationTree(tree tree, comparation cmp);


#endif //SO1_CONTACTLIB_H
