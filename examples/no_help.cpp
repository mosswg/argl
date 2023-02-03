#define ARGL_DISABLE_HELP
#define ARGL_IMPLEMENTATION
#include "../include/argl.h"
#include <iostream>

int main(int argc, char** argv) {
	long first;
	long second;
	argl::register_required_argument(first, "first");
	argl::register_required_argument(second, "second");

	argl::parse_arguments(argc, argv);

	std::cout << first << " * " << second << " = " << first * second << "\n";
}
