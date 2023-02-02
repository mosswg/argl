#define ARGL_IMPLEMENTATION
#include "../include/argl.h"
#include <stdio.h>

int main(int argc, char** argv) {
	double value;
	argl_register_double_argument_with_shorthand(&value, "value", "v");

	argl_parse_arguments(argc, argv);

	printf("%lf\n", value);
}
