#include "assembler.h"
#include "external_vars.h"
#include "scanFunctions.h"
#include "functions.h"

/*This file include functions for all program use*/ 

/*********functions for main**********/

/* Function for creates the filename */
char* create_file_name(char* original, int type) {
    char* modified = (char*)malloc(strlen(original) + 1); /* A memory will be assigned for the part after the "." */
    char* tmp;
    if (modified == NULL)/*if there is a problem at malloc*/
    {
        fprintf(stderr, "Problem at the allocation of memory.");
        exit(ERROR);
    }
    
    strcpy(modified, original); /* Copying file name to a bigger string */
    tmp = strchr(modified,'.');/* Cut .as end */
    *tmp='\0';
    /* add filetype extension */
    switch (type)
    {
    case FILE_OBJECT:
        strcat(modified, ".ob");
        break;

    case FILE_ENTRY:
        strcat(modified, ".ent");
        break;

    case FILE_EXTERN:
        strcat(modified, ".ext");
    }
    return modified;
}

/* Checks if it's empty line or line needs to be ignored (;) */
int ignore_line(char* line) {
    line = skip_spaces(line);
    if (*line == ';' || *line == '\0' || *line == '\n') {
        return TRUE;
    }
    return FALSE;
}

/*This function check if was a error , and sign the errors's flag*/
int if_error() {

    if (error != EMPTY_ERROR)
        return 1;
    else
        return 0;
}

/* This function skips white space*/
char* skip_spaces(char* ch) {

    if (ch == NULL) 
        return NULL;
    while (isspace(*ch))/* Skip a white char */
        ch++;
    return ch;/* Return if isn't a white char  */
}

/* Checking if it's end of  line */
int end_of_line(char* line) {

    return (line == NULL || *line == '\0' || *line == '\n');
}

/* Copies the next word from the line to the destination */
void copy_word(char* destination, char* line) {

    int i = 0;
    if (destination == NULL || line == NULL) /*If one of them is not defined*/
        return;

    while (i < MAX_INPUT && !isspace(line[i]) && line[i] != '\0') /* Copy the line to *dest */
    {
        destination[i] = line[i];
        i++;
    }
    destination[i] = '\0'; /* Add ending to the string */
}

/* This function extracts bits */
unsigned int extract_bits(unsigned int word, int start, int end) {

    unsigned int result;
    int length = end - start + 1; /* Length bit-sequence */
    unsigned int mask = (int)pow(2, length) - 1; /* Creating a '111...1' mask */

    mask <<= start; /* Moving mask to the location of extraction */
    result = word & mask; /* The bits are now in their original position, and the rest is 0's */
    result >>= start; /* Moving the sequence to LSB */
    return result;
}

/* Divite int  to hex */
unsigned int int_to_hex(unsigned int word,int i) {
    int j= word;
    j= (word>>(8*i)) & 0xff;
    return j;
}


/* Check if it's a label word */
boolean is_label(char* word, int colon) {

    boolean has_digits = FALSE; /* If it's has a digits it's can be a command */
    int word_len = strlen(word);
    int i;

    /* Check if the length isn't too short */
    if (colon == TRUE) {
        if (word == NULL || word_len < 2) /* If colon ,2 is the min lengths label*/
            return FALSE; 
    }

    if (colon && (word[word_len - 1] != ':'))
        return FALSE; /* If colon , must be a colon at the end */

    if (word_len > MAX_LABEL) {
        if (colon) {
            error = LABEL_TOO_LONG; /* Max length label is 32 */
        }
        return FALSE;
    }
    if (!isalpha(*word)) {
        if (colon) {
            error = LABEL_INVALID_FIRST_CHAR;/* First char must be a letter */
        }
        return FALSE;
    }

    if (colon) { /* Remove colon */
        word[word_len - 1] = '\0';
        word_len--;
    }

    /* Check if all the charcters are digits or letters */
    for (i = 0; i < word_len; i++)
    {
        if (isdigit(word[i]))
            has_digits = TRUE;
    }

    if (!has_digits) /* If there not digits it can't be a command */
    {
        if (find_command(word) != NO_MATCH) {
            if (colon) {
                error = LABEL_CANT_BE_COMMAND; /* Label can't be a command name */
            }
            return FALSE;
        }
    }

    if (is_register(word)) /* Label can't be a register name */
    {
        if (colon) {
            error = LABEL_CANT_BE_REGISTER;
        }
        return FALSE;
    }

    return TRUE; /* Its a label */
}

