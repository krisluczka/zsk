#include "interpreter.h"

namespace zsk {
	interpreter::interpreter() {
		code_position = 0;
		registerA = 0; registerB = 0;
		loop_counter = 0; loop_amount = 0; loop_code_position = 0; loop_final_position = 0;
		debug = false;
	}

	interpreter::~interpreter() {
		for ( uint_fast64_t i( 0 ); i < expressions.size(); ++i )
			delete expressions[i];
	}

	void interpreter::main_loop() {
		std::string input;
		std::cout << "ZSK prerelease 24.0, by Krzysztof Luczka" << std::endl;
		while ( true ) {
			std::cout << " >>  ";
			getline( std::cin, input );

			// help  - pomoc
			if ( input.substr( 0, 4 ) == "help" ) {
				std::cout << " - help           - wyswietla pomoc dot. komend interpretatora" << std::endl;
				std::cout << " - load <sciezka> - zaladowuje program do interpretatora" << std::endl;
				std::cout << " - run            - uruchamia program" << std::endl;
				std::cout << " - debug          - zmienia tryb debugowania w interpretatorze" << std::endl;
			}

			// load  - �adowanie programu
			else if ( input.substr( 0, 4 ) == "load" ) {
				if ( load_program( input.substr( 5, 4096 ) ) )
					std::cout << " Poprawnie zaladowano program. " << std::endl;
				else
					std::cout << " Nie znaleziono pliku. " << std::endl;
			}

			// run  -  uruchamianie programu	
			else if ( input.substr( 0, 3 ) == "run" ) {
				system( "cls" );
				run_program();
				break;
			}

			// debug - tryb debugowania kodu	
			else if ( input.substr( 0, 5 ) == "debug" ) {
				debug = !debug;
				if ( debug ) std::cout << " Wlaczono tryb debugowania." << std::endl;
				else std::cout << " Wylaczono tryb debugowania." << std::endl;
			}
		}
	}

