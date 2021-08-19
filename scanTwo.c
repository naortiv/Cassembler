#include "assembler.h"
#include "external_vars.h"
#include "scanFunctions.h"
#include "functions.h"


void scanTwo(FILE* fp, char* filename) {

	char line[MAX_INPUT];
	int line_number = 1;/* number of line for error */
	ic = 0; /* return ic to 0 , follow the additional word to set */

	while (fgets(line, MAX_INPUT, fp) != NULL) {
		error = EMPTY_ERROR;
		if (!ignore_line(line)) {/* Check if to ignore line */
			line_secend_scan(line);
		}
		
		if (if_error()) {
			error_exist = TRUE; /* sign if there is a error in scanTwo */
			print_error(line_number); 
		}
		line_number++; 
	}
	
	if (!error_exist) { /* No errors in scanTwo */
		creat_output_files(filename); /* creating files */
	}
	free_label_table(&symbols_table); /* free symbol table */
	free_ext(&ext_list); /* free external table */
}

/* check and read line by line */
void line_secend_scan(char* line) {
	
	int guidance_type, command_type;
	char current_word[MAX_INPUT];

	line = skip_spaces(line); /* if there is a white space-skip */
	if (end_of_line(line)) /* Checks line end */
		return;

	copy_word(current_word, line); /* Copy word by word */
	if (is_label(current_word, COLON)) {/* check if it's a label */
		line = next_word(line);
			copy_word(current_word, line); /* Copy word */
	}

	if ((guidance_type = find_guidance(current_word)) != NO_MATCH) /* if it's .entry guidance, process it */
	{
		line = next_word(line);
			if (guidance_type == ENTRY)
			{
				copy_word(current_word, line);
				entry_insert(symbols_table, current_word); /* if it's entry sign entry_exists TRUE */
			}
	}

	else if ((command_type = find_command(current_word)) != NO_MATCH) /* Encoding command's additional words if need */
	{
		char operand[20];  
		int address =0;
		line = next_word(line);
		if (command_label_check(command_type)==1)
		{
			line = next_list_word(operand, line);
			if(is_label(operand,FALSE)){
				address= get_label_address(symbols_table,operand);
				instructions_image[ic] |= (address & 0xffff);
				if(address == 0)
					add_ext(&ext_list,operand,ic*4+DEFAULT_IC);
			}
				
		}
		else if(command_label_check(command_type)==2){
			int i;
			for (i = 0; i < 5; i++) /* skip to third operand*/
			{
				line = next_list_word(operand, line);
			}
			if(is_label(operand,FALSE)){
				address= get_label_address(symbols_table,operand);
				instructions_image[ic] |= ((address - (ic*4 +DEFAULT_IC)) & 0xffff);
				if(address == 0)
					add_ext(&ext_list,operand,ic*4+DEFAULT_IC);
			}
		}
		ic++;
		}
}

/* Creating the output files */
int creat_output_files(char* name) {
	FILE* file;
	file = open_file(name, FILE_OBJECT);
	
	creat_object_file(file); /* Creating object file */

	if (entry_exists) {/* Creat entry file only if entry exists */
		file = open_file(name, FILE_ENTRY);
		creat_entry_file(file);
	}

	if (extern_exists) { /* Creating external file if extern exists */
		file = open_file(name, FILE_EXTERN);
		creat_external_file(file);
	}

	return EMPTY_ERROR; /* No errors found during the passes */
}

/* opens files */
FILE* open_file(char* filename, int type) {
	FILE* file;
	filename = create_file_name(filename, type); /* Creating filename with the correct extension */
	file = fopen(filename, "w"); /* Opening file*/
	if (file == NULL)
	{
		error = CANNOT_OPEN_FILE;
		return NULL;
	}
	return file;
}

/* Creating ob file */
void creat_object_file(FILE* fp) {
	unsigned int address = DEFAULT_IC;/*reset to the first ic memory*/
	int i;
	int byte1, byte2, byte3, byte4;
	fprintf(fp, "     %d %d\n", (ic)*4, dc); /* First line icf and dcf */
	for (i = 0; i < ic; address+=4, i++) /* instructions memory */
	{
		 
		byte1 = int_to_hex(instructions_image[i],0);
		byte2 = int_to_hex(instructions_image[i],1);
		byte3 = int_to_hex(instructions_image[i],2);
		byte4 = int_to_hex(instructions_image[i],3);
		fprintf(fp, "%.4d %.2X %.2X %.2X %.2X\n", address, byte1 ,byte2 ,byte3, byte4);
	}

	for (i = 0; i < dc; i++)
	{
		if (i%4==0)
			fprintf(fp, "%.4d ", address+i);
		fprintf(fp, "%.2X ", data_image[i]);
		if (i%4==3)
			fprintf(fp, "\n");
	}
	fclose(fp); 
}

/* Creating entry file */
void creat_entry_file(FILE* fp) {
	labelPtr label = symbols_table;
	while (label)
	{
		if (label->entry)
		{
			fprintf(fp, "%s %.4d\n", label->name, label->address);
		}
		label = label->next;
	}

	fclose(fp);
}

/* Creating extern file */
void creat_external_file(FILE* fp) {
	extPtr label = ext_list;
	int add= (label)->address;
	    do{
		fprintf(fp, "%s %.4d\n", label->name, label->address);
        label = label->next;
    } while (label->address!=add);

	fclose(fp); 
}

/* if need to check label */
int command_label_check(int type) {
	switch (type) {
	/* the commands that don't need process */
    case ADD:
    case SUB:
    case AND:
    case OR:
    case NOR:
	case ADDI:
    case SUBI:
    case ANDI:
    case ORI:
    case NORI:
	case LB:
    case SB:
    case LW:
    case SW:
    case LH:
    case SH:
	case STOP:
	case MOVE:
    case MVHI:
    case MVLO:
		return 0;
		break;

	/* J type */
	case JMP:
	case LA:
	case CALL:
		return 1;
		break;
	
	/* need to check these opcodes */
	case BEQ:
	case BNE:
	case BLT:
	case BGT:
		return 2;
	}
	return UNKNOWN_COMMAND;
}

