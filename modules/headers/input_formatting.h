#ifndef INPUT_FORMATTING_H_SENTRY
#define INPUT_FORMATTING_H_SENTRY

void clean_read_buf (char *buf, int32_t n);
void set_buf (char *buf, int32_t size, char c);
void create_part_control (char *buf, copa **first, copa **last);
void create_part_copa (const char *buf, int32_t size, copa **first, copa **last);
int32_t comp_last_part_for_background (const copa *last);
int32_t comp_last_part_for_train (const copa *last);
int32_t comp_last_part_for_conveyor (const copa *last);
int32_t comp_last_part_for_bracket_left (const copa *last);
int32_t comp_last_part_for_bracket_right (const copa *last);
int32_t comp_last_part_for_input_from (const copa *last);
int32_t comp_last_part_for_output_to_start (const copa *last);
void free_copa (copa *t);
uint32_t size_of_copa_part (const char *part); // without '\0' symbol
void middle_backspace_for_buf (char *buf, int32_t from, int32_t to);
void middle_backword_for_buf (char *buf, int32_t new_pos, int32_t from, int32_t to);
void middle_del_for_buf (char *buf, int32_t from, int32_t to);
void middle_insert_for_buf (char *buf, int32_t from, int32_t to);
void cp_buf1tobuf2 (const char *buf1, char *buf2);
void clear_buf (char *buf);

#endif