/* gets next word */
char* next_word(char* seq) {

    if (seq == NULL)
        return NULL;
    while (!isspace(*seq) && !end_of_line(seq)) {/* Skip characters while aren't space or end of line  */
        seq++; 
    }
    seq = skip_spaces(seq); /* Skip white space */
    if (end_of_line(seq)) {
        return NULL;
    }
    return seq;
}

/* Check if the word is register name */
boolean is_register(char* word) {
    /* check if the first char is '$' and the register num between 0-31 */
    if (word[0] == '$') {
        int r = find_reg_number(word);
        if(0<=r && r<32)
            return TRUE;
        else{
            error = UNVALID_REG;
            return FALSE;
        }
    } 
    else return FALSE;      
}

/* Find the register num */
int find_reg_number(char* word) {
    int number = -1;
    if (word[0] == '$')
    {
        word++;
        if(is_number(word)){/*divite to a digits*/
            number = atoi(word);
        }
    }
    return number;
}

/*Finds an index of a string in an strings array */
int find_index(char* word, const char* arr[], int n) {
    int i;
    for (i = 0; i < n; i++)
        if (strcmp(word, arr[i]) == 0) {
            return i;
        }
    return NO_MATCH;
}

/* Check if the word is a guidance */
int find_guidance(char* word) {
    if (word == NULL || *word != '.') {
        return NO_MATCH;
    }/* Check in unum of guidance if the num there*/
    return find_index(word, guidance, 6);
}

/* Check if the word is a command */
int find_command(char* word) {

    int enum_index;
    int word_len = strlen(word);
    if (word_len > 4 || word_len < 2)/* A command must be between 2 and 4 chars */
        return NO_MATCH;
    else
        enum_index = find_index(word, commands, NUMBER_OF_COMMANDS); 
    return enum_index;
}

/* Copies the next word to dest array */
char* next_list_word(char* dest, char* line) {

    char* temp = dest;

    if (end_of_line(line)) /* If it's empty line, copy an empty word */
    {
        dest[0] = '\0';
        return NULL;
    }

    if (isspace(*line)) /* If the word start with white spaces , skip them */
        line = skip_spaces(line);

    if (*line == ',') /* if it's a ',' */
    {
        strcpy(dest, ",");
        return ++line;
    }

    /* Copy the word until is a ',', white space or line end */
    while (!end_of_line(line) && *line != ',' && !isspace(*line))
    {
        *temp = *line;
        temp++;
        line++;
    }
    *temp = '\0'; /* Add end to the word*/

    return line;
}

/* Checks if it's a num ( can be with a +/- signs) */
boolean is_number(char* seq) {
    if (end_of_line(seq)) 
        return FALSE;
    if (*seq == '+' || *seq == '-') {/*check if there is a +/- sign*/
        seq++;
        if (!isdigit(*seq++))/*check if isn't a digit*/ 
            return FALSE; 
    }

    /* Check until the end if are's a digits*/
    while (!end_of_line(seq)) {
        if (!isdigit(*seq++)) return FALSE;
    }
    return TRUE;
}


/* Copies next string to dest */
char* next_word_string(char* dest, char* line) {

    char temp[MAX_INPUT];
    line = next_list_word(dest, line);/* Creat function */
    if (*dest != '"') 
        return line;
    while (!end_of_line(line) && dest[strlen(dest) - 1] != '"')
    {
        line = next_list_word(temp, line);
        if (line) strcat(dest, temp);
    }
    return line;
}

/* Checks if the sequence is a valid string (with "") */
boolean is_string(char* string) {

    if (string == NULL)
        return FALSE;

    if (*string == '"') /* Start with " */
        string++;
    else
        return FALSE;

    while (*string && *string != '"') { /*  loop until the end of string */
        string++;
    }

    if (*string != '"') /* check the string end with " */
        return FALSE;

    string++;
    if (*string != '\0') /* check that the string isn't continue after the '"' */
        return FALSE;

    return TRUE; /* return it's a string */
}

/* If it's a string add to data_image */
void add_string_to_data_image(char* str) {

    while (!end_of_line(str))
    {
        data_image[dc++] = *str; /* Insert a char to data_image array */
        str++;
    }
    data_image[dc++] = '\0';
}

