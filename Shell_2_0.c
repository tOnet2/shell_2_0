#include <stdint.h>
#include "modules/headers/Shell_2_0.h"
#include "modules/headers/mk_full_hist_file_path.h"
#include "modules/headers/input_formatting.h"
#include "modules/headers/process_control.h"
#include "modules/headers/help_functions.h"
#include "modules/headers/history_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

typedef struct command_parts copa;

enum {
	bufs_size = 1024,
	part_size_five = 5,
	part_size_six = 6,
	input_error_size = 35,
	w8_size = 1,
	h_f_size = 8,
	l_d_size = 10,
};

static const char w8[w8_size] = ">"; // w8 = wait.  Invation for some comand.
static const char input_error[] = "Incorrect input of control symbols\n";
static const char hist_file[h_f_size] = "history";				// CHANGE PATH
static const char local_dir[l_d_size] = "Shell_2_0";

char conveyor_part[part_size_five + 1] = "\" | \"";		/* DONT MOVE THEM */
char train_part[part_size_five + 1] = "\" ; \"";		/* DONT MOVE THEM */
char or_part[part_size_six + 1] = "\" || \"";		/* DONT MOVE THEM */
char background_part[part_size_five + 1] = "\" & \"";	/* DONT MOVE THEM */
char and_part[part_size_six + 1] = "\" && \"";		/* DONT MOVE THEM */
char output_to_start_part[part_size_five + 1] = "\" > \"";	/* DONT MOVE THEM */
char output_to_end_part[part_size_six + 1] = "\" >> \"";	/* DONT MOVE THEM */
char input_from_part[part_size_five + 1] = "\" < \"";	/* DONT MOVE THEM */
char bracket_left_part[part_size_five + 1] = "\" ( \"";	/* DONT MOVE THEM */
char bracket_right_part[part_size_five + 1] = "\" ) \"";	/* DONT MOVE THEM */
char quote_part[part_size_five + 1] = "\" \" \"";		/* DONT MOVE THEM */
char shield_part[part_size_five + 1] = "\" \\ \"";		/* DONT MOVE THEM */

static char read_buf[bufs_size];
static char part_buf[bufs_size / 4];
static char buf[bufs_size];
static char temp_buf[bufs_size];

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
		perror("Can't set termios new attributes");
		exit(1);
	}
	int32_t read_return, rbi, rbi_max, pbi, bi, S;
	int32_t copa_last_comp_value;
	int32_t shield_trigger, quote_trigger, input_error_trigger, bracket_trigger;
	int32_t for_umask = 0002;
	int32_t hist_file_exists, fd_hist;
	const char *full_hist_dir_path = create_hist_dir_path(local_dir);
	const char *full_hist_file_path = create_full_hist_path(full_hist_dir_path, hist_file);
	umask(for_umask);
	hist_file_exists = create_history_file(full_hist_dir_path, full_hist_file_path);
	copa *first, *last;
	first = last = NULL;
/*
 * copa *first and *last are pointer for struct command_parts (there are parts for future cmdline)
 * read_return for amount control of input characters
 * rbi - read buffer index for handing input.
 * pbi - part buffer index for control part buffer.
 * bi - buffer index
 * S - for exit of program (S need 'S' for this)
 * scsp - stack cmdline size pointer
 */
	bracket_trigger = copa_last_comp_value = input_error_trigger = shield_trigger = quote_trigger ^= quote_trigger;
	write(1, w8, w8_size);
/*
 * 0x8  = '\b'	(Backspace)
 * 0x1b = '^['	(Esc)
 * 0x5b = '['	(After Esc for some keys)
 * 0x9  = '\t'	(Tabulation)-----------------------
 * 0x10 = '\n'	(New line)
 * 0x20 = ' '	(Space)
 * 0x22 = '"'	(Symbol for using spaces and tabs
 * 		 in part and other syms)
 * 0x17 = '^W'	(Delete last word)
 * 0x26 = '&'	(For background process)
 * 0x28 0x29 =	'(' ')
 * 0x3b = ';'	(For traing of process)
 * 0x3c = '<'	(Redirect input)
 * 0x3e = '>'	(Redirect output)
 * 0x44 = 'D' 	(for <-)
 * 0x43 = 'C' 	(for ->)
 * 0x41 = 'A' 	(for ^)----------------------------
 * 0x42 = 'B' 	(for v)----------------------------
 * 0x5c = '\'	(Shielding)
 * 0x7c = '|'	(Conveyor symbol)
 * 0x7f = DEL	(Delete character under cursor)
 */
	goto go;

