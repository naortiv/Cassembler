#include "assembler.h"
#include "external_vars.h"
#include "scanFunctions.h"
#include "functions.h"


void scanOne(FILE* fp) {

    char line[MAX_INPUT]; /* Array for current line*/
    int line_number = 1;/* the first line is 1*/
    ic = 0;
    dc = 0;
    
    while (fgets(line, MAX_INPUT, fp) != NULL) /* Read line by line until end of file */
    {
        error = EMPTY_ERROR; /* If it's new line- reset the error variable*/
        if (!ignore_line(line)) /* If it's blank line- skip*/
            line_first_scan(line);
        if (if_error()) {
            error_exist = TRUE; /* There was at least one error through all the program */
            print_error(line_number); /* Print the error */
        }
        line_number++;
    }
    offset_address(symbols_table,ic*4+DEFAULT_IC,TRUE);
  } 

/*This function reads each line and checks it */
void  line_first_scan(char* line) {

    /* Variables for the type of the guidance/command */
    int guidance_type = UNKNOWN_TYPE;
    int command_type = UNKNOWN_COMMAND;
    boolean label = FALSE; /* If there is a labal in this line, this variable will be TRUE */
    labelPtr label_node = NULL; /* This variable holds optional label in case we need to create it */
    char current_word[MAX_INPUT]; /* This array storage the current word if it exsiting */

    /*process line */
    line = skip_spaces(line);/* Skip white space */
    if (end_of_line(line)) return;/* if it's an empty line- skip it */

    if ((isalpha(*line) == 0) && *line != '.') { /* First character must be a letter or a dot */
        error = INVALID_SYNTAX;
        return;
    }
    copy_word(current_word, line); /* copy label */

    if (is_label(current_word, COLON))/* check if the first word is a label */
    {
        label = TRUE;

        label_node = add_label(&symbols_table, current_word, DEFAULT_IC, FALSE, FALSE);/* if it's an new label, add to the Symbol table */
        if (label_node == NULL){/* if there is a problem to add the label */
            return;
        }
        line = next_word(line);/* get the next word */
        if (end_of_line(line))
        {
            error = INVALID_LABEL_LINE;/* if the line include only label */
            return;
        }
        copy_word(current_word, line);/* save the label and continue */
    }

    if (if_error())
        return; /* if the search for this label failed */

    if ((guidance_type = find_guidance(current_word)) != NO_MATCH) /* if this word is a guidance */
    {
        if (label)
        {
            if (guidance_type == ENTRY || guidance_type == EXTERN) { /* If it is an extern or entry guidancece, delet it from label table */
                delete_label(&symbols_table, label_node->name);
                label = FALSE;
            }
            else {
                /* Setting fields label by the type*/
                strcpy(label_node->attributes, "data");
            }
            label_node->address = dc; /* data_image address label is dc */ 
        }
        line = next_word(line);
        handle_guidance(guidance_type, line); /* This function process all types of guidances */
    }

    else if ((command_type = find_command(current_word)) != NO_MATCH) /*if it's a command word */
    {        
        if (label != 0)
        {
            /* Setting fields label by types */
            label_node->command_line = TRUE;
            strcpy(label_node->attributes, "code");
            label_node->address = ic*4+DEFAULT_IC;/* data_image label address vis ic */
        }
        line = next_word(line);
        handle_command(command_type, line);/* This function process all types of commands */

    }
    else
        error = MISSING_SYNTAX;/* if there isn't a command or a guidance in this line*/  
}

/* This function get the guidance and send it to the function of it  */
int handle_guidance(int guidance_type, char* line) {

    if (line == NULL || end_of_line(line)) /* Must be a parameter after the  guidance */
    {
        error = NO_PARAMETER_AVAILABLE;
        return ERROR;
    }

    switch (guidance_type)
    {
    case DH:
    case DW:
    case DB:
        /* Handle .dh/.dw/db */
        return handle_dtypes_guidance(line, guidance_type);

    case ASCIZ:
        /* Handle .asciz guidance*/
        return handle_asciz_guidance(line);

    case ENTRY:
        /* Check syntax of entry (can contain only one parameter) */
        if (!end_of_line(next_word(line))) /* If there's more than one word */
        {
            error = GUIDANCE_INVALID_NUM_PARAMS;
            return ERROR;
        }
        break;

    case EXTERN:
        /* Handle .extern guidance */
        return handle_extern_guidance(line);
    }
    return EMPTY_ERROR;
}