/* Inserts this machine_code to instructions_image memory */
void add_to_instructions_image(unsigned int machine_code) {

    instructions_image[ic] = machine_code;
}

/* print errors */
void print_error(int line_number) {
  
    fprintf(stderr, "ERROR (line %d): ", line_number);

    switch (error)
    {
    case INVALID_SYNTAX:
        fprintf(stderr, "first non-blank character must be a letter or a dot.\n");
        break;

    case LABEL_ALREADY_EXISTS:
        fprintf(stderr, "this label already exists.\n");
        break;

    case LABEL_TOO_LONG:
        fprintf(stderr, "label is too long (MAX_LABEL_LENGTH: %d).\n", MAX_LABEL);
        break;

    case LABEL_INVALID_FIRST_CHAR:
        fprintf(stderr, "label must start with an alphanumeric character.\n");
        break;

    case LABEL_CANT_BE_COMMAND:
        fprintf(stderr, "label can't have the same name as a command.\n");
        break;

    case LABEL_CANT_BE_REGISTER:
        fprintf(stderr, "label can't have the same name as a register.\n");
        break;

    case INVALID_LABEL_LINE:
        fprintf(stderr, "label must be followed by a command or guidance.\n");
        break;

    case NO_PARAMETER_AVAILABLE:
        fprintf(stderr, "guidance must have parameters.\n");
        break;

    case GUIDANCE_INVALID_NUM_PARAMS:
        fprintf(stderr, "illegal number of parameters for a directive.\n");
        break;

    case DATA_COMMAS_IN_A_ROW:
        fprintf(stderr, "incorrect usage of commas in a .data_image directive.\n");
        break;

    case DATA_EXPECTED_NUM:
        fprintf(stderr, ".data_image expected a numeric parameter.\n");
        break;

    case DATA_EXPECTED_COMMA_AFTER_NUM:
        fprintf(stderr, ".data_image expected a comma after a numeric parameter.\n");
        break;

    case DATA_UNEXPECTED_COMMA:
        fprintf(stderr, ".data_image got an unexpected comma after the last number.\n");
        break;

    case STRING_TOO_MANY_OPERANDS:
        fprintf(stderr, ".string must contain exactly one parameter.\n");
        break;

    case STRING_OPERAND_NOT_VALID:
        fprintf(stderr, ".string operand is invalid.\n");
        break;

    case EXTERN_NO_LABEL:
        fprintf(stderr, ".extern directive must be followed by a label.\n");
        break;

    case EXTERN_INVALID_LABEL:
        fprintf(stderr, ".extern directive received an invalid label.\n");
        break;

    case EXTERN_TOO_MANY_OPERANDS:
        fprintf(stderr, ".extern must only have one operand that is a label.\n");
        break;

    case MISSING_SYNTAX:
        fprintf(stderr, "invalid command or guidance.\n");
        break;

    case COMMAND_UNEXPECTED_CHAR:
        fprintf(stderr, "invalid syntax of a command.\n");
        break;

    case COMMAND_TOO_MANY_OPERANDS:
        fprintf(stderr, "command can't have more than 2 operands.\n");
        break;

    case COMMAND_INVALID_NUMBER_OF_OPERANDS:
        fprintf(stderr, "number of operands does not match command requirements.\n");
        break;

    case COMMAND_INVALID_OPERANDS_METHODS:
        fprintf(stderr, "operands' addressing methods do not match command requirements.\n");
        break;

    case ENTRY_LABEL_DOES_NOT_EXIST:
        fprintf(stderr, ".entry directive must be followed by an existing label.\n");
        break;

    case ENTRY_CANT_BE_EXTERN:
        fprintf(stderr, ".entry can't apply to a label that was defined as external.\n");
        break;

    case COMMAND_LABEL_DOES_NOT_EXIST:
        fprintf(stderr, "label does not exist.\n");
        break;

    case CANNOT_OPEN_FILE:
        fprintf(stderr, "there was an error while trying to open the requested file.\n");
        break;

    case UNVALID_REG:
        fprintf(stderr, "the register number is invalid.\n");
        break;

    case NO_LABEL_FOUND:
        fprintf(stderr, "no such label found in file for this method.\n");
        break;

    case UNVALID_LABEL:
        fprintf(stderr, "incorrect label syntax.\n");
        break;
    }

}