ccmd:	for (pbi = rbi ^= rbi; rbi < bufs_size; rbi++) {
		switch (read_buf[rbi]) {
			case 0:
				if (pbi > 0) {
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi ^= pbi;
				}
				if (quote_trigger || input_error_trigger || bracket_trigger\
					|| (last && ((last->part >= conveyor_part && last->part <= or_part)\
					|| (last->part >= and_part && last->part <= input_from_part)))) {
					write(2, input_error, input_error_size);
					bracket_trigger = input_error_trigger = quote_trigger ^= quote_trigger;
					goto break_execute;
				}
#if 1 
// This is for debugging, I see the parts of the future cmdline from struct command_parts. Set 0 instead 1 near #if for turn it off.
				write_copa(first);
				write(1, "\n", 1);
#endif
break_execute:			write(1, w8, sizeof(w8));
				free_copa(first);
				first = last = NULL;
				shield_trigger ^= shield_trigger;
				clear_buf(read_buf);
				goto go;
			case 0x5c: // '\'
				if (shield_trigger || quote_trigger) {
					shield_trigger ^= shield_trigger;
					goto dflt;
				}
				shield_trigger |= 1;
				break;
			case 0x22: // '"'
				if (shield_trigger) {
					shield_trigger ^= shield_trigger;
					goto dflt;
				}
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
				if (pbi > 0) {
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi ^= pbi;
				}
				copa_last_comp_value = comp_last_part_for_background((const copa*)last);
				if (copa_last_comp_value == 1)
					last->part = and_part;
				else if (copa_last_comp_value == 2)
					input_error_trigger++;
				else if (!copa_last_comp_value)
					create_part_control(background_part, &first, &last);
				break;
			case 0x28: // '('
				if (shield_trigger || quote_trigger) {
					shield_trigger ^= shield_trigger;
					goto dflt;
				}
				bracket_trigger++;
				if (pbi > 0) {
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi ^= pbi;
				}
				copa_last_comp_value = comp_last_part_for_bracket_left((const copa*)last);
				if (copa_last_comp_value == 1)
					input_error_trigger++;
				else if (!copa_last_comp_value)
					create_part_control(bracket_left_part, &first, &last);
				break;
			case 0x29: // ')'
				if (shield_trigger || quote_trigger) {
					shield_trigger ^= shield_trigger;
					goto dflt;
				}
				bracket_trigger--;
				if (pbi > 0) {
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi ^= pbi;
				}
				copa_last_comp_value = comp_last_part_for_bracket_right((const copa*)last);
				if (copa_last_comp_value == 1)
					input_error_trigger++;
				else if (!copa_last_comp_value)
					create_part_control(bracket_right_part, &first, &last);
				break;
			case 0x3b: // ';'
				if (shield_trigger || quote_trigger) {
					shield_trigger ^= shield_trigger;
					goto dflt;
				}
				if (pbi > 0) {
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi ^= pbi;
				}
				copa_last_comp_value = comp_last_part_for_train((const copa*)last);
				if (copa_last_comp_value == 1)
					input_error_trigger++;
				else if (!copa_last_comp_value)
					create_part_control(train_part, &first, &last);
				break;
			case 0x3c: // '<'
				if (shield_trigger || quote_trigger) {
					shield_trigger ^= shield_trigger;
					goto dflt;
				}
				if (pbi > 0) {
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi ^= pbi;
				}
				copa_last_comp_value = comp_last_part_for_input_from((const copa*)last);
				if (copa_last_comp_value == 1)
					input_error_trigger++;
				else if (!copa_last_comp_value)
					create_part_control(input_from_part, &first, &last);
				break;
			case 0x3e: // '>'
				if (shield_trigger || quote_trigger) {
					shield_trigger ^= shield_trigger;
					goto dflt;
				}
				if (pbi > 0) {
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi ^= pbi;
				}
				copa_last_comp_value = comp_last_part_for_output_to_start((const copa*)last);
				if (copa_last_comp_value == 1)
					last->part = output_to_end_part;
				else if (copa_last_comp_value == 2)
					input_error_trigger++;
				else if (!copa_last_comp_value)
					create_part_control(output_to_start_part, &first, &last);
				break;
			case 0x7c: // '|'
				if (shield_trigger || quote_trigger) {
					shield_trigger = 0;
					goto dflt;
				}
				if (pbi > 0) {
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi = 0;
				}
				copa_last_comp_value = comp_last_part_for_conveyor((const copa*)last);
				if (copa_last_comp_value == 1)
					last->part = or_part;
				else if (copa_last_comp_value == 2)
					input_error_trigger++;
				else if (!copa_last_comp_value)
					create_part_control(conveyor_part, &first, &last);
				break;
			case 0x9: //    '	'
			case 0x20: //   ' '
				shield_trigger ^= shield_trigger;
				if (quote_trigger)
					goto dflt;
				if (pbi > 0){
					create_part_copa((const char*)part_buf, pbi, &first, &last);
					clear_buf(part_buf);
					pbi ^= pbi;
				}
				break;
dflt:			default:
				part_buf[pbi++] = read_buf[rbi];
		}
	}

