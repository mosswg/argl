#define ARGL_IMPLEMENTATION
#define ARGL_NO_C
#include "../include/argl.h"
#include <iostream>

int main(int argc, char** argv) {
	long value;
	argl::register_argument(value, "value", "v");

	argl::parse_arguments(argc, argv);

	std::cout << value << "\n";
}
