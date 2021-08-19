#include "assembler.h"
#include "functions.h"
#include "external_vars.h"

/*struct functions*/

/*add the label to symbol tabel*/
labelPtr add_label(labelPtr* lptr, char* name, unsigned int address, boolean external, ...) {
	va_list p;
	labelPtr t = *lptr;
	labelPtr temp; /* varibal for the label , add it to symbol table */

	if (is_existing_label(*lptr, name))
	{
		error = LABEL_ALREADY_EXISTS;
		return NULL;
	}
	temp = (labelPtr)malloc(sizeof(Labels));
	if (!temp) /*problem in allocate memory to temp ,print an error massage and exit the program*/
	{
		error = MEMORY_ALLOCATION_FAILED;
		return NULL;
	}

	/* copy label info to temp */
	strcpy(temp->name, name);
	temp->entry = FALSE;
	temp->address = address;
	temp->external = external;

	if (!external) /* An external label cant be in opcodes*/
	{
		va_start(p, external);
		temp->command_line = va_arg(p, boolean);
	}
	else
	{
		extern_exists = TRUE;
	}

	/* If it empty list, temp will be the head */
	if (!(*lptr))
	{
		*lptr = temp;
		temp->next = NULL;
		return temp;
	}

	/* pointer to the list */
	while (t->next != NULL)
		t = t->next;
	temp->next = NULL;
	t->next = temp;

	va_end(p);
	return temp;
}

/* add adress for the label */
void offset_address(labelPtr l, int num, boolean is_data) {
	while (l)
	{
		if (!(l->external) && (is_data ^ (l->command_line)))
		{
			l->address += num;
		}
		l = l->next;
	}
}

/* if ther is existing entry sign it */
int entry_insert(labelPtr l, char* name) {
	labelPtr label = get_label(l, name);
	if (label != NULL)
	{
		if (label->external)
		{
			error = ENTRY_CANT_BE_EXTERN; /*add to error list*/
			return FALSE;
		}
		label->entry = TRUE;
		entry_exists = TRUE; /* if there is at least one entry in the program sign it */
		return TRUE;
	}
	else
		error = ENTRY_LABEL_DOES_NOT_EXIST;/*add to error list*/
	return FALSE;
}

/* gets the label address */
unsigned int get_label_address(labelPtr l, char* name) {
	labelPtr label = get_label(l, name);
	if (label != NULL) {
		return (label->address);
	}
	else return FALSE;
}

/* check if it is external label */
boolean is_external_label(labelPtr l, char* name) {
	labelPtr label = get_label(l, name);
	if (label != NULL) {
		return (label->external);
	}
	else return FALSE;
}

/* check if this label already exists */
boolean is_existing_label(labelPtr l, char* name) {
	
	return (get_label(l, name) != NULL);
}

/* gets the label */
labelPtr get_label(labelPtr l, char* name) {
	
	while (l) {
		if (strcmp(l->name, name) == 0)
			return l;
		l = l->next;
	}
	return NULL;
}

/* free the label list */
void free_label_table(labelPtr * lptr) {
		
	labelPtr temp;
		while (*lptr)
		{
			temp = *lptr;
			*lptr = (*lptr)->next;
			free(temp);
		}
}

/* if entered a wrong labal, delets in the label list */
int delete_label(labelPtr * lptr, char* name) {
	labelPtr temp = *lptr;
	labelPtr prevtemp;
	while (temp) {
		if (strcmp(temp->name, name) == 0) {
			if (strcmp(temp->name, (*lptr)->name) == 0) {
				*lptr = (*lptr)->next;
				free(temp);
			}
			else {
				prevtemp->next = temp->next;
				free(temp);
			}
			return 1;
		}
		prevtemp = temp;
		temp = temp->next;
	}
	return 0;
}

void print_table(labelPtr * lptr) {	
	labelPtr temp;
		while (*lptr)
		{
			temp = *lptr;
			printf("\n%d %s %s ext:%d\n",temp->address,temp->name,temp->attributes,temp->external);
			*lptr = (*lptr)->next;
		}
}
