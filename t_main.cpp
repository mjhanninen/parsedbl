#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include <mpfr.h>

#include "parsedbl.hpp"

class TestData
{

public:

    TestData()
        : data_(),
          words_()
    {

    }

    bool
    load(const std::string &path)
    {
        std::ifstream file(path, std::ios::ate);
        size_t size = file.tellg();
        file.seekg(std::ios::beg);
        auto buffer = std::unique_ptr<char[]>(new char[size]);
        file.read(buffer.get(), size);
        file.close();
        char *c = buffer.get();
        while (*c) {
            while (*c && isspace(*c)) {
                *c++ = '\0';
            }
            if (*c) words_.push_back(c);
            while (*c && !isspace(*c)) c++;
        }
        data_.push_back(std::move(buffer));
        return true;
    }

    typedef std::vector<char *>::const_iterator const_iterator;

    const_iterator inline
    begin() const
    {
        return words_.cbegin();
    }

    const_iterator inline
    end() const
    {
        return words_.cend();
    }

    const char *
    operator[](size_t ix)
    {
        return words_[ix];
    }

    size_t
    count() const
    {
        return words_.size();
    }

private:

    std::vector<std::unique_ptr<char[]>> data_;
    std::vector<char *> words_;
};

class Timer
{
    typedef std::chrono::high_resolution_clock clock_type;
    typedef std::chrono::time_point<clock_type> measurement_type;

    measurement_type start_;

public:
    typedef std::chrono::milliseconds msec_type;

    Timer() : start_(clock_type::now())
    {
    }

    void reset()
    {
        start_ = clock_type::now();
    }

    msec_type elapsed()
    {
        return std::chrono::duration_cast<msec_type>(
            clock_type::now() - start_);
    }
};

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " testfile" << std::endl;

        return 1;
    }

    TestData testData;
    testData.load(std::string(argv[1]));
    {
        double x_res = 0.0;
        double x_ref = 0.0;
        double err = 0.0;
        double max_err = 0.0;
        size_t bad_count = 0;
        for (auto &&s : testData) {
            x_ref = strtod(s, nullptr);
            parse_double(s, x_res);
            if (x_ref == 0.0) {

            }
            else {
                err = std::abs(x_res / x_ref - 1.0);
                if (err > max_err) {
                    mpfr_t x;
                    mpfr_init2(x, 1024);
                    mpfr_strtofr(x, s, nullptr, 10, MPFR_RNDN);
                    std::cout << "Error           = " << err << std::endl
                              << "Reference       = " << x_ref << std::endl
                              << "Reference(MPFR) = " << mpfr_get_d(x, MPFR_RNDN) << std::endl
                              << "Result          = " << x_res << std::endl
                              << "Input           = '" << s << "'" << std::endl
                              << std::endl;
                    max_err = err;
                    mpfr_clear(x);
                }
                if (err > 0.001) {
                    bad_count++;
                }
            }
        }
        std::cout << "There were total of " << bad_count << " really bad cases."
                  << std::endl;
    }

    size_t n = 0;
    double x = 0.0;
    char *stop;
    Timer timer;
    Timer::msec_type elapsed;
    for (size_t j = 5; j; j--) {
        n = 0;
        timer.reset();
        for (size_t i = 1; i; i--) {
            for (auto &&dbl_str : testData) {
                x = strtod(dbl_str, nullptr);
                n++;
            }
        }
        elapsed = timer.elapsed();
        std::cout << "Did " << n << " parses in " << elapsed.count()
                  << " milliseconds." << std::endl;
    }
    bool okay;
    for (size_t j = 5; j; j--) {
        n = 0;
        timer.reset();
        for (size_t i = 1; i; i--) {
            for (auto &&dbl_str : testData) {
                okay = parse_double(dbl_str, x);
                n++;
            }
        }
        elapsed = timer.elapsed();
        std::cout << "Did " << n << " parses in " << elapsed.count()
                  << " milliseconds." << std::endl;
    }
}
