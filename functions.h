#ifndef functions_H

#define functions_H

#include "assembler.h"
#include "struct.h"

/*Declarations of functions found in type files '.c'*/

/**functions.c functions**/ 

/*This functions creat the file name*/
char* create_file_name(char* original, int type);

int ignore_line(char* line);

int if_error();

char* skip_spaces(char* ch);

int end_of_line(char* line);

void copy_word(char* destination, char* line);

unsigned int extract_bits(unsigned int word, int start, int end);

unsigned int int_to_hex(unsigned int word,int i);

boolean is_label(char* sign, int colon);

char* next_word(char* seq);

boolean is_register(char* sign);

int find_reg_number(char* sign);

int find_index(char* sign, const char* arr[], int n);

int find_guidance(char* sign);

int find_command(char* sign);

char* next_list_word(char* dest, char* line);

boolean is_number(char* seq);

char* next_word_string(char* dest, char* line);

boolean is_string(char* string);

void add_string_to_data_image(char* str);

void add_to_instructions_image(unsigned int word);

void print_error(int line_number);

/* Functions from struct.c */
labelPtr add_label(labelPtr* lptr, char* name, unsigned int address, boolean external, ...);
void offset_address(labelPtr l, int num, boolean is_data);
int entry_insert(labelPtr l, char* name);
unsigned int get_label_address(labelPtr l, char* name);
boolean is_external_label(labelPtr l, char* name);
boolean is_existing_label(labelPtr l, char* name);
labelPtr get_label(labelPtr l, char* name);
void free_label_table(labelPtr* lptr);
int delete_label(labelPtr* lptr, char* name);

/* Functions from externalStructs.c */
extPtr add_ext(extPtr* lptr, char* name, unsigned int reference);
void free_ext(extPtr* lptr);

#endif
