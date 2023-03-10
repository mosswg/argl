#ifndef ARGL_H
#define ARGL_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {
	// char**
	argl_string,
	// std::string*
	argl_cppstring,
	// long*
	argl_long,
	// double*
	argl_double,
	// bool*
	argl_bool,
	// INVALID
	argl_INVALID
} argl_argument_type;

long argl_cstring_length(const char* string) {
	long len = 0;
	for (; string[len]; len++);
	return len;
}

bool argl_string_equal(const char* str1, const char* str2) {
	for (int index = 0; ; index++) {
		if (!str1[index] || !str2[index]) {
			// If they are both null then return true otherwise they are different lengths and therefore not equal.
			return (str1[index] == str2[index]);
		}

		if (str1[index] != str2[index]) {
			return false;
		}
	}

	// We should never get here but just in case.
	return true;
}

long argl_string_to_long(const char* string) {
	long output = 0;
	int negative = 1;

	int i = 0;
	if (string[0] == '-') {
		negative = -1;
		i++;
	}

	for (; string[i]; i++) {
		output *= 10;
		if (string[i] < '0' || string[i] > '9') {
			fprintf(stderr, "argl error: invalid character '%c' in integer: \"%s\"\n", string[i], string);
			return 0;
		}
		output += string[i] - '0';
	}

	return output * negative;
}

double argl_string_to_double(const char* string) {
	double whole_number_part = 0;
	double decimal_number_part = 0;
	double after_decimal_dividend = 1;
	double negative = 1;
	bool whole = true;

	int i = 0;
	if (string[0] == '-') {
		negative = -1;
		i++;
	}

	for (; string[i]; i++) {
		if (string[i] == '.') {
			if (!whole) {
				fprintf(stderr, "argl error: too many dots in double: \"%s\"\n", string);
			}
			whole = false;
			continue;
		}
		if (whole == true) {
			whole_number_part *= 10;
			if (string[i] < '0' || string[i] > '9') {
				fprintf(stderr, "argl error: invalid character '%c' in whole part of double: \"%s\"\n", string[i], string);
				return 0;
			}
			whole_number_part += string[i] - '0';
		}
		else {
			decimal_number_part *= 10;
			if (string[i] < '0' || string[i] > '9') {
				fprintf(stderr, "argl error: invalid character '%c' in decimal part of double: \"%s\"\n", string[i], string);
				return 0;
			}
			decimal_number_part += string[i] - '0';
			after_decimal_dividend *= 10;
		}
	}

	return whole_number_part + (decimal_number_part / ( after_decimal_dividend ));

}


#ifdef ARGL_BOOL_DEFAULT_TRUE
#define ARGL_BOOL_DEFAULT 1
#else
#define ARGL_BOOL_DEFAULT 0
#endif

#ifndef ARGL_STRING_DEFAULT
#define ARGL_STRING_DEFAULT ""
#endif

#ifndef ARGL_LONG_DEFAULT
#define ARGL_LONG_DEFAULT 0
#endif

#ifndef ARGL_DOUBLE_DEFAULT
#define ARGL_DOUBLE_DEFAULT 0
#endif


#ifndef ARGL_NO_C
// C mode (included with cpp by default)
#include <stdlib.h> // for exit function

typedef struct {
	void* value;
	argl_argument_type type;
	char* name;
	char* shorthand;
	bool set;
	bool required;
} argl_argument;

static char ARGL_EMPTY_STRING[1] = {0};
static int number_of_c_arguments = 0;
static argl_argument arguments[500];
#endif // no c

#ifdef __cplusplus
#include <string>
#include <vector>
#include <iostream>

namespace argl {
	class argument {
		public:
			void* value;
			argl_argument_type type;
			std::string name;
			std::string shorthand;
			std::string description;
			bool set = false;
			bool required = false;

			argument(char** value, const std::string& name, const std::string& shorthand, const std::string& description, char* default_value = 0) : name(name), shorthand(shorthand), description(description) {
				this->value = value;
				if (default_value) {
					for (int i = 0; default_value[i]; i++) {
						(*value)[i] = default_value[i];
					}
				}
				else {
					for (int i = 0; ARGL_STRING_DEFAULT[i]; i++) {
						(*value)[i] = ARGL_STRING_DEFAULT[i];
					}
				}
				this->type = argl_string;
			}

			argument(std::string* value, const std::string& name, const std::string& shorthand, const std::string& description, const std::string& default_value = ARGL_STRING_DEFAULT) : name(name), shorthand(shorthand), description(description) {
				this->value = value;
				*value = default_value;
				this->type = argl_string;
			}

			argument(long* value, const std::string& name, const std::string& shorthand, const std::string& description, long default_value = ARGL_LONG_DEFAULT) : name(name), shorthand(shorthand), description(description) {
				this->value = value;
				*value = default_value;
				this->type = argl_long;
			}

			argument(double* value, const std::string& name, const std::string& shorthand, const std::string& description, double default_value = ARGL_DOUBLE_DEFAULT) : name(name), shorthand(shorthand), description(description) {
				this->value = value;
				*value = default_value;
				this->type = argl_double;
			}

			argument(bool* value, const std::string& name, const std::string& shorthand, const std::string& description, bool default_value = ARGL_BOOL_DEFAULT) : name(name), shorthand(shorthand), description(description) {
				this->value = value;
				*value = default_value;
				this->type = argl_bool;
			}


			argument(char** value, const std::string& name) : name(name) {
				this->value = value;
				for (int i = 0; ARGL_STRING_DEFAULT[i]; i++) {
					(*value)[i] = ARGL_STRING_DEFAULT[i];
				}
				this->required = true;
				this->type = argl_string;
			}

			argument(std::string* value, const std::string& name) : name(name) {
				this->value = value;
				*value = ARGL_STRING_DEFAULT;
				this->required = true;
				this->type = argl_string;
			}

			argument(long* value, const std::string& name) : name(name) {
				this->value = value;
				*value = ARGL_LONG_DEFAULT;
				this->required = true;
				this->type = argl_long;
			}

			argument(double* value, const std::string& name) : name(name) {
				this->value = value;
				*value = ARGL_DOUBLE_DEFAULT;
				this->required = true;
				this->type = argl_double;
			}


			argument() {
				this->value = nullptr;
				this->type = argl_INVALID;
			}

			void print_value();

			std::string value_to_string();

			void set_value(std::string str);

#ifndef ARGL_NO_C
			argument operator=(argl_argument c_style_arg) {
				this->value = c_style_arg.value;
				this->type = c_style_arg.type;
				this->name = c_style_arg.name;
				this->shorthand = c_style_arg.shorthand;
				return *this;
			}
#endif
	};

	std::vector<argument> cpp_arguments;
	bool has_optional_argument = false;
	bool has_required_argument = false;
	long max_argument_length = 0;
	long max_line_width = 100;
}
#endif // cpp




#ifdef __cplusplus
namespace argl {
	template <class t>
		void register_required_argument(t& value, const std::string& name);

	template <class t>
		void register_argument(t& value, std::string name);

	template <class t>
		void register_argument(t& value, const std::string& name, t default_value);

	template <class t>
		void register_argument(t& value, const std::string& name, const std::string& description);

	template <class t>
		void register_argument(t& value, const std::string& name, const std::string& shorthand, const std::string& description);

	template <class t>
		void register_argument(t& value, const std::string& name, const std::string& shorthand, const std::string& description, t default_value);

	int parse_arguments(int argc, char** argv);

	void print_usage(int argc, char** argv);

	void print_argument_values();

#ifdef ARGL_IMPLEMENTATION

	template <class t>
		void register_required_argument(t& value, const std::string& name) {
			has_required_argument = true;
			cpp_arguments.emplace_back(&value, name);
		}

	template <class t>
		void register_argument(t& value, std::string name) {
			has_optional_argument = true;
			if ((name.length() + 2) > max_argument_length) {
				max_argument_length = name.length() + 2;
			}
			cpp_arguments.emplace_back(&value, name, "", "");
		}

	template <class t>
		void register_argument(t& value, const std::string& name, t default_value) {
			has_optional_argument = true;
			if ((name.length() + 2) > max_argument_length) {
				max_argument_length = name.length() + 2;
			}
			cpp_arguments.emplace_back(&value, name, "", "", default_value);
		}

