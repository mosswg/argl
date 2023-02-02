#define ARGL_IMPLEMENTATION
#include "../include/argl.h"
#include <iostream>

int main(int argc, char** argv) {
	bool should_print;
	argl::register_argument(should_print, "print", "p");

	argl::parse_arguments(argc, argv);

	if (should_print) {
		std::cout << "I am printing because you told me to\n";
	}
}
