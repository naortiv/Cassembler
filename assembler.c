#include "assembler.h"
#include "external_vars.h"
#include "scanFunctions.h"
#include "functions.h"

/*commands List*/
const char* commands[] = { "add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo", "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt", "lb", "sb", "lw", "sw", "lh", "sh", "jmp", "la", "call", "stop"};

/*guidance List*/
const char* guidance[] = { ".entry", ".db", ".dh", ".asciz", ".dw", ".extern" };

unsigned int instructions_image[MAX_LINES];
unsigned char data_image[MAX_LINES];
int ic;
int dc;
int error;

labelPtr symbols_table;
extPtr ext_list;
boolean entry_exists, extern_exists, error_exist;

/* This fucnction resets all flags for next file */
void reset_flags() {
    symbols_table = NULL;
    ext_list = NULL;
    entry_exists = FALSE;
    extern_exists = FALSE;
    error_exist = FALSE;
}

/* Check if file end with .as*/
int check_file_name(char *name){
    int len = strlen(name)-3;
    int i=0;
    for (i = 0; i < len; i++)
    {
        name++;
    }
    return (strcmp(name,".as")==0);
}


/* The main function of the program */
int main(int argc, char* argv[]) {
    int i;
    FILE* fp;

    if (argc < 2) {
        fprintf(stdout, "No argument found, please enter a file name to process\n");
        exit(1);
    }
    else {
        for (i = 1; i < argc; i++) {
            if (!check_file_name(argv[i]))
                fprintf(stderr, "\nInvalid file type: %s\n", argv[i]);
            else{
                if ((fp = fopen(argv[i], "r")) == NULL) { /*Open the file*/
                    fprintf(stderr, "\nCannot open file: %s\n", argv[i]);
                }
                else {
                    if (feof(fp) == 1)/* Checks for empty file */
                        fprintf(stderr, "\nThe file is empty\n");
                    else {
                        /*shadi say remove this msg */
                        reset_flags();/* Resets all flags for the next file */
                        scanOne(fp);

                        if (error == 0) {/* If there are no errors, continue with second pass */
                            rewind(fp);/*Starts the second pass from the start of the file*/
                            scanTwo(fp, argv[i]);
                        }
                        else {/* First pass contains errors, stop processing the file */
                            fprintf(stdout, "Errors found in file: %s, stoped working on file\n", argv[i]);
                        }
                        fclose(fp); /* Close file */
                    }
                }
                
            }
        }
    }
    return 0;
}