	bool interpreter::load_program( std::string path ) {
		std::ifstream file( path );
		if ( !file.good() ) return false;
		std::string code_line;
		int_fast32_t argument;

		// za�adowywanie komend do bloku kodu
		while ( std::getline( file, code_line ) ) {

			// komenda wrzucania na stos
			if ( code_line.substr( 0, 4 ) == "push" ) {
				lines.push_back( 1 );
				if ( code_line.substr( 4, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid argument, nothing to push on a stack." << std::endl;
				}
			}

			// komenda drukowania stosu
			else if ( code_line.substr( 0, 5 ) == "stack" ) {
				lines.push_back( 2 );
				if ( code_line.substr( 5, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda zrzucania ze stosu
			else if ( code_line.substr( 0, 3 ) == "pop" ) {
				lines.push_back( 3 );
				if ( code_line.substr( 3, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 3, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda wy�wietlaj�ca znaki ascii
			else if ( code_line.substr( 0, 5 ) == "print" ) {
				lines.push_back( 4 );
				if ( code_line.substr( 5, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda wykonuj�ca obliczenia
			else if ( code_line.substr( 0, 4 ) == "calc" ) {
				lines.push_back( 5 );
				std::string* calcval = new std::string;
				*calcval = code_line.substr( 4, 256 );
				if ( calcval->size() > 1 ) {
					expressions.push_back( calcval );
					arguments.push_back( expressions.size() - 1 );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid expression, nothing to calculate." << std::endl;
				}
			}

			// komenda wy��czaj�ca program lub w�tek
			else if ( code_line.substr( 0, 4 ) == "kill" ) {
				lines.push_back( 6 );
				if ( code_line.substr( 4, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 3 ) == "if=" ) {
				lines.push_back( 7 );
				if ( code_line.substr( 4, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 4 ) == "if> " ) {
				lines.push_back( 8 );
				if ( code_line.substr( 5, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 4 ) == "if< " ) {
				lines.push_back( 9 );
				if ( code_line.substr( 5, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 4 ) == "if>=" ) {
				lines.push_back( 10 );
				if ( code_line.substr( 5, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 4 ) == "if<=" ) {
				lines.push_back( 11 );
				if ( code_line.substr( 5, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 3 ) == "if!" ) {
				lines.push_back( 12 );
				if ( code_line.substr( 4, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda p�tlowa
			else if ( code_line.substr( 0, 4 ) == "loop" ) {
				lines.push_back( 13 );
				if ( code_line.substr( 4, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda pobieraj�ca ci�g znak�w od u�ytkownika
			else if ( code_line.substr( 0, 9 ) == "getstring" ) {
				lines.push_back( 14 );
				if ( code_line.substr( 9, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 9, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda pobieraj�ca liczb� od u�ytkownika
			else if ( code_line.substr( 0, 10 ) == "getinteger" ) {
				lines.push_back( 15 );
				if ( code_line.substr( 10, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 10, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda skoku bezwarunkowego
			else if ( code_line.substr( 0, 4 ) == "jump" ) {
				lines.push_back( 16 );
				if ( code_line.substr( 4, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda kopiuj�ca kolejne liczby stosu
			else if ( code_line.substr( 0, 4 ) == "copy" ) {
				lines.push_back( 17 );
				if ( code_line.substr( 4, 11 ).size() > 1 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda wykonuj�ca obliczenia ( z kopi� stosu )
			else if ( code_line.substr( 0, 5 ) == "~calc" ) {
				lines.push_back( 18 );
				std::string* calcval = new std::string;
				*calcval = code_line.substr( 5, 256 );
				if ( calcval->size() > 1 ) {
					expressions.push_back( calcval );
					arguments.push_back( expressions.size() - 1 );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid expression, nothing to calculate." << std::endl;
				}
			}

			// komenda wrzucaj�ca ci�g znak�w na stos
			else if ( code_line.substr( 0, 6 ) == "string" ) {
				lines.push_back( 19 );
				std::string* strval = new std::string;
				*strval = code_line.substr( 7, 4096 );
				if ( strval->size() > 0 ) {
					expressions.push_back( strval );
					arguments.push_back( expressions.size() - 1 );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid expression, no string to push on stack." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 4 ) == "~if=" ) {
				lines.push_back( 20 );
				if ( code_line.substr( 4, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 5 ) == "~if> " ) {
				lines.push_back( 21 );
				if ( code_line.substr( 5, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 5 ) == "~if< " ) {
				lines.push_back( 22 );
				if ( code_line.substr( 5, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 5 ) == "~if>=" ) {
				lines.push_back( 23 );
				if ( code_line.substr( 5, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 5 ) == "~if<=" ) {
				lines.push_back( 24 );
				if ( code_line.substr( 5, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 5, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda por�wnuj�ca dwie liczby ze stosu
			else if ( code_line.substr( 0, 4 ) == "~if!" ) {
				lines.push_back( 25 );
				if ( code_line.substr( 4, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 4, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
			}

			// komenda generuj�ca losow� liczb�
			else if ( code_line.substr( 0, 6 ) == "random" ) {
				lines.push_back( 26 );
				if ( code_line.substr( 6, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 6, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda odwracaj�ca stos
			else if ( code_line.substr( 0, 7 ) == "reverse" ) {
				lines.push_back( 27 );
				if ( code_line.substr( 7, 11 ).size() > 0 ) {
					argument = std::stoi( code_line.substr( 7, 11 ) );
					arguments.push_back( argument );
				} else {
					arguments.push_back( -1 );
				}
			}

			// komenda zwracaj�ca wielko�� stosu
			else if ( code_line.substr( 0, 4 ) == "size" ) {
				lines.push_back( 28 );
				arguments.push_back( -1 );
			}

			// brak komendy
			else {
				//lines.push_back( 0 );
				//arguments.push_back( -1 );
			}
		}

		file.close();

		return true;
	}

	void interpreter::run_program() {
		srand( time( NULL ) );

		// dodatkowe informacje wy�wietlane w trybie debugowania
		if ( debug ) {
			uint_fast64_t size = lines.size();
			size += arguments.size() * 4;
			for ( int_fast64_t i( 0 ); i < expressions.size(); ++i ) 
				size += expressions[i]->size();

			std::cout << " Program total size: " << size << " bytes" << std::endl;
			system( "pause > nul" );
			system( "cls" );
		}
		while ( code_position < lines.size() ) {
			if ( debug ) std::cout << ":" << main_stack.size() * 4 << "B: ";

			// b��dna linijka - omijamy j�
			if ( lines[code_position] == 0 ) {
				code_position++;
			}

			// wrzucanie liczby na stos
			else if ( lines[code_position] == 1 ) {
				main_stack.push( arguments[code_position] );
				code_position++;
			}

			// wy�wietlanie stosu
			else if ( lines[code_position] == 2 ) {
				std::stack<int_fast32_t> copy_stack = main_stack;
				if ( arguments[code_position] != -1 ) {
					uint_fast64_t stack_index = 0;
					while ( !copy_stack.empty() && (stack_index < arguments[code_position]) ) {
						std::cout << copy_stack.top() << " ";
						copy_stack.pop();
						stack_index++;
					}
				} else {
					while ( !copy_stack.empty() ) {
						std::cout << copy_stack.top() << " ";
						copy_stack.pop();
					}
				}
				code_position++;
			}

			// wywalenie liczby ze stosu
			else if ( lines[code_position] == 3 ) {
				if ( !main_stack.empty() ) main_stack.pop();
				code_position++;
			}

			// wydrukowanie kodu ascii ze stosu
			else if ( lines[code_position] == 4 ) {
				if ( main_stack.size() > 1 ) {
					uint_fast64_t stack_index = 0;
					if ( arguments[code_position] != -1 ) {
						main_stack.pop();
						while ( !main_stack.empty() && (stack_index < arguments[code_position]) ) {
							std::cout << (char)main_stack.top();
							main_stack.pop();
							stack_index++;
						}
					} else {
						stack_index = main_stack.top();
						main_stack.pop();
						uint_fast64_t helper_value = 0;
						while ( !main_stack.empty() && (helper_value < stack_index) ) {
							std::cout << (char)main_stack.top();
							main_stack.pop();
							helper_value++;
						}
					}
				} else if ( debug ) {
					std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
				}
				code_position++;
			}

			// obliczanie wyra�enia w notacji PRN
			else if ( lines[code_position] == 5 ) {
				evaluate_prn( *expressions[arguments[code_position]] );
				code_position++;
			}

			// usuwanie niszczenie w�tku
			else if ( lines[code_position] == 6 ) {
				break;
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� r�wne
			else if ( lines[code_position] == 7 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();
					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB == registerA ) {
						code_position++;
						continue;
					}
					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest wi�ksza od m�odszej
			else if ( lines[code_position] == 8 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB > registerA ) {
						code_position++;
						continue;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest mniejsza od m�odszej
			else if ( lines[code_position] == 9 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB < registerA ) {
						code_position++;
						continue;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest wi�ksza lub r�wna od m�odszej
			else if ( lines[code_position] == 10 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB >= registerA ) {
						code_position++;
						continue;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest mniejsza lub r�wna od m�odszej
			else if ( lines[code_position] == 11 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB <= registerA ) {
						code_position++;
						continue;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest r�na od m�odszej
			else if ( lines[code_position] == 12 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB != registerA ) {
						code_position++;
						continue;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// zap�tla kod bloku dan� ilo�� razy
			else if ( lines[code_position] == 13 ) {
				if ( arguments[code_position] != -1 ) {
					if ( main_stack.size() > 0 ) {

						// ustawianie parametr�w p�tli
						if ( loop_amount == 0 ) {
							loop_amount = main_stack.top();
							main_stack.pop();
							loop_counter = 0;
							loop_code_position = code_position;
							loop_final_position = code_position + arguments[code_position];
						}

						// wychodzenie z p�tli
						else {
							if ( loop_counter == loop_amount ) {
								loop_amount = 0;
								code_position = loop_final_position;
							}
						}
					} else {
						if ( debug )
							std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever." << std::endl;
				}
				code_position++;
			}

			// pobiera tekst od u�ytkownika i wrzuca na stos
			else if ( lines[code_position] == 14 ) {
				std::string input;
				int_fast64_t length = 0;
				std::getline( std::cin, input );
				for ( int_fast64_t i( input.size() - 1 ); i >= 0; --i ) { //for ( int_fast8_t i : input ) {
					if ( input[i] != 0 ) {
						main_stack.push( int_fast32_t( input[i] ) );
						length++;
					}
				}
				main_stack.push( length );
				code_position++;
			}

			// pobiera liczb� od u�ytkownika i wrzuca na stos
			else if ( lines[code_position] == 15 ) {
				std::string input;
				std::getline( std::cin, input );
				main_stack.push( std::stoi( input ) );
				code_position++;
			}

			// wykonuje skok bezwarunkowy
			else if ( lines[code_position] == 16 ) {
				if ( arguments[code_position] > 0 ) {
					code_position = arguments[code_position] - 1;
				}
			}

			// kopiuje najm�odsz� liczb� stosu
			else if ( lines[code_position] == 17 ) {
				if ( arguments[code_position] != -1 ) {
					if ( arguments[code_position] > main_stack.size() ) arguments[code_position] = main_stack.size();

					std::stack<int_fast32_t> operation_stack = main_stack;
					std::stack<int_fast32_t> final_stack;

					for ( int_fast32_t i( 0 ); i < arguments[code_position]; ++i ) {
						final_stack.push( operation_stack.top() );
						operation_stack.pop();
					}
					while ( !final_stack.empty() ) {
						main_stack.push( final_stack.top() );
						final_stack.pop();
					}
				} else {
					main_stack.push( main_stack.top() );
				}
				code_position++;
			}

			// obliczanie wyra�enia w notacji PRN ( z kopi� stosu )
			else if ( lines[code_position] == 18 ) {
				main_stack.push( evaluate_prn_copy( *expressions[arguments[code_position]] ) );
				code_position++;
			}

			// wrzuca przygotowanego stringa na stos
			else if ( lines[code_position] == 19 ) {
				std::string operation_string = *expressions[arguments[code_position]];
				int_fast64_t length = 0;
				for ( int_fast64_t i( operation_string.size() ); i >= 0; --i ) { //for ( int_fast8_t i : operation_string ) {
					if ( operation_string[i] != 0 ) {
						if ( operation_string[i] == '~' ) { // znak nowej linii
							main_stack.push( 10 );
						} else {
							main_stack.push( int_fast32_t( operation_string[i] ) );
						}
						length++;
					}
				}
				main_stack.push( length );
				code_position++;
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� r�wne
			else if ( lines[code_position] == 20 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB == registerA ) {
						code_position++;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest wi�ksza od m�odszej
			else if ( lines[code_position] == 21 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB > registerA ) {
						code_position++;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest mniejsza od m�odszej
			else if ( lines[code_position] == 22 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB < registerA ) {
						code_position++;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest wi�ksza lub r�wna od m�odszej
			else if ( lines[code_position] == 23 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB >= registerA ) {
						code_position++;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest mniejsza lub r�wna od m�odszej
			else if ( lines[code_position] == 24 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB <= registerA ) {
						code_position++;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// por�wnuje dwie liczby ze stosu i sprawdza czy s� starsza jest r�na od m�odszej
			else if ( lines[code_position] == 25 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// kontynuujemy kod je�eli warunek jest spe�niony
					if ( registerB != registerA ) {
						code_position++;
					}

					// skaczemy o dan� ilo�� linijek, je�eli warunek nie jest spe�niony
					else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					code_position += arguments[code_position] + 1;
				}
			}

			// losuje liczb� ze wskazanego zakresu
			else if ( lines[code_position] == 26 ) {
				if ( arguments[code_position] != -1 ) {
					main_stack.push( rand() % arguments[code_position] );
				} else {
					if ( main_stack.size() > 0 ) {
						registerA = main_stack.top();
						main_stack.pop();
						main_stack.push( rand() % registerA );
					} else {
						main_stack.push( 0 );
						if ( debug )
							std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					}
				}
				code_position++;
			}

			// odwracanie stosu
			else if ( lines[code_position] == 27 ) {
				std::stack<int_fast32_t> copy_stack;
				uint_fast64_t max = main_stack.size();
				if ( arguments[code_position] != -1 ) {
					if ( arguments[code_position] < max ) max = arguments[code_position];
					std::stack<int_fast32_t> another_stack;
					for ( uint_fast64_t i( 0 ); i < max; ++i ) { // wydzielanie fragmentu stosu
						another_stack.push( main_stack.top() );
						main_stack.pop();
					}
					for ( uint_fast64_t i( 0 ); i < max; ++i ) { // odwracanie danego fragmentu stosu
						copy_stack.push( another_stack.top() );
						another_stack.pop();
					}
					for ( uint_fast64_t i( 0 ); i < max; ++i ) { // aktualizowanie g��wnego stosu
						main_stack.push( copy_stack.top() );
						copy_stack.pop();
					}
				} else {
					for ( uint_fast64_t i( 0 ); i < max; ++i ) {
						copy_stack.push( main_stack.top() );
						main_stack.pop();
					}
					main_stack = copy_stack;
				}
				code_position++;
			}

			// wielko�� stosu
			else if ( lines[code_position] == 28 ) {
				main_stack.push( main_stack.size() );
				code_position++;
			}

			// obs�ugiwanie p�tli
			if ( loop_amount != 0 ) {
				if ( code_position > loop_final_position ) {
					code_position = loop_code_position;
					loop_counter++;
				}
			}
		}
		system( "pause > nul" );
	}

	/*
		Ewaluacja wyra�enia z polskiej notacji wyk�adniczej
	*/
	int_fast32_t interpreter::evaluate_prn( std::string expression ) {
		std::string operation_string = "";
		uint_fast32_t operation_index = 0;
		int_fast32_t number_a = 0, number_b = 0;

		for ( uint_fast32_t i( 0 ); i < expression.size(); ++i ) {
			if ( (expression[i] == '+') || (expression[i] == '-') || (expression[i] == '*') || (expression[i] == '/') || (expression[i] == '%') ) {
				if ( main_stack.size() > 1 ) {
					number_a = main_stack.top();
					main_stack.pop();
					number_b = main_stack.top();
					main_stack.pop();

					if ( expression[i] == '+' ) {
						main_stack.push( number_b + number_a );
						continue;
					} else if ( expression[i] == '-' ) {
						main_stack.push( number_b - number_a );
						continue;
					} else if ( expression[i] == '*' ) {
						main_stack.push( number_b * number_a );
						continue;
					} else if ( expression[i] == '/' ) {
						main_stack.push( number_b / number_a );
						continue;
					} else if ( expression[i] == '%' ) {
						main_stack.push( number_b % number_a );
						continue;
					} else {
						continue;
					}
				} else {
					if ( debug ) {
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					}
					return 0;
				}
			} else {
				operation_index = i;
				while ( (expression[i] != ' ') && (i < expression.size() - 1) ) { i++; }
				operation_string = expression.substr( operation_index, i - operation_index + 1 );
				if ( operation_string[0] != ' ' ) main_stack.push( std::stod( operation_string, nullptr ) );
			}
		}
		return main_stack.top();
	}

	int_fast32_t interpreter::evaluate_prn_copy( std::string expression ) {
		std::string operation_string = "";
		uint_fast32_t operation_index = 0;
		int_fast32_t number_a = 0, number_b = 0;
		std::stack<int_fast32_t> operation_stack = main_stack;

		for ( uint_fast32_t i( 0 ); i < expression.size(); ++i ) {
			if ( (expression[i] == '+') || (expression[i] == '-') || (expression[i] == '*') || (expression[i] == '/') || (expression[i] == '%') ) {
				if ( operation_stack.size() > 1 ) {
					number_a = operation_stack.top();
					operation_stack.pop();
					number_b = operation_stack.top();
					operation_stack.pop();

					if ( expression[i] == '+' ) {
						operation_stack.push( number_b + number_a );
						continue;
					} else if ( expression[i] == '-' ) {
						operation_stack.push( number_b - number_a );
						continue;
					} else if ( expression[i] == '*' ) {
						operation_stack.push( number_b * number_a );
						continue;
					} else if ( expression[i] == '/' ) {
						operation_stack.push( number_b / number_a );
						continue;
					} else if ( expression[i] == '%' ) {
						operation_stack.push( number_b % number_a );
						continue;
					} else {
						continue;
					}
				} else {
					if ( debug ) {
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short." << std::endl;
					}
					return 0;
				}
			} else {
				operation_index = i;
				while ( (expression[i] != ' ') && (i < expression.size() - 1) ) { i++; }
				operation_string = expression.substr( operation_index, i - operation_index + 1 );
				if ( operation_string[0] != ' ' ) operation_stack.push( std::stod( operation_string, nullptr ) );
			}
		}
		return operation_stack.top();
	}
}