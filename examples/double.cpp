#define ARGL_IMPLEMENTATION
#include "../include/argl.h"
#include <cstdio>

int main(int argc, char** argv) {
	double value;
	argl::register_argument(value, "value", "v");

	argl::parse_arguments(argc, argv);

	printf("%lf\n", value);
}
