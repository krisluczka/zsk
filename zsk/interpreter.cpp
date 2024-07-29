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
		std::cout << "ZSK prerelease 24.0, by Krzysztof Luczka\n";
		while ( true ) {
			std::cout << " >>  ";
			getline( std::cin, input );
			std::vector<std::string> tokens;
			std::istringstream iss( input );
			std::string token;

			// tokenization
			while ( iss >> token )
				tokens.push_back( token );

			// help
			if ( tokens[0] == "help" ) {
				std::cout << " - help           - displays the info about interpreter's commands\n";
				std::cout << " - load <path>    - loads the program\n";
				std::cout << " - run            - runs the program\n";
				std::cout << " - debug          - changes the interpreter's debug flag\n";
			}

			// load
			else if ( tokens[0] == "load" ) {
				if ( tokens.size() > 1 && load_program( tokens[1] ) )
					std::cout << " Program loaded correctly.\n";
				else
					std::cout << " Invalid path. \n";
			}

			// run
			else if ( tokens[0] == "run" ) {
				//system( "cls" );
				run_program();
				break;
			}

			// debug
			else if ( tokens[0] == "debug" ) {
				debug = !debug;
				if ( debug ) std::cout << " Debugging mode on. \n";
				else std::cout << " Debugging mode off. \n";
			}
		}
	}

	bool interpreter::load_program( std::string path ) {
		std::ifstream file( path );
		if ( !file.good() ) return false;
		std::string code_line;

		// loading the file into the code
		while ( std::getline( file, code_line ) ) {
			std::vector<std::string> tokens;
			std::istringstream iss( code_line );
			std::string token;

			// tokenization
			while ( iss >> token )
				tokens.push_back( token );

			// push - pushing the number on the stack
			if ( tokens[0] == "push" ) {
				lines.push_back( 1 );
				if ( tokens.size() > 1 ) {
					arguments.push_back( std::stoi( tokens[1] ) );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid argument, nothing to push on a stack. \n";
				}
			}

			// stack - printing the stack
			else if ( tokens[0] == "stack" ) {
				lines.push_back( 2 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// pop - popping the last value
			else if ( tokens[0] == "pop" ) {
				lines.push_back( 3 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// print - ascii characters printing
			else if ( tokens[0] == "print" ) {
				lines.push_back( 4 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// calc - evaluating an expression
			else if ( tokens[0] == "calc" ) {
				lines.push_back( 5 );
				std::string* calcval( new std::string );
				for ( uint_fast64_t i( 1 ); i < tokens.size(); ++i )
					*calcval += tokens[i] + " ";

				if ( calcval->size() > 1 ) {
					expressions.push_back( calcval );
					arguments.push_back( expressions.size() - 1 );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid expression, nothing to calculate.\n";
				}
			}

			// kill - terminating the code
			else if ( tokens[0] == "kill" ) {
				lines.push_back( 6 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// if= - comparing two stack numbers
			else if ( tokens[0] == "if=" ) {
				lines.push_back( 7 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// if> - comparing two stack numbers
			else if ( tokens[0] == "if> " ) {
				lines.push_back( 8 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// if< - comparing two stack numbers
			else if ( tokens[0] == "if<" ) {
				lines.push_back( 9 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// if>= - comparing two stack numbers
			else if ( tokens[0] == "if>=" ) {
				lines.push_back( 10 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// if<= - comparing two stack numbers
			else if ( tokens[0] == "if<=" ) {
				lines.push_back( 11 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// if! - comparing two stack numbers
			else if ( tokens[0] == "if!" ) {
				lines.push_back( 12 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// loop - loop command
			else if ( tokens[0] == "loop" ) {
				lines.push_back( 13 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// getstring - getting a string from user
			else if ( tokens[0] == "getstring" ) {
				lines.push_back( 14 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// getinteger - getting an integer from user
			else if ( tokens[0] == "getinteger" ) {
				lines.push_back( 15 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// jump - unconditional jump
			else if ( tokens[0] == "jump" ) {
				lines.push_back( 16 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// copy - copying given amount of stack numbers
			else if ( tokens[0] == "copy" ) {
				lines.push_back( 17 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// ~calc - evaluating an expression with stack copy
			else if ( tokens[0] == "~calc" ) {
				lines.push_back( 18 );
				std::string* calcval( new std::string );
				for ( uint_fast64_t i( 1 ); i < tokens.size(); ++i )
					*calcval += tokens[i] + " ";

				if ( calcval->size() > 1 ) {
					expressions.push_back( calcval );
					arguments.push_back( expressions.size() - 1 );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid expression, nothing to calculate.\n";
				}
			}

			// string - pushing ascii characters onto stack
			else if ( tokens[0] == "string" ) {
				lines.push_back( 19 );
				std::string* strval = new std::string;
				*strval = code_line.substr( 7, 4096 );
				if ( strval->size() > 0 ) {
					expressions.push_back( strval );
					arguments.push_back( expressions.size() - 1 );
				} else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid expression, no string to push on stack.\n";
				}
			}

			// ~if= - comparing two stack numbers based on stack's copy
			else if ( tokens[0] == "~if=" ) {
				lines.push_back( 20 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// ~if> - comparing two stack numbers based on stack's copy
			else if ( tokens[0] == "~if> " ) {
				lines.push_back( 21 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// ~if< - comparing two stack numbers based on stack's copy
			else if ( tokens[0] == "~if<" ) {
				lines.push_back( 22 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// ~if>= - comparing two stack numbers based on stack's copy
			else if ( tokens[0] == "~if>=" ) {
				lines.push_back( 23 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// ~if<= - comparing two stack numbers based on stack's copy
			else if ( tokens[0] == "~if<=" ) {
				lines.push_back( 24 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// ~if! - comparing two stack numbers based on stack's copy
			else if ( tokens[0] == "~if!" ) {
				lines.push_back( 25 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else {
					arguments.push_back( -1 );
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
			}

			// random - random number generator
			else if ( tokens[0] == "random" ) {
				lines.push_back( 26 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// reverse - reversing the stack
			else if ( tokens[0] == "reverse" ) {
				lines.push_back( 27 );
				if ( tokens.size() > 1 )
					arguments.push_back( std::stoi( tokens[1] ) );
				else
					arguments.push_back( -1 );
			}

			// size - returning stack size
			else if ( tokens[0] == "size" ) {
				lines.push_back( 28 );
				arguments.push_back( -1 );
			}

			// no command
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

		// additional info about debugging mode
		if ( debug ) {
			uint_fast64_t size = lines.size();
			size += arguments.size() * 4;
			for ( int_fast64_t i( 0 ); i < expressions.size(); ++i ) 
				size += expressions[i]->size();

			std::cout << " Program total size: " << size << " bytes\n";
		}
		while ( code_position < lines.size() ) {
			if ( debug ) std::cout << ":" << main_stack.size() * 4 << "B: ";

			// error line
			if ( lines[code_position] == 0 )
				++code_position;

			// pushing number onto stack
			else if ( lines[code_position] == 1 ) {
				main_stack.push( arguments[code_position] );
				++code_position;
			}

			// displaying the stack
			else if ( lines[code_position] == 2 ) {
				std::stack<int_fast32_t> copy_stack = main_stack;
				if ( arguments[code_position] != -1 ) {
					uint_fast64_t stack_index = 0;
					while ( !copy_stack.empty() && (stack_index < arguments[code_position]) ) {
						std::cout << copy_stack.top() << " ";
						copy_stack.pop();
						++stack_index;
					}
				} else {
					while ( !copy_stack.empty() ) {
						std::cout << copy_stack.top() << " ";
						copy_stack.pop();
					}
				}
				++code_position;
			}

			// poppin the number
			else if ( lines[code_position] == 3 ) {
				if ( !main_stack.empty() ) main_stack.pop();
				++code_position;
			}

			// printing ascii character
			else if ( lines[code_position] == 4 ) {
				if ( main_stack.size() > 1 ) {
					uint_fast64_t stack_index( 0 );
					if ( arguments[code_position] != -1 ) {
						main_stack.pop();
						while ( !main_stack.empty() && (stack_index < arguments[code_position]) ) {
							std::cout << (char)main_stack.top();
							main_stack.pop();
							++stack_index;
						}
					} else {
						stack_index = main_stack.top();
						main_stack.pop();
						uint_fast64_t helper_value( 0 );
						while ( !main_stack.empty() && (helper_value < stack_index) ) {
							std::cout << (char)main_stack.top();
							main_stack.pop();
							++helper_value;
						}
					}
				} else if ( debug ) {
					std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
				}
				++code_position;
			}

			// expression evaluation
			else if ( lines[code_position] == 5 ) {
				evaluate_prn( *expressions[arguments[code_position]] );
				code_position++;
			}

			// killswitch
			else if ( lines[code_position] == 6 ) {
				break;
			}

			// comparing two numbers
			else if ( lines[code_position] == 7 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// staying if the expression is true
					if ( registerB == registerA ) {
						++code_position;
						continue;
					}
					// jumping if the expression is false
					else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			else if ( lines[code_position] == 8 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB > registerA ) {
						++code_position;
						continue;
					} else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			else if ( lines[code_position] == 9 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB < registerA ) {
						++code_position;
						continue;
					} else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			else if ( lines[code_position] == 10 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB >= registerA ) {
						++code_position;
						continue;
					} else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			else if ( lines[code_position] == 11 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB <= registerA ) {
						++code_position;
						continue;
					} else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			else if ( lines[code_position] == 12 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB != registerA ) {
						++code_position;
						continue;
					} else {
						code_position += arguments[code_position] + 1;
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			// looping the code given amount of times
			else if ( lines[code_position] == 13 ) {
				if ( arguments[code_position] != -1 ) {
					if ( main_stack.size() > 0 ) {

						// the loop parameters
						if ( loop_amount == 0 ) {
							loop_amount = main_stack.top();
							main_stack.pop();
							loop_counter = 0;
							loop_code_position = code_position;
							loop_final_position = code_position + arguments[code_position];
						}

						// getting out
						else if ( loop_counter == loop_amount ) {
							loop_amount = 0;
							code_position = loop_final_position;
						}
					} else {
						if ( debug )
							std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					}
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid block length, this line will repeat forever.\n";
				}
				++code_position;
			}

			// string get
			else if ( lines[code_position] == 14 ) {
				std::string input;
				int_fast64_t length( 0 );
				std::getline( std::cin, input );
				for ( int_fast64_t i( input.size() - 1 ); i >= 0; --i ) {
					if ( input[i] != 0 ) {
						main_stack.push( int_fast32_t( input[i] ) );
						++length;
					}
				}
				main_stack.push( length );
				++code_position;
			}

			// number get
			else if ( lines[code_position] == 15 ) {
				std::string input;
				std::getline( std::cin, input );
				main_stack.push( std::stoi( input ) );
				++code_position;
			}

			// unconditional jump
			else if ( lines[code_position] == 16 ) {
				if ( arguments[code_position] > 0 ) {
					code_position = arguments[code_position] - 1;
				}
			}

			// copying given amount of numbers from stack
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

				++code_position;
			}

			// rpn with stack copy
			else if ( lines[code_position] == 18 ) {
				main_stack.push( evaluate_prn_copy( *expressions[arguments[code_position]] ) );
				++code_position;
			}

			// pushing string onto stack
			else if ( lines[code_position] == 19 ) {
				std::string operation_string = *expressions[arguments[code_position]];
				int_fast64_t length( 0 );
				for ( int_fast64_t i( operation_string.size() ); i >= 0; --i ) {
					if ( operation_string[i] != 0 ) {
						if ( operation_string[i] == '~' ) { // newline
							main_stack.push( 10 );
						} else {
							main_stack.push( int_fast32_t( operation_string[i] ) );
						}
						++length;
					}
				}
				main_stack.push( length );
				++code_position;
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
						++code_position;
					}
					
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

			// comparing two numbers with copy
			else if ( lines[code_position] == 21 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					// staying if the expression is true
					if ( registerB == registerA ) {
						++code_position;
						continue;
					}
					// jumping if the expression is false
					else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			else if ( lines[code_position] == 22 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB < registerA ) {
						++code_position;
					} else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			else if ( lines[code_position] == 23 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB >= registerA ) {
						++code_position;
					} else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			else if ( lines[code_position] == 24 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB <= registerA ) {
						++code_position;
					} else {
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

			else if ( lines[code_position] == 25 ) {
				if ( main_stack.size() > 1 ) {
					registerA = main_stack.top();
					main_stack.pop();
					registerB = main_stack.top();
					main_stack.pop();

					if ( registerB != registerA ) {
						++code_position;
					} else {
						code_position += arguments[code_position] + 1;
					}
					main_stack.push( registerB );
					main_stack.push( registerA );
				} else {
					if ( debug )
						std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					code_position += arguments[code_position] + 1;
				}
			}

			// randomizing a number
			else if ( lines[code_position] == 26 ) {
				if ( arguments[code_position] != -1 ) {
					main_stack.push( rand() % arguments[code_position] );
				} else {
					if ( main_stack.size() > 0 ) {
						registerA = main_stack.top();
						main_stack.pop();
						std::random_device rd;
						std::mt19937 mt( rd() );
						std::uniform_int_distribution<uint_fast64_t> random( 0, registerA );
						main_stack.push( random(mt) );
					} else {
						main_stack.push( 0 );
						if ( debug )
							std::cout << " line " << lines.size() << ": Invalid operation, stack is too short.\n";
					}
				}
				++code_position;
			}

			// reversing the stack
			else if ( lines[code_position] == 27 ) {
				std::stack<int_fast32_t> copy_stack;
				uint_fast64_t max = main_stack.size();
				if ( arguments[code_position] != -1 ) {
					if ( arguments[code_position] < max ) max = arguments[code_position];
					std::stack<int_fast32_t> another_stack;
					for ( uint_fast64_t i( 0 ); i < max; ++i ) { // portioning the stack
						another_stack.push( main_stack.top() );
						main_stack.pop();
					}
					for ( uint_fast64_t i( 0 ); i < max; ++i ) { // reversing given chunk
						copy_stack.push( another_stack.top() );
						another_stack.pop();
					}
					for ( uint_fast64_t i( 0 ); i < max; ++i ) { // pushing the updated chunk onto stack
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
				++code_position;
			}

			// stack size
			else if ( lines[code_position] == 28 ) {
				main_stack.push( main_stack.size() );
				++code_position;
			}

			// the loop
			if ( loop_amount != 0 ) {
				if ( code_position > loop_final_position ) {
					code_position = loop_code_position;
					++loop_counter;
				}
			}
		}
		system( "pause > nul" ); // ?
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
				while ( (expression[i] != ' ') && (i < expression.size() - 1) ) { ++i; }
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
				while ( (expression[i] != ' ') && (i < expression.size() - 1) ) { ++i; }
				operation_string = expression.substr( operation_index, i - operation_index + 1 );
				if ( operation_string[0] != ' ' ) operation_stack.push( std::stod( operation_string, nullptr ) );
			}
		}
		return operation_stack.top();
	}
}