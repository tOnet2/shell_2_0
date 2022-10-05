#include <stdint.h>
#include "modules/headers/shell_2_0.h"
#include "modules/headers/input_formatting.h"
#include "modules/headers/process_control.h"
#include "modules/headers/help_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <termios.h>

typedef struct command_parts copa;

enum { 
	r_b_size = 1024,
	p_b_size = 4096,
	part_size_five = 5,
	part_size_six = 6,
	input_error_size = 35,
	w8_size = 1,
};

static const uint8_t w8[w8_size] = ">"; // w8 = wait.  Invation for some comand.
static const uint8_t input_error[] = "Incorrect input of control symbols\n";

uint8_t conveyor_part[part_size_five + 1] = "\" | \"";
uint8_t train_part[part_size_five + 1] = "\" ; \"";
uint8_t or_part[part_size_six + 1] = "\" || \"";
uint8_t background_part[part_size_five + 1] = "\" & \"";
uint8_t and_part[part_size_six + 1] = "\" && \"";
uint8_t output_to_start_part[part_size_five + 1] = "\" > \"";
uint8_t output_to_end_part[part_size_six + 1] = "\" >> \"";
uint8_t input_from_part[part_size_five + 1] = "\" < \"";
uint8_t bracket_left_part[part_size_five + 1] = "\" ( \"";
uint8_t bracket_right_part[part_size_five + 1] = "\" ) \"";

static uint8_t read_buf[r_b_size];
static uint8_t part_buf[p_b_size];

