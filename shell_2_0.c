#include <stdint.h>
#include "shell_2_0.h"
#include "modules/input_formatting.h"
#include "modules/process_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <termios.h>

typedef struct command_parts copa;

enum { r_b_size = 1024, w_b_size = 4096 };

static const uint8_t w8[1] = ">"; // w8 = wait.  Invation for some comand.

static uint8_t read_buf[r_b_size];
static uint8_t word_buf[w_b_size];

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
	int32_t read_return;
	uint32_t rbi, wbi, S;
	/*
	 * read_return for amount control of input characters
	 * rbi - read buffer index for handing input.
	 * wbi - word buffer index for control word buffer.
	 * S - for exit of program (S need 'S' for this)
	 */
	wbi ^= wbi; // "^=" like "= 0". But operations with bites faster:)
	write(1, w8, sizeof(w8));
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
			 * 0x23 = '^W'	(Delete last word)
			 * 0x44 = 'D' 	(<-)
			 * 0x43 = 'C' 	(->)
			 * 0x41 = 'A' 	(^)
			 * 0x42 = 'B' 	(v)
			 */
			switch (read_buf[rbi]) {
				case 0x10:
					write(1, "\n", 1);
					write(1, w8, sizeof(w8));
					break;
				case 0x20:
				// im here, I have make a parts of command :)
				default:
					word_buf[wbi++] = read_buf[rbi];
					write(1, (const char*)word_buf, 1);
			}
		}
		clean_read_buf(read_buf, read_return);
		if(S == 'S') break;
	}
	write(1, "\n", 1);
	tcsetattr(0, TCSADRAIN, &termios_old_p);
	//Restore old line desciple settings
	return 0;
}
