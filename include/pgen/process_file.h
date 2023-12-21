
/*
 *  process text file line by line, split each line into tokens
 *  and pass the tokens to the callback function
 *  the callback function should be defined as:  
 *  void process_line(char** tokens, int num_tokens);
 *  
 */
#ifndef _PROCESS_FILE_H_
#define _PROCESS_FILE_H_
#if defined(__cplusplus)
extern "C" {
#endif

void process_file(const char* filename, void (*process_line)(char**, int, void *), void *params);

#if defined(__cplusplus)
} // extern "C"
#endif
#endif // _PROCESS_FILE_H_