go:	for (rbi = rbi_max ^= rbi_max; (read_return = read(0, buf, bufs_size));) {
		if(read_return == -1){
			perror("Some wrong with read's call");
			exit(1);
		}
		for (bi ^= bi; bi < bufs_size && buf[bi]; bi++) {
			switch (buf[bi]) {
				case 0xa:
					write(1, "\n", 1);
					read_buf[rbi_max] = '\0';
					if (hist_file_exists == 1) {
						fd_hist = open(full_hist_file_path, O_RDWR);
						if (fd_hist == -1) goto ccmd;
						if (write(fd_hist, (const char*)read_buf, rbi_max) == -1 || write(fd_hist, "\n", 1) == -1) {
							unlink(full_hist_file_path);
							close(fd_hist);
							fd_hist = -1;
							hist_file_exists = 0;
							perror("Broken history system");
							goto ccmd;
						} else {
							;
						}
					}
					goto ccmd;
				case 0x8: // backspace
					if (!rbi) break;
					if (rbi == rbi_max) {
						rbi--;
						write(1, "\b \b", 3);
						read_buf[--rbi_max] = 0;
					} else {
						write(1, "\b", 1);
						write(1, (const char*)&read_buf[rbi], rbi_max - rbi);
						write(1, " ", 1);
						for (int r = 0; r <= rbi_max - rbi; r++)
							write(1, "\b", 1);
						middle_backspace_for_buf(read_buf, rbi, rbi_max);
						rbi--; rbi_max--;
					}
					break;
				case 0x17: // ^W
					if (!rbi) break;
					if (rbi == rbi_max) {
						for (; read_buf[rbi - 1] == 0x20; rbi--, rbi_max--) {
							write(1, "\b", 1);
							read_buf[rbi - 1] = 0;
						}
						for (; read_buf[rbi - 1] != 0x20 && rbi - 1 >= 0; rbi--, rbi_max--) {
							write(1, "\b \b", 3);
							read_buf[rbi - 1] = 0;
						}
					} else {
						int r, zero, for_b, syms_deleted, need_back;
						for (r = rbi; r > 0 && read_buf[r - 1] == 0x20; r--)
							write(1, "\b", 1);
						for (; r > 0 && read_buf[r - 1] != 0x20; r--)
							write(1, "\b", 1);
						zero = rbi - r;
						write(1, (const char*)&read_buf[rbi], rbi_max - rbi);
						for (; zero > 0; zero--)
							write(1, " ", 1);
						need_back = rbi_max - r;
						for (for_b = 0; for_b < need_back; for_b++)
							write(1, "\b", 1);
						middle_backword_for_buf(read_buf, r, rbi, rbi_max);
						syms_deleted = rbi - r;
						rbi -= syms_deleted;
						rbi_max -= syms_deleted;
					}
					break;
				case 0x1b: // ^[
					bi++;
					if (buf[bi] == 0x5b) { // [
						bi++;
						int lrud = buf[bi];
						if (lrud == 0x44) { // <- left
							if (rbi == 0) break;
							write(1, "\b", 1);
							write(1, (const char*)&read_buf[--rbi], 1);
							write(1, "\b", 1);
							break;
						} else if (lrud == 0x43) { // -> right
							if (rbi == rbi_max) break;
							write(1, (const char*)&read_buf[rbi++], 1);
							break;
						} else if (lrud == 0x41) { // ^ up
							break;
						} else if (lrud == 0x42) { // V down
							break;
						} else if (lrud == 0x33) {
							bi++;
							if (buf[bi] == 0x7e) { // del
								if (rbi == rbi_max) break;
								write(1, (const char*)&read_buf[rbi + 1], rbi_max - rbi - 1);
								write(1, " ", 1);
								if (rbi == rbi_max - 1) {
									write(1, "\b", 1);
								} else
									for (int32_t b = 0; b < rbi_max - rbi; b++)
										write(1, "\b", 1);
								rbi_max--;
								middle_del_for_buf(read_buf, rbi, rbi_max);
								break;
							}
						}
					}
				case 0x9: // tab
					break;
				case 'S': // END PROGRAMM
					S = 'S';
					break;
				default:
					if (!rbi && *buf == 0x20) {
						write(1, (const char*)buf, 1);
						break;
					}
					if (rbi == rbi_max) {
						read_buf[rbi] = buf[bi];
						write(1, (const char*)&read_buf[rbi++], 1);
						rbi_max++;
					} else {
						middle_insert_for_buf(read_buf, rbi, rbi_max);
						read_buf[rbi] = buf[bi];
						write(1, (const char*)&read_buf[rbi], rbi_max - rbi + 1);
						for (int r = 0; r < rbi_max - rbi; r++)
							write(1, "\b", 1);
						rbi++;
						rbi_max++;
					}
			}
		}
		clear_buf(buf);
		if (S == 'S')
			break;
	}
//	unlink(hist_file);
	free_copa(first);
	write(1, "\n", 1);
	tcsetattr(0, TCSADRAIN, &termios_old_p);
	//Restore old line desciple settings
	return 0;
}
