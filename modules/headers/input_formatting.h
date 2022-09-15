#ifndef INPUT_FORMATTING_H_SENTRY
#define INPUT_FORMATTING_H_SENTRY

void clean_read_buf (uint8_t *buf, int32_t n);
void set_buf (uint8_t *buf, uint32_t size, uint8_t c);
void create_part_copa (uint8_t *buf, uint32_t size, copa **first, copa **last);

#endif
