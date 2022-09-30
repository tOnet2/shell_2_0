#ifndef INPUT_FORMATTING_H_SENTRY
#define INPUT_FORMATTING_H_SENTRY

void clean_read_buf (uint8_t *buf, int32_t n);
void set_buf (uint8_t *buf, int32_t size, uint8_t c);
void create_part_copa (const uint8_t *buf, int32_t size, copa **first, copa **last);
void free_copa (copa *t);
void change_last_copa_part(uint8_t *part, const uint8_t *new_part, int32_t s_new_part);
int32_t comp_last_copa_part_for_background_conveyor_train (const copa *t, const uint8_t *buf, int32_t s_buf);
int32_t check_read_buf_control_symbol(const uint8_t *read_buf, int32_t read_buf_size, uint8_t symbol);
uint32_t size_of_copa_part (uint8_t *part);

#endif
