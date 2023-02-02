#define ARGL_IMPLEMENTATION
#include "../include/argl.h"
#include <stdio.h>

int main(int argc, char** argv) {
	bool should_print;
	argl_register_bool_argument_with_shorthand(&should_print, "print", "p");

	argl_parse_arguments(argc, argv);

	if (should_print) {
		printf("I am printing because you told me to\n");
	}
}
