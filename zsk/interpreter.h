/*
	Krzysztof Łuczka

	Project released under the MIT license.
	Originally created for the purpose of passing the "Programowanie" course
	at the Technikum Komunikacji im. Hipolita Cegielskiego in Poznań.
*/
#pragma once
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>

namespace zsk {
	class interpreter {
		public:
			interpreter();
			~interpreter();

			// main loop
			void main_loop();

			// preprocessor
			bool load_program( std::string );

			// running loaded program
			void run_program();

			// polish reversed notation
			int_fast32_t evaluate_prn( std::string );
			int_fast32_t evaluate_prn_copy( std::string );

		private:
			// code instructions
			std::vector<uint_fast8_t> lines;

			// argument container
			std::vector<int_fast32_t> arguments;

			// expression container
			std::vector<std::string*> expressions;

			// stack
			std::stack<int_fast32_t> main_stack;

			// code pointer
			uint_fast64_t code_position;

			// pseudoregisters
			int_fast32_t registerA, registerB;

			// loop counters
			int_fast32_t loop_counter, loop_amount, loop_code_position, loop_final_position;

			// interpreter's flags
			bool debug;
	};
}

/*
	Command id:
		0 -  none
		1 -  push
		2 -  stack
		3 -  pop
		4 -  print
		5 -  calc
		6 -  kill
		7 -  if=
		8 -  if>
		9 -  if<
		10 - if>=
		11 - if<=
		12 - if!
		13 - loop
		14 - getstring
		15 - getinteger
		16 - jump
		17 - copy
		18 - ~calc
		19 - string
		20 - ~if=
		21 - ~if>
		22 - ~if<
		23 - ~if>=
		24 - ~if<=
		25 - ~if!
		26 - random
		27 - reverse
		28 - size
*/