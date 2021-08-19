#ifndef functions_H

#define functions_H

#include "assembler.h"
#include "struct.h"

/*Declarations of functions found in type files '.c'*/

/**functions.c functions**/ 

/*This functions creat the file name*/
char* create_file_name(char* original, int type);

int ignore_line(char* line);/*skip to the next line*/

int if_error();/*check if have a error*/

char* skip_spaces(char* ch);/*skip white space*/

int end_of_line(char* line);/*check if it's end of line*/

void copy_word(char* destination, char* line);/*copy one word*/

unsigned int extract_bits(unsigned int word, int start, int end);/*this function check bit by bit*/ 

unsigned int int_to_hex(unsigned int word,int i);/*divite int to hex*/

boolean is_label(char* sign, int colon);/*check if it's label word*/

char* next_word(char* seq);/*skip to the next word*/

boolean is_register(char* sign);/*check if the word is a register*/

int find_reg_number(char* sign);/*Check if the number of the register is in range*/

int find_index(char* sign, const char* arr[], int n);

int find_guidance(char* sign);/*match the guidance to guidance's list*/

int find_command(char* sign);/*match the command to command's list*/

char* next_list_word(char* dest, char* line);/*get the word to dest array*/

boolean is_number(char* seq);/*check if it's a number*/

char* next_word_string(char* dest, char* line);

boolean is_string(char* string);/*check if it's string*/

void add_string_to_data_image(char* str);

void add_to_instructions_image(unsigned int word);

void print_error(int line_number);

/* Functions from struct.c */
labelPtr add_label(labelPtr* lptr, char* name, unsigned int address, boolean external, ...);/*add label to the symbol table*/
void offset_address(labelPtr l, int num, boolean is_data);
int entry_insert(labelPtr l, char* name);/*if is entry guidance insert with 'entry' sign*/
unsigned int get_label_address(labelPtr l, char* name);
boolean is_external_label(labelPtr l, char* name);/*if is extern guidance insert with 'extern' sign*/
boolean is_existing_label(labelPtr l, char* name);/*check if it's an existing label*/
labelPtr get_label(labelPtr l, char* name);
void free_label_table(labelPtr* lptr);
int delete_label(labelPtr* lptr, char* name);

/* Functions from externalStructs.c */
extPtr add_ext(extPtr* lptr, char* name, unsigned int reference);/*add extern to the file.ext*/
void free_ext(extPtr* lptr);

#endif
