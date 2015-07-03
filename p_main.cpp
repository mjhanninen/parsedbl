#include "parsedbl.hpp"

#include <iostream>
#include <iomanip>

int
main(int argc, char *argv[])
{
    double x;
    bool r;
    for (size_t i = 1; i < argc; i++) {
        r = parse_double(argv[i], x);
        std::cout << "ARG(" << i << ") = '" << argv[i]
                  << "' -> " << std::setprecision(15) << x
                  << " (" << (r ? "SUCCESS" : "FAILURE") << ")"
                  << std::endl;
    }
}