	template <class t>
		void register_argument(t& value, const std::string& name, const std::string& description) {
			has_optional_argument = true;
			if ((name.length() + 2) > max_argument_length) {
				max_argument_length = name.length() + 2;
			}
			cpp_arguments.emplace_back(&value, name, "", description);
		}

	template <class t>
		void register_argument(t& value, const std::string& name, const std::string& shorthand, const std::string& description) {
			has_optional_argument = true;
			if ((name.length() + 2) + (shorthand.length() + 1) > max_argument_length) {
				max_argument_length = name.length() + 2 + shorthand.length() + 1;
			}
			cpp_arguments.emplace_back(&value, name, shorthand, description);
		}

	template <class t>
		void register_argument(t& value, const std::string& name, const std::string& shorthand, const std::string& description, t default_value) {
			has_optional_argument = true;
			if ((name.length() + 2) + (shorthand.length() + 1) > max_argument_length) {
				max_argument_length = name.length() + 2 + shorthand.length() + 1;
			}
			cpp_arguments.emplace_back(&value, name, shorthand, description, default_value);
		}


	/*
	 * Returns 0 on success and 1 on failure
	 */
	int parse_arguments(int argc, char** argv) {
		int return_value = 0;

		char* current_argv_value;
		std::string current_arg_name;
		bool current_arg_is_shorthand;
		std::string current_arg_value;
		argl::argument current_arg;

		/// Starting at 1 to skip ./<progam> or however the program is called.
		for (int i = 1; i < argc; i++) {
			current_argv_value = argv[i];



			/// This looks really weird but all it does is:
			/// If ARGL_DISABLE_HELP is defined: nothing
			/// If ARGL_DISABLE_HELP_SHORTHAND is defined but ARGL_DISABLE_HELP is not: check if argv[i] is "--help"
			/// If neither ARGL_DISABLE_HELP or ARGL_DISABLE_HELP_SHORTHAND is defined: check if argv[i] is "--help" or "-h"
#ifndef ARGL_DISABLE_HELP
			if (argl_string_equal(current_argv_value, "--help")
#ifndef ARGL_DISABLE_HELP_SHORTHAND
					|| argl_string_equal(current_argv_value, "-h")
#endif
			   ) {
				argl::print_usage(argc, argv);
				exit(0);
			}

#endif


			if (current_argv_value[0] != 0) {
				if (current_argv_value[0] == '-' && current_argv_value[1] != 0) {
					if (current_argv_value[1] == '-') {

						if (current_argv_value[2] == 0) {
							// Ignore any arguments that are just "--".
							continue;
						}

						// name is everything after the double dash
						current_arg_name = std::string(current_argv_value + 2);
					}
					else {
						// shorthand is everything after the dash
						current_arg_name = std::string(current_argv_value + 1);
						current_arg_is_shorthand = true;
					}

					current_arg.type = argl_INVALID;

#ifndef ARGL_NO_C
					for (int argument_index = 0; argument_index < number_of_c_arguments; argument_index++) {
						auto arg = arguments[argument_index];
						if (current_arg_is_shorthand) {
							if (argl_string_equal(arg.shorthand, current_arg_name.c_str())) {
								current_arg = arg;
								break;
							}
						}
						else {
							if (argl_string_equal(arg.name, current_arg_name.c_str())) {
								current_arg = arg;
								break;
							}
						}
					}
#endif

					for (auto& arg : cpp_arguments) {
						if (arg.required) {
							continue;
						}

						if (current_arg_is_shorthand) {
							if (arg.shorthand == current_arg_name) {
								current_arg = arg;
								break;
							}
						}
						else {
							if (arg.name == current_arg_name) {
								current_arg = arg;
								break;
							}
						}
					}

					if (current_arg.type == argl_INVALID) {
						continue;
					}

					if (current_arg.type == argl_bool) {
						*((bool*)current_arg.value) = !ARGL_BOOL_DEFAULT;
						continue;
					}

					// If this is the last argument
					if (argc == i + 1) {
						if (current_arg_is_shorthand) {
							std::cerr << "argl error: -" << current_arg.shorthand << " requires a value.\n";
						}
						else {
							std::cerr << "argl error --" << current_arg.name << " requires a value.\n";
						}

						return_value = 1;
					}
					current_arg_value = argv[++i];

					current_arg.set_value(current_arg_value);
				}
				else {
					// Arguments not prefixed with '--'. These do not suppert boolean arguments so we do not bother checking.

					current_arg.type = argl_INVALID;
#ifndef ARGL_NO_C
					for (int argument_index = 0; argument_index < number_of_c_arguments; argument_index++) {
						auto arg = arguments[argument_index];
						if (arg.required && !arg.set) {
								current_arg = arg;
								arg.set = true;
								break;
						}
					}
#endif

					for (auto& arg : cpp_arguments) {
						if (arg.required && !arg.set) {
							current_arg = arg;
							arg.set = true;
							break;
						}
					}

					if (current_arg.type == argl_INVALID) {
						fprintf(stderr, "argl error: unknown argument: %s\n", current_argv_value);
						exit(1);
					}

					current_arg.set_value(current_argv_value);
				}
			}
		}

		for (auto& arg : cpp_arguments) {
			if (arg.required && !arg.set) {
				argl::print_usage(argc, argv);
				exit(1);
			}
		}

		return return_value;
	}


