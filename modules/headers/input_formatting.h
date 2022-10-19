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
int32_t comp_last_part_for_and (const copa *last);
int32_t comp_last_part_for_or (const copa *last);
int32_t comp_last_part_for_output_to_end (const copa *last);
int32_t comp_last_part_for_new_part (const copa *last, int32_t quote_trigger);
void fill_space_buffer (uint8_t *buf, int32_t length);
void copa_part_backspace (const uint8_t *part, uint8_t *new_part, int32_t size_for_copy);
int32_t space_part_check (const uint8_t *part);
void free_copa (copa *t);
void change_last_copa_part_control (copa *last, uint8_t *control);
uint32_t size_of_copa_part (const uint8_t *part); // without '\0' symbol
int32_t comp_str1_with_str2 (const uint8_t *str1, const uint8_t *str2);
void copy_str1_to_str2 (const uint8_t *str1, uint8_t *str2, int32_t str1_size);
void pbi_spaceCount_copaLastCompValue (uint8_t *part_buf, uint8_t *space_buf, copa **first\
	, copa **last, uint8_t *control_part1, uint8_t *contol_part2, int32_t *pbi\
	, int32_t *space_count, int32_t *input_error_trigger, int32_t reset_space_count_for_andor_parts\
	, int32_t (*comp_last_part_for)(const copa*));

#endif
