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

			argument(char** value, std::string& name, std::string& shorthand, char* default_value = 0) : name(name), shorthand(shorthand) {
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

			argument(std::string* value, std::string& name, std::string& shorthand, const std::string& default_value = ARGL_STRING_DEFAULT) : name(name), shorthand(shorthand) {
				this->value = value;
				*value = default_value;
				this->type = argl_string;
			}

			argument(long* value, std::string& name, std::string& shorthand, long default_value = ARGL_LONG_DEFAULT) : name(name), shorthand(shorthand) {
				this->value = value;
				*value = default_value;
				this->type = argl_long;
			}

			argument(double* value, std::string& name, std::string& shorthand, double default_value = ARGL_DOUBLE_DEFAULT) : name(name), shorthand(shorthand) {
				this->value = value;
				*value = default_value;
				this->type = argl_double;
			}

			argument(bool* value, std::string& name, std::string& shorthand, bool default_value = ARGL_BOOL_DEFAULT) : name(name), shorthand(shorthand) {
				this->value = value;
				*value = default_value;
				this->type = argl_bool;
			}

			argument() {
				this->value = nullptr;
				this->type = argl_INVALID;
			}

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
}
#endif // cpp




#ifdef __cplusplus
namespace argl {
	template <class t>
		void register_argument(t* value, std::string name);

	template <class t>
		void register_argument(t* value, std::string name, std::string shorthand);

	template <class t>
		void register_argument(t* value, std::string name, t default_value);

	template <class t>
		void register_argument(t* value, std::string name, std::string shorthand, t default_value);

	int parse_arguments(int argc, char** argv);

#ifdef ARGL_IMPLEMENTATION

	template <class t>
		void register_argument(t& value, std::string name) {
			cpp_arguments.emplace_back(&value, name, "");
		}

	template <class t>
		void register_argument(t& value, std::string name, std::string shorthand) {
			cpp_arguments.emplace_back(&value, name, shorthand);
		}

	template <class t>
		void register_argument(t& value, std::string name, t default_value) {
			cpp_arguments.emplace_back(&value, name, "", default_value);
		}

	template <class t>
		void register_argument(t& value, std::string name, std::string shorthand, t default_value) {
			cpp_arguments.emplace_back(&value, name, shorthand, default_value);
		}


	/*
	 * Returns 0 on success and 1 on failure
	 */
	int parse_arguments(int argc, char** argv) {
		int return_value = 0;

		/// TODO: Rename these. this is terrible.
		char* current_arg;
		std::string current_arg_name;
		bool current_arg_is_shorthand;
		std::string current_arg_value;
		argl::argument current;
		for (int i = 0; i < argc; i++) {
			current_arg = argv[i];

			if (current_arg[0] != 0 && current_arg[1] != 0 && current_arg[0] == '-') {
				if (current_arg[1] == '-') {

					if (current_arg[2] == 0) {
						// Ignore any arguments that are just "--".
						continue;
					}

					// name is everything after the double dash
					current_arg_name = std::string(current_arg + 2);
				}
				else {
					// shorthand is everything after the dash
					current_arg_name = std::string(current_arg + 1);
					current_arg_is_shorthand = true;
				}

				current.type = argl_INVALID;

#ifndef ARGL_NO_C
				for (int argument_index = 0; argument_index < number_of_c_arguments; argument_index++) {
					auto arg = arguments[argument_index];
					if (current_arg_is_shorthand) {
						if (argl_string_equal(arg.shorthand, current_arg_name.c_str())) {
							current = arg;
							break;
						}
					}
					else {
						if (argl_string_equal(arg.name, current_arg_name.c_str())) {
							current = arg;
							break;
						}
					}
				}
#endif

				for (auto& arg : cpp_arguments) {
					if (current_arg_is_shorthand) {
						if (arg.shorthand == current_arg_name) {
							current = arg;
							break;
						}
					}
					else {
						if (arg.name == current_arg_name) {
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
						std::cerr << "argl error: -" << current.shorthand << " requires a value.\n";
					}
					else {
						std::cerr << "argl error --" << current.name << " requires a value.\n";
					}

					return_value = 1;
				}
				current_arg_value = argv[++i];

				current.set_value(current_arg_value);
			}
			else {
				// Were only concerned with stuff that starts with '-' or '--'.
				continue;
			}
		}
		return return_value;
	}


	void argl::argument::set_value(std::string str) {
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
