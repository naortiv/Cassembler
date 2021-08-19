#ifndef struct_h

#define struct_h

#include "assembler.h"

/* data image and instructions image arrays */
extern unsigned char data_image[];
extern unsigned int instructions_image[];

/* boolean variable */
typedef enum { FALSE, TRUE } boolean;

/* we creat linked list for a label table */
typedef struct structLabels* labelPtr;
typedef struct structLabels {
	char name[MAX_LABEL];/* The name of the label */
	unsigned int address;/* The address of the label */
	char attributes[MAX_LABEL];/* The symbol type of the label */
	boolean external;/* sign if the label is extern */
	boolean command_line;/* sign if it's comannd label */
	boolean entry;/* sign if't entry label */
	labelPtr next;/* a pointer to the next label in the link list */
} Labels;

/*Creating a two-way linked list for extern labels */
typedef struct ext* extPtr;
typedef struct ext {
	char name[MAX_LABEL]; /* The name of the extern label */
	unsigned int address; /* The memory address for external address */
	extPtr next; /* A pointer to the next node extern in list */
	extPtr prev; /* A pointer to the previous node extern in list */
} ext;

#endif