	void print_usage(int argc, char** argv) {
		std::string usage = "Usage: ";
		usage += argv[0];

		if (has_optional_argument) {
			usage += " <options>";
		}

		if (has_required_argument) {
			usage += " ";
			for (auto& arg : cpp_arguments) {
				if (arg.required) {
					usage += arg.name + " ";
				}
			}
		}

		if (!has_optional_argument) {
			std::cout << usage << "\n";
			return;
		}

		if (has_required_argument) {
			usage += "<options>";
		}
		usage += "\nOptions:\n";

		for (auto& arg : cpp_arguments) {
			if (arg.required) {
				continue;
			}

			std::string line = "    --" + arg.name;
			if (!arg.shorthand.empty()) {
				line += "  -" + arg.shorthand;
			}
			if (!arg.description.empty()) {
				int number_of_spaces_on_first_line = (max_argument_length + 8) - line.size();
				int number_of_spaces = (max_argument_length + 12);
				int remaining_line_characters = max_line_width - number_of_spaces;

				/// First line of description
				for (int j = 0; j < number_of_spaces_on_first_line; j++) {
					line += ' ';
				}
				int last_split_index = arg.description.find(" ", remaining_line_characters);
				line += arg.description.substr(0, last_split_index) + '\n';

				while (last_split_index < arg.description.size()) {
					for (int j = 0; j < number_of_spaces; j++) {
						line += ' ';
					}

					int split_index = arg.description.find(" ", last_split_index + remaining_line_characters);
					line += arg.description.substr(last_split_index, split_index - last_split_index);
					line += '\n';
					last_split_index = split_index;
				}
			}
			if (line.size() > max_line_width) {
				std::vector<std::string> lines;
				for (int i = 0; i < line.size() / max_line_width; i++) {

				}
			}
			usage += line;
		}

		std::cout << usage;
	}

	void print_argument_values() {
		for (auto& arg : cpp_arguments) {
			std::cout << arg.name << ": ";
			arg.print_value();
			std::cout << "\n";
		}
	}

	void argument::print_value() {
		switch (type) {
			case argl_string:
				std::cout << (*((char**)this->value));
				break;
			case argl_cppstring:
				std::cout << *((std::string*)this->value);
				break;
			case argl_long:
				std::cout << *((long*)this->value);
				break;
			case argl_double:
				std::cout << *((double*)this->value);
				break;
			case argl_bool:
				std::cout << *((bool*)this->value);
				break;
			default:
				// Gracefully scream and yell then die
				fprintf(stderr, "argl internal error: Invalid argument type\n");
				exit(1);
		}
	}