/* This function d types guidance and encodes it to data_image */
int handle_dtypes_guidance(char* line,int guidance_type)
{
    char word[MAX_INPUT]; /* save word */
    /* flage for check if there is a comma between the numbers */
    boolean number = FALSE;
    boolean comma = FALSE;

    while (!end_of_line(line)) /* checks if it the end of the line */
    {
        line = next_list_word(word, line); /* get word */

        if (strlen(word) > 0) /* if isn't empty word */
        {
            if (number == FALSE) { /* if is the first word */
                signed long parameter= atoi(word);
                signed long limit = pow( 2 , guidance_type*8 -1)-1;/*the limit of i bytes*/
                if (is_number(word) == FALSE) { /* check if the word is number */
                    error = DATA_EXPECTED_NUM;
                    return ERROR;
                }
                if (parameter<limit && parameter>=-limit)
                {
                    add_num_to_data_image(parameter,guidance_type);
                    number = TRUE; /* if it's valid number */
                    comma = FALSE; /* comma stay FALSE because is the firs number */
                }
                else {
                    error =GUIDANCE_INVALID_NUM_PARAMS;
                    return ERROR;
                }
            }

            else if (*word != ',') /* if there number/s before */
            {
                error = DATA_EXPECTED_COMMA_AFTER_NUM;
                return ERROR;
            }

            else /* check if have only one comma , comma must come FALSE */
            {
                if (comma == TRUE) {
                    error = DATA_COMMAS_IN_A_ROW;
                    return ERROR;
                }
                else {
                    comma = TRUE;
                    number = FALSE;
                }
            }

        }
    }
    if (comma == TRUE)
    {
        error = DATA_UNEXPECTED_COMMA;
        return ERROR;
    }
    return EMPTY_ERROR;
}

/* process .asciz guidance and encoding it to data_image */
int handle_asciz_guidance(char* line)
{
    char word[MAX_INPUT];
    line = next_word_string(word, line);
    if (!end_of_line(word) && is_string(word)) { /* check if there is a word and it a string */
        line = skip_spaces(line);
        if (end_of_line(line)) /* end of line */
        {
            /* remove a quote mark and encoding it to data_image */
            word[strlen(word) - 1] = '\0';
            add_string_to_data_image(word + 1);
        }

        else /* if have word after the string */
        {
            error = STRING_TOO_MANY_OPERANDS;
            return ERROR;
        }
    }

    else /* Invalid string */
    {
        error = STRING_OPERAND_NOT_VALID;
        return ERROR;
    }
    return EMPTY_ERROR;
}

/* This function process an .extern guidance */
int handle_extern_guidance(char* line) {
    char word[MAX_LABEL]; /* get the label */

    copy_word(word, line); /* get word */
    if (end_of_line(word)) /* empty word , have no label */
    {
        error = EXTERN_NO_LABEL;
        return ERROR;
    }

    if (!is_label(word, FALSE)) /* check if it's a label (without a colon) */
    {
        error = EXTERN_INVALID_LABEL;
        return ERROR;
    }

    line = next_word(line);
    if (!end_of_line(line))
    {
        error = EXTERN_TOO_MANY_OPERANDS;
        return ERROR;
    }

    /* Try add the label to symbols table */
    if (add_label(&symbols_table, word, 0, TRUE) == NULL)
        return ERROR;
    return if_error(); /* error in is_label() */
}

