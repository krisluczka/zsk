/*
	Krzysztof �uczka, czerwiec 2023

	Projekt udost�pniony na licencji OpenSource,
	stworzony w celach zaliczenia przedmiotu "Programowanie"
	w Technikum Komunikacji im. Hipolita Cegielskiego w Poznaniu
*/
#pragma once
#include <iostream>
#include <stack>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>

namespace zsk23 {
	class interpreter {
		public:
			// konstruktor
		interpreter();

		// destruktor
		~interpreter();

		// g��wna p�tla intepretera, umo�liwiaj�ca kontakt z u�ytkownikiem
		void main_loop();

		// funkcja wczytuj�ca program z wbudowanym preprocesorem
		bool load_program( std::string path );

		// funkcja wykonuj�ca za�adowany program
		void run_program();

		// funkcja ewaluuj�ca wynik odwrotnej notacji polskiej
		int_fast32_t evaluate_prn( std::string expression );
		int_fast32_t evaluate_prn_copy( std::string expression );

		private:
			// kontener na za�adowane indeksy linii kodu
		std::vector<uint_fast8_t> lines;

		// kontener na za�adowane argumenty komend
		std::vector<int_fast32_t> arguments;

		// kontener na przechowywanie wyra�e�
		std::vector<std::string*> expressions;

		// stos programu
		std::stack<int_fast32_t> main_stack;

		// wska�nik do pozycji w kodzie
		uint_fast64_t code_position;

		// pseudorejestry do por�wnywania
		int_fast32_t registerA, registerB;

		// liczniki do p�tli
		int_fast32_t loop_counter, loop_amount, loop_code_position, loop_final_position;

		// flagi interpretera
		bool debug;
	};
}

/*
	ID komend:
		0 -  brak
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