#ifndef PROCESS_CONTROL_H
#define PROCESS_CONTROL_H

char **copa_to_cmdline (const copa *t);
size_t copa_elements (const copa *t);
int32_t execute_cmdline (char **cmdline, struct termios *ttty);
int8_t cmdline_has_ (const char *_item, const char **cmdline);
int32_t set_redirects (const char **cmdline);


#endif