/* This function process commands and encodes it to words */
int handle_command(int type, char* line) {

    boolean is_first = FALSE,is_second = FALSE,is_third = FALSE; /* These booleans will tell which of the operands were received*/
    char first_operand[20], second_operand[20], third_operand[20]; /* These strings will hold the operands */
    int first_operand_type = 0, second_operand_type = 0, third_operand_type = 0; /* These will hold the addressing methods of the operands */
    int first_value = 0, second_value = 0, third_value = 0; /* These will hold the register numbers */

    /* Trying to parse operands */
    line = next_list_word(first_operand, line);
    if (!end_of_line(first_operand)) /* If first operand is not empty */
    {
        is_first = TRUE; /* First operand exists! */
        line = next_list_word(second_operand, line);
        if (!end_of_line(second_operand)) /* If second operand (should hold temporarily a comma) is not empty */
        {
            if (second_operand[0] != ',') /* A comma must separate two operands of a command */
            {
                error = COMMAND_UNEXPECTED_CHAR;
                return ERROR;
            }
            else
            {
                line = next_list_word(second_operand, line);
                if (end_of_line(second_operand)) /* If second operand is not empty */
                {
                    error = COMMAND_UNEXPECTED_CHAR;
                    return ERROR;
                }
                is_second = TRUE; /* Second operand exists! */
                line = next_list_word(third_operand, line);
                if (!end_of_line(third_operand)) /* If second operand (should hold temporarily a comma) is not empty */
                {
                    if (third_operand[0] != ',') /* A comma must separate operands of a command */
                    {
                        error = COMMAND_UNEXPECTED_CHAR;
                        return ERROR;
                    }
                    else
                    {
                        line = next_list_word(third_operand, line);
                        if (end_of_line(third_operand)) /* If Third operand is not empty */
                        {
                            error = COMMAND_UNEXPECTED_CHAR;
                            return ERROR;
                        }
                        is_third = TRUE; /* Third operand exists! */
                    }
                }
            }
        }
    }

    line = skip_spaces(line);
    if (!end_of_line(line)) /* If the line continues more then 3 operands */
    {
        error = COMMAND_TOO_MANY_OPERANDS;
        return ERROR;
    }

    if ((is_first)) {
        first_operand_type = find_operand_type(first_operand); /* Detect addressing method of first operand */
        if (first_operand_type == METHOD_REGISTER)
            first_value = find_reg_number(first_operand); /* Finds the register number */
    }
    if ((is_second)) {
        second_operand_type = find_operand_type(second_operand); /* Detect addressing method of second operand */
        if ((second_operand_type == METHOD_REGISTER))
            second_value = find_reg_number(second_operand); /* Finds the register number */
        else if ((second_operand_type == METHOD_IMMEDIATE))
            second_value = atoi(second_operand);
    }
    if ((is_third)) {
        third_operand_type = find_operand_type(third_operand); /* Detect addressing method of second operand */
        if ((third_operand_type == METHOD_REGISTER))
            third_value = find_reg_number(third_operand); /* Finds the register number */
    }

    if (!if_error()) /* If there was no error while trying to parse addressing methods */
    {
        if (command_accept_num_operands(type, is_first, is_second, is_third)) /* If number of operands is valid for this specific command */
        {
            if (command_accept_methods(type, first_operand_type, second_operand_type, third_operand_type)) /* If addressing methods are valid for this specific command */
            {
                /* add machine code of the command to memory and increase ic by command size (4) */
                add_to_instructions_image(build_machine_code(type, first_operand_type, second_operand_type, third_operand_type, first_value, second_value ,third_value));
                ic++;
            }

            else
            {
                error = COMMAND_INVALID_OPERANDS_METHODS;
                return ERROR;
            }
        }
        else
        {
            error = COMMAND_INVALID_NUMBER_OF_OPERANDS;
            return ERROR;
        }
   
    }

    return EMPTY_ERROR;
}

/* This function inserts .data_image to data_image memory */
void add_num_to_data_image(int num,int guidance_type) {
    int i;
    int x =num;
    for (i = 0; i < guidance_type; i++)
    {
        data_image[dc] = x;
        dc++;
        x=x>>8;
    }

}

/* This function tries to find the addressing method of a given operand and returns -1 if it was not found */
int find_operand_type(char* operand) {

    if (end_of_line(operand)) /* If end of line*/
        return NO_MATCH;

    /*----- label method check ----- */
    else if (is_label(operand, FALSE)) /* Checking if it's a label when there shouldn't be a colon (:) at the end */
        return METHOD_LABEL;
    
    /*----- Register method check -----*/
    else if (is_register(operand))
        return METHOD_REGISTER;

    else if (is_number(operand))
        return METHOD_IMMEDIATE;

    error = COMMAND_INVALID_METHOD;
    return NO_MATCH;
}

/* This function checks for the validity of given methods according to the opcode */
boolean command_accept_num_operands(int type, boolean first, boolean second, boolean third) {

    switch (type)
    {
    /* These opcodes can't have any operand */
    case STOP:
        return !first && !second && !third;

    /* These opcodes must only receive 1 operand */
    case JMP:
    case LA:
    case CALL:
        return first && !second && !third;

    /* These opcodes must receive 2 operands */
    case MOVE:
    case MVHI:
    case MVLO:
        return first && second && !third;

    /* These opcodes must receive 3 operands */
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
    case BEQ:
    case BNE:
    case BLT:
    case BGT:
    case LB:
    case SB:
    case LW:
    case SW:
    case LH:
    case SH:
        return first && second && third;
    }
    return FALSE;
}

/* This function checks for the validity of given addressing methods according to the opcode */
boolean command_accept_methods(int type, int first_operand_type, int second_operand_type, int third_operand_type) {

    switch (type)
    {
    case ADD:
    case SUB:
    case AND:
    case OR:
    case NOR:
        return (first_operand_type == METHOD_REGISTER) && (second_operand_type == METHOD_REGISTER) && (third_operand_type == METHOD_REGISTER);

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
        return (first_operand_type == METHOD_REGISTER) && (second_operand_type == METHOD_IMMEDIATE) && (third_operand_type == METHOD_REGISTER);

    case MOVE:
    case MVHI:
    case MVLO:
        return (first_operand_type == METHOD_REGISTER) && (second_operand_type == METHOD_REGISTER);

    case BEQ:
    case BNE:
    case BLT:
    case BGT:
        return (first_operand_type == METHOD_REGISTER) && (second_operand_type == METHOD_REGISTER) && (third_operand_type == METHOD_LABEL);

    case JMP:
    case LA:
    case CALL:
        return (first_operand_type == METHOD_LABEL || first_operand_type == METHOD_REGISTER);

    case STOP:
        return TRUE;
    }
    return FALSE;
}

/* This function encodes the machine code of the command */
unsigned int build_machine_code(int type, int first_operand_type, int second_operand_type, int third_operand_type, int first_value, int second_value, int third_value) {
    int funct = 0; 
    int opcode = 0;
    int code_word = 0;
    opcode = find_opcode(type); /* Finds the correct opcode  */
    funct = find_funct(type); /* Finds the correct funct   */
    code_word = opcode<<26;
    /* R type */
    if (opcode <= 1) {
        if (opcode == 1){
            third_value=second_value;
            second_value=0;
        }
        code_word |= first_value<<21;
        code_word |= second_value<<16;
        code_word |= third_value<<11;
        code_word |= funct<<6;
        code_word |= 0;
    }
    
    /* I type */
    else if (1 < opcode && opcode <= 24) {
        code_word |= first_value<<21;
        code_word |= third_value<<16;
        if (second_operand_type == METHOD_IMMEDIATE){
            code_word |= (second_value & 0xffff); 
        }
    }
    /*if label wait for secend scan*/
    /* J type */
    else if(opcode != 63){
        if (first_operand_type == METHOD_LABEL ){
            code_word |= (0 & 0x1) <<25; /*if label wait for secend scan*/
        }
        else if(first_operand_type == METHOD_REGISTER){
            code_word |= (1 & 0x1) <<25;
            code_word |= (first_value & 0xffffff);
        }
    }
    return code_word;
}


/* Finds the function number to insert in word */
int find_funct(int commands) {

    switch (commands)
    {
    case ADD:
    case MOVE:
        return 1;
        break;

    case SUB:
    case MVHI:
        return 2;
        break;

    case AND:
    case MVLO:
        return 3;
        break;

    case OR:
        return 4;
        break;

    case NOR:
        return 5;
        break;

    }
    return UNKNOWN_FUNCT;
}

 /* Finds the real number of the opcode to insert in word */
int find_opcode(int type) {
    
    switch (type)
    {
    case ADD:
    case SUB:
    case AND:
    case OR:
    case NOR:
        return 0;
        break;
    
    case MOVE:
    case MVHI:
    case MVLO:
        return 1;
        break;
    
    case ADDI:
        return 10;
        break;

    case SUBI:
        return 11;
        break;

    case ANDI:
        return 12;
        break;
   
    case ORI:
        return 13;
        break;

    case NORI:
        return 14;
        break;   

    case BNE:
        return 15;
        break;

    case BEQ:
        return 16;
        break;

    case BLT:
        return 17;
        break;
   
    case BGT:
        return 18;
        break;

    case LB:
        return 19;
        break;    

    case SB:
        return 20;
        break;
   
    case LW:
        return 21;
        break;

    case SW:
        return 22;
        break;   

    case LH:
        return 23;
        break;

    case SH:
        return 24;
        break;

    case JMP:
        return 30;
        break;
   
    case LA:
        return 31;
        break;

    case CALL:
        return 32;
        break;    

    case STOP:
        return 63;
        break;
    }
    return UNKNOWN_FUNCT;
}
