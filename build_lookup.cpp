#include <iomanip>
#include <iostream>

#include <mpfr.h>

#define SMALLEST_POWER_OF_TEN  (-323)
#define LARGEST_POWER_OF_TEN   (+308)
#define POWERS_OF_TEN_COUNT    (LARGEST_POWER_OF_TEN - SMALLEST_POWER_OF_TEN + 1)

#define INDEX_OF_ONE           (-SMALLEST_POWER_OF_TEN)

int
main()
{
    mpfr_t ten;
    mpfr_t mpfr_result;
    union {
        uint64_t u;
        double d;
    } result;
    mpfr_init2(ten, 256);
    mpfr_init2(mpfr_result, 256);
    mpfr_set_si(ten, 10, MPFR_RNDN);
    std::cout << "#define SMALLEST_POWER_OF_TEN  "
              << "(" << SMALLEST_POWER_OF_TEN << ")" << std::endl
              << "#define LARGEST_POWER_OF_TEN   "
              << "(" << LARGEST_POWER_OF_TEN << ")" << std::endl
              << "#define POWERS_OF_TEN_COUNT    "
              << "(LARGEST_POWER_OF_TEN - SMALLEST_POWER_OF_TEN + 1)" << std::endl
              << "#define INDEX_OF_ONE           "
              << "(-SMALLEST_POWER_OF_TEN)" << std::endl
              << std::endl
              << "const uint64_t u_powers_of_ten[POWERS_OF_TEN_COUNT] = {" << std::endl
              << std::hex << std::uppercase;
    for (long i = 0, j = SMALLEST_POWER_OF_TEN; j <= LARGEST_POWER_OF_TEN; i++, j++) {
        mpfr_pow_si(mpfr_result, ten, j, MPFR_RNDN);
        result.d = mpfr_get_d(mpfr_result, MPFR_RNDN);
        if (i % 4 == 0) {
            if (i > 0)
                std::cout << "," << std::endl;
            std::cout << "    ";
        }
        else {
            std::cout << ", ";
        }
        std::cout << "0x" << std::setw(16) << std::setfill('0') << result.u;
    }
    std::cout << std::dec << std::nouppercase << std::endl
              << "};" << std::endl
              << std::endl
              << "const double *powers_of_ten = "
              << "reinterpret_cast<const double *>(u_powers_of_ten);" << std::endl;
    mpfr_clear(mpfr_result);
    mpfr_clear(ten);
}
