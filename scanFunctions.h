#ifndef scanFunctions_h

#define scanFunctions_h

#include "assembler.h"
#include "struct.h"

/*Functions that used in first and second passes*/

/***********scanOne fuctions***********/
void scanOne(FILE* fp);
void line_first_scan(char* line);
int handle_guidance(int guidance_type, char* line);
int handle_dtypes_guidance(char* line, int guidance_type);
int handle_asciz_guidance(char* line);
int handle_extern_guidance(char* line);
int handle_command(int type, char* line);
void add_num_to_data_image(int num,int guidance_type);
int find_operand_type(char* operand);
boolean command_accept_num_operands(int type, boolean first, boolean second, boolean third);
boolean command_accept_methods(int type, int first_method, int second_method, int third_method);
unsigned int build_machine_code(int type, int first_method, int second_method, int third_method, int first_register, int second_register, int third_register);
int num_words(int method);
int find_funct(int commands);
int find_opcode(int type);

/***********scanTwo fuctions***********/
void scanTwo(FILE* fp, char* filename);
void line_secend_scan(char* line);
int creat_output_files(char* name);
FILE* open_file(char* filename, int type);
void creat_object_file(FILE* fp);
void creat_entry_file(FILE* fp);
void creat_external_file(FILE* fp);
int command_label_check(int type);
int handle_command_two(int type, char* line);

#endif