	std::string argument::value_to_string() {
		std::string out;
		switch (type) {
			case argl_string:
				out = (*((char**)this->value));
				break;
			case argl_cppstring:
				out = *((std::string*)this->value);
				break;
			case argl_long:
				out = *((long*)this->value);
				break;
			case argl_double:
				out = *((double*)this->value);
				break;
			case argl_bool:
				out = *((bool*)this->value);
				break;
			default:
				// Gracefully scream and yell then die
				fprintf(stderr, "argl internal error: Invalid argument type\n");
				exit(1);
		}
		return out;
	}

	void argument::set_value(std::string str) {
		switch (type) {
			case argl_string:
				for (int i = 0; i < str.size(); i++) {
					(*((char**)this->value))[i] = str[i];
				}
				break;
			case argl_cppstring:
				*((std::string*)this->value) = str;
				break;
			case argl_long:
				*((long*)this->value) = argl_string_to_long(str.c_str());
				break;
			case argl_double:
				*((double*)this->value) = argl_string_to_double(str.c_str());
				break;
			case argl_bool:
				break;
			default:
				// Gracefully scream and yell then die
				fprintf(stderr, "argl internal error: Invalid argument type\n");
				exit(1);
		}
	}

#endif // argl implementation
} // End namespace
#endif // cpp
#ifndef ARGL_NO_C

void argl_register_string_argument(char** value, char* name);
void argl_register_string_argument_with_shorthand(char** value, char* name, char* shorthand);

void argl_register_long_argument(long* value, char* name);
void argl_register_long_argument_with_shorthand(long* value, char* name, char* shorthand);

void argl_register_double_argument(double* value, char* name);
void argl_register_double_argument_with_shorthand(double* value, char* name, char* shorthand);

void argl_register_bool_argument(bool* value, char* name);
void argl_register_bool_argument_with_shorthand(bool* value, char* name, char* shorthand);

int argl_parse_arguments(int argc, char** argv);

void argl_argument_set_value(argl_argument argument, char* string);


#ifdef ARGL_IMPLEMENTATION

void argl_register_string_argument(char** value, char* name) {
	// Get the pointer to the next available argument
	// Equivalent to &(arguments[number_of_c_arguments])
	argl_argument* arg = arguments + number_of_c_arguments;

	arg->value = value;
	arg->type = argl_string;
	arg->name = name;
	arg->shorthand = ARGL_EMPTY_STRING;
	arg->required = false;
	arg->set = false;

	number_of_c_arguments++;
}

void argl_register_string_argument_with_shorthand(char** value, char* name, char* shorthand) {
	// Get the pointer to the next available argument
	// Equivalent to &(arguments[number_of_c_arguments])
	argl_argument* arg = arguments + number_of_c_arguments;

	arg->value = value;
	arg->type = argl_string;
	arg->name = name;
	arg->shorthand = shorthand;
	arg->required = false;
	arg->set = false;

	number_of_c_arguments++;
}

void argl_register_long_argument(long* value, char* name) {
	// Get the pointer to the next available argument
	// Equivalent to &(arguments[number_of_c_arguments])
	argl_argument* arg = arguments + number_of_c_arguments;

	arg->value = value;
	arg->type = argl_long;
	arg->name = name;
	arg->shorthand = ARGL_EMPTY_STRING;
	arg->required = false;
	arg->set = false;

	number_of_c_arguments++;
}

void argl_register_long_argument_with_shorthand(long* value, char* name, char* shorthand) {
	// Get the pointer to the next available argument
	// Equivalent to &(arguments[number_of_c_arguments])
	argl_argument* arg = arguments + number_of_c_arguments;

	arg->value = value;
	arg->type = argl_long;
	arg->name = name;
	arg->shorthand = shorthand;
	arg->required = false;
	arg->set = false;

	number_of_c_arguments++;
}

void argl_register_double_argument(double* value, char* name) {
	// Get the pointer to the next available argument
	// Equivalent to &(arguments[number_of_c_arguments])
	argl_argument* arg = arguments + number_of_c_arguments;

	arg->value = value;
	arg->type = argl_double;
	arg->name = name;
	arg->shorthand = ARGL_EMPTY_STRING;
	arg->required = false;
	arg->set = false;

	number_of_c_arguments++;
}