int main (int argc, char **argv)
{
	struct termios termios_new_p, termios_old_p;
	if (isatty(0)) {
		tcgetattr(0, &termios_old_p);
		memcpy(&termios_new_p, &termios_old_p, sizeof(termios_new_p));
		termios_new_p.c_lflag &= ~(ICANON | ECHO | ISIG);
		//off canon, off echo and off Ctrl+C/Ctrl+D/Ctrl+-
		termios_new_p.c_lflag |= TOSTOP;
		//Background processes cant use terminal for output
		termios_new_p.c_cc[VMIN] = 1;
		termios_new_p.c_cc[VTIME] = 0;
		//VMIN minimum characters for read and VTIME for a wait of this
		tcsetattr(0, TCSANOW, &termios_new_p);
		//Set new line desciple settings that we need
	} else {
		perror("Set termios new attributes");
		exit(1);
	}
	int32_t read_return, rbi, pbi, S;
	int32_t copa_last_comp_value;
	uint8_t shield_trigger, quote_trigger, input_error_trigger, space_trigger;
	copa *first, *last;
	first = last = NULL;
	/*
	 * copa *first and *last are pointer for struct command_parts (there are parts for future cmdline)
	 * read_return for amount control of input characters
	 * rbi - read buffer index for handing input.
	 * pbi - part buffer index for control part buffer.
	 * S - for exit of program (S need 'S' for this)
	 */
	space_trigger = copa_last_comp_value = input_error_trigger = shield_trigger = quote_trigger = pbi ^= pbi;
	write(1, w8, w8_size);
	while ((read_return = read(0, read_buf, r_b_size))) {
		if(read_return == -1){
			perror("Some wrong with read's call");
			exit(1);
		}
		for (rbi ^= rbi; read_buf[rbi]; rbi++) {
			/*
			 * 0x8  = '\b'	(Backspace)
			 * 0x1b = '^['	(Esc)
			 * 0x5b = '['	(After Esc for some keys)
			 * 0x9  = '\t'	(Tabulation)
			 * 0x10 = '\n'	(New line)
			 * 0x18 = '^R'	(Restore last word)
			 * 0x20 = ' '	(Space)
			 * 0x22 = '"'	(Symbol for using spaces and tabs
			 * 		 in part and other syms)
			 * 0x23 = '^W'	(Delete last word)
			 * 0x26 = '&'	(For background process)
			 * 0x28 0x29 =	'(' ')'
			 * 0x3b = ';'	(For traing of process)
			 * 0x3c = '<'	(Redirect input)
			 * 0x3e = '>'	(Redirect output)
			 * 0x44 = 'D' 	(for <-)
			 * 0x43 = 'C' 	(for ->)
			 * 0x41 = 'A' 	(for ^)
			 * 0x42 = 'B' 	(for v)
			 * 0x5c = '\'	(Shielding)
			 * 0x7c = '|'	(Conveyor symbol)
			 *
			 */
			switch (read_buf[rbi]) {
				case 0xA:
					write(1, "\n", 1);
					if (quote_trigger || input_error_trigger) {
						write(2, input_error, input_error_size);
						input_error_trigger = quote_trigger ^= quote_trigger;
						goto break_execute;
					}
					if (pbi > 0) {
						create_part_copa((const uint8_t*)part_buf, pbi, &first, &last);
						set_buf(part_buf, pbi, '\0');
						pbi ^= pbi;
					}
#if 1 // this is for debugging, I see parts of future cmdline from struct 
      // command_parts write 0 instead 1 near #if for turn it off
					write_copa(first);
					write(1, "\n", 1);
#endif
break_execute:				write(1, w8, sizeof(w8));
					free_copa(first);
					first = last = NULL;
					space_trigger = shield_trigger ^= shield_trigger;
					break;
				case 0x5c: // '\'
					if (shield_trigger) {
						shield_trigger ^= shield_trigger;
						goto dflt;
					}
					write(1, "\\", 1);
					shield_trigger |= 1;
					break;
				case 0x22: // '"'
					if (shield_trigger) {
						shield_trigger ^= shield_trigger;
						goto dflt;
					}
					write(1, "\"", 1);
					if (quote_trigger)
						quote_trigger ^= quote_trigger;
					else
						quote_trigger |= 1;
					break;
				case 0x26: // '&'
					if (shield_trigger || quote_trigger) {
						shield_trigger ^= shield_trigger;
						goto dflt;
					}
					write(1, "&", 1);
					if (pbi > 0) {
						create_part_copa((const uint8_t*)part_buf, pbi, &first, &last);
						set_buf(part_buf, pbi, '\0');
						pbi ^= pbi;
						create_part_control(background_part, &first, &last);
						space_trigger ^= space_trigger;
						break;
					}
					copa_last_comp_value = comp_last_part_for_background((const copa*)last);
					if (copa_last_comp_value == 2 || (copa_last_comp_value == 1 && space_trigger))
						input_error_trigger = 1;
					else if (copa_last_comp_value == 1)
						last->part = and_part;
					else
						create_part_control(background_part, &first, &last);
					break;
				case 0x28: // '('
					if (shield_trigger || quote_trigger) {
						shield_trigger ^= shield_trigger;
						goto dflt;
					}
					write(1, "(", 1);
					break;
				case 0x3b: // ';'
					if (shield_trigger || quote_trigger) {
						shield_trigger ^= shield_trigger;
						goto dflt;
					}
					write(1, ";", 1);
					if (pbi > 0) {
						create_part_copa((const uint8_t*)part_buf, pbi, &first, &last);
						set_buf(part_buf, pbi, '\0');
						pbi ^= pbi;
						create_part_control(train_part, &first, &last);
						break;
					}
					copa_last_comp_value = comp_last_part_for_train((const copa*)last);
					if (copa_last_comp_value == 1)
						input_error_trigger = 1;
					else
						create_part_control(train_part, &first, &last);
					break;
				case 0x7c: // '|'
					if (shield_trigger || quote_trigger) {
						shield_trigger ^= shield_trigger;
						goto dflt;
					}
					write(1, "|", 1);
					if (pbi > 0) {
						create_part_copa((const uint8_t*)part_buf, pbi, &first, &last);
						set_buf(part_buf, pbi, '\0');
						pbi ^= pbi;
						create_part_control(conveyor_part, &first, &last);
						break;
					}
					break;
				case 0x9: //    '	'
				case 0x20: //   ' '
					if (shield_trigger)
						shield_trigger ^= shield_trigger;
					if (quote_trigger)
						goto dflt;
					else {
						if (pbi > 0){
							create_part_copa((const uint8_t*)part_buf, pbi, &first, &last);
							set_buf(part_buf, pbi, '\0');
							pbi ^= pbi;
						} else
							space_trigger = 1;
						if (read_buf[rbi] == 0x9)
							write(1, "	", 1);
						else if (read_buf[rbi] == 0x20)
							write(1, " ", 1);
					}
					break;
				case 'S':
					S = 'S';
					break;
dflt:				default:
					part_buf[pbi] = read_buf[rbi];
					write(1, (const uint8_t*)&(part_buf[pbi]), 1);
					pbi++;
			}
		}
		clean_read_buf(read_buf, read_return);
		if(S == 'S') break;
	}
	free_copa(first);
	write(1, "\n", 1);
	tcsetattr(0, TCSADRAIN, &termios_old_p);
	//Restore old line desciple settings
	return 0;
}
