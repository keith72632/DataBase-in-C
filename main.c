#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* buffer;
    size_t buffer_length;
    size_t input_length;
} InputBuffer;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

InputBuffer* new_input_buffer(){
    InputBuffer* input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt(){ printf("db > "); }

void read_input(InputBuffer* input_buffer){
    ssize_t bytes_read = 
        getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if(bytes_read <= 0){
        printf("Error Reading Input\n");
        exit(EXIT_FAILURE);
    }

    //ignore trailing newline
    input_buffer->input_length = bytes_read -1;
    input_buffer->buffer[bytes_read -1] = 0;
}

void close_input_buffer(InputBuffer* input_buffer){
    free(input_buffer->buffer);
    free(input_buffer);
}

//prototypes
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

int main(int argc, char* argv[]){
    InputBuffer* input_buffer = new_input_buffer();
    while(1){
       print_prompt();
       read_input(input_buffer);
        
        if(strcmp(input_buffer->buffer, ".exit") == 0){
            printf("Exiting...\n");
            close_input_buffer(input_buffer);
            exit(EXIT_SUCCESS);
        } else {
            if(input_buffer->buffer[0] == '.'){
                switch(do_meta_command(input_buffer)){
                    case (META_COMMAND_SUCCESS):
                        continue;
                    case(META_COMMAND_UNRECOGNIZED):
                        printf("Unrecognized command\n");
                        continue;
                }
            }

            Statement statement;
            switch(prepare_statement(input_buffer, &statement)){
                case(PREPARE_SUCCESS):
                    break;
                case(PREPARE_UNRECOGNIZED_STATEMENT):
                    printf("Unrecognized keyword at start of '%s'.\n",
                            input_buffer->buffer);
                    continue;
            }

            execute_statement(&statement);
            printf("Executed.\n");
        }

    }

    return 0;
}
