#include "assembler.h"
#include "struct.h"
#include "external_vars.h"

/*This file has all the functions that are responsible for linked lists of extern labels */

/* Add a node to end's list */
extPtr add_ext(extPtr* lptr, char* name, unsigned int reference) {

    extPtr t = *lptr;
    extPtr temp;

    temp = (extPtr)malloc(sizeof(ext));
    if (!temp)/*if there's problam to malloc a meory*/
    {
        printf("\nError, problam in allocate memory\n");
        exit(1);
    }

    temp->address = reference;
    strcpy(temp->name, name);

    if (!(*lptr)) /* If it's empty list*/
    {
        *lptr = temp;
        temp->next = temp;
        temp->prev = temp;
        return temp;
    }

    ((*lptr)->prev)->next = temp;
    temp->next = t;
    temp->prev = t->prev;
    (*lptr)->prev = temp;

    return temp;
}

void print_ext(extPtr* lptr) {	/*print ext linked list*/
    int add= (*lptr)->address;
    do{
        printf("\n %s %d\n",(*lptr)->name,(*lptr)->address);
        *lptr = (*lptr)->next;
    } while ((*lptr)->address!=add);
    
}


/* free the allocated memory of the list */
void free_ext(extPtr* lptr) {

    /* free each node in the list by going over it*/
    extPtr temp = *lptr;
    if (temp) {
        unsigned int last_reference = ((*lptr)->prev)->address;
        unsigned int reference = 0;
        do {
            temp = *lptr;
            reference = temp->address;
            *lptr = (*lptr)->next;
            free(temp);
        } while (reference != last_reference);
    }
}
