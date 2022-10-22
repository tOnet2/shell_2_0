#ifndef INPUT_FORMATTING_H_SENTRY
#define INPUT_FORMATTING_H_SENTRY

void clean_read_buf (uint8_t *buf, int32_t n);
void set_buf (uint8_t *buf, int32_t size, uint8_t c);
void create_part_control (uint8_t *buf, copa **first, copa **last);
void create_part_copa (const uint8_t *buf, int32_t size, copa **first, copa **last);
int32_t comp_last_part_for_background (const copa *last);
int32_t comp_last_part_for_train (const copa *last);
int32_t comp_last_part_for_conveyor (const copa *last);
int32_t comp_last_part_for_bracket_left (const copa *last);
int32_t comp_last_part_for_bracket_right (const copa *last);
int32_t comp_last_part_for_input_from (const copa *last);
int32_t comp_last_part_for_output_to_start (const copa *last);
void free_copa (copa *t);
uint32_t size_of_copa_part (const uint8_t *part); // without '\0' symbol
void middle_backspace_for_buf (uint8_t *buf, int32_t from, int32_t to);

#endif
