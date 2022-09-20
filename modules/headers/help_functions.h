#ifndef HELP_FUNCTIONS_H_SENTRY
#define HELP_FUNCTIONS_H_SENTRY

void write_copa (copa *t);
uint32_t size_of_copa_part (uint8_t *part);
void change_last_copa_part(uint8_t *part, const uint8_t *new_part, int32_t s_new_part);
int32_t comp_last_copa_part (uint8_t *part, const uint8_t *buf, int32_t s_buf);

#endif