void argl_register_double_argument_with_shorthand(double* value, char* name, char* shorthand) {
	// Get the pointer to the next available argument
	// Equivalent to &(arguments[number_of_c_arguments])
	argl_argument* arg = arguments + number_of_c_arguments;

	arg->value = value;
	arg->type = argl_double;
	arg->name = name;
	arg->shorthand = shorthand;
	arg->required = false;
	arg->set = false;

	number_of_c_arguments++;
}

void argl_register_bool_argument(bool* value, char* name) {
	// Get the pointer to the next available argument
	// Equivalent to &(arguments[number_of_c_arguments])
	argl_argument* arg = arguments + number_of_c_arguments;

	arg->value = value;
	arg->type = argl_bool;
	arg->name = name;
	arg->shorthand = ARGL_EMPTY_STRING;
	arg->required = false;
	arg->set = false;

	number_of_c_arguments++;
}

void argl_register_bool_argument_with_shorthand(bool* value, char* name, char* shorthand) {
	// Get the pointer to the next available argument
	// Equivalent to &(arguments[number_of_c_arguments])
	argl_argument* arg = arguments + number_of_c_arguments;

	arg->value = value;
	arg->type = argl_bool;
	arg->name = name;
	arg->shorthand = shorthand;
	arg->required = false;
	arg->set = false;

	number_of_c_arguments++;
}

void argl_argument_set_value(argl_argument argument, char* string) {
	switch (argument.type) {
		case argl_string:
			for (int i = 0; string[i]; i++) {
				(*((char**)argument.value))[i] = string[i];
			}
			break;
		case argl_long:
			*((long*)argument.value) = argl_string_to_long(string);
			break;
		case argl_double:
			*((double*)argument.value) = argl_string_to_double(string);
			break;
		case argl_bool:
			break;
		default:
			// Gracefully scream and yell then die
			fprintf(stderr, "argl internal error: Invalid argument type\n");
			exit(1);
	}
}

	/*
	 * Returns 0 on success and 1 on failure
	 */
int argl_parse_arguments(int argc, char** argv) {
	int return_value = 0;

	/// TODO: Rename these. this is terrible.
	char* current_arg;
	char* current_arg_name;
	bool current_arg_is_shorthand;
	char* current_arg_value;
	argl_argument current;
	for (int i = 0; i < argc; i++) {
		current_arg = argv[i];

		if (current_arg[0] != 0 && current_arg[1] != 0 && current_arg[0] == '-') {
			if (current_arg[1] == '-') {

				if (current_arg[2] == 0) {
					// Ignore any arguments that are just "--".
					continue;
				}

				// name is everything after the double dash
				current_arg_name = current_arg + 2;
			}
			else {
				// shorthand is everything after the dash
				current_arg_name = current_arg + 1;
				current_arg_is_shorthand = true;
			}

			current.type = argl_INVALID;
			for (int argument_index = 0; argument_index < number_of_c_arguments; argument_index++) {
				argl_argument arg = arguments[argument_index];
				if (current_arg_is_shorthand) {
					if (argl_string_equal(arg.shorthand, current_arg_name)) {
						current = arg;
						break;
					}
				}
				else {
					if (argl_string_equal(arg.name, current_arg_name)) {
						current = arg;
						break;
					}
				}
			}

			if (current.type == argl_INVALID) {
				continue;
			}

			if (current.type == argl_bool) {
				*((bool*)current.value) = !ARGL_BOOL_DEFAULT;
				continue;
			}

			// If this is the last argument
			if (argc == i + 1) {
				if (current_arg_is_shorthand) {
					fprintf(stderr, "argl error: -%s requires a value.\n", current.name);
				}
				else {
					fprintf(stderr, "argl error: --%s requires a value.\n", current.name);
				}

				return_value = 1;
			}
			current_arg_value = argv[++i];

			argl_argument_set_value(current, current_arg_value);
		}
		else {
			// Were only concerned with stuff that starts with '-' or '--'.
			continue;
		}
	}
	return return_value;
}

#endif // ARGL_IMPLEMENTATION
#endif // ARGL_NO_C



#endif // Header guard
