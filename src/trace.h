#ifndef TRACE_H
#define TRACE_H

#include "rassert.h"

#include <fmt/ostream.h>

#include <iostream>

#ifdef _WIN32
#define fdbg(FMT, ...) do { fmt::print(std::cout, FMT_STRING(FMT), __VA_ARGS__); std::cout << "\n" << std::flush; } while (false)
#define ferr(FMT, ...) do { fmt::print(std::cerr, FMT_STRING(FMT), __VA_ARGS__); std::cerr << "\n" << std::flush; } while (false)
#else
#define fdbg(FMT, ...) do { fmt::print(std::cout, FMT_STRING(FMT), ##__VA_ARGS__); std::cout << "\n" << std::flush; } while (false)
#define ferr(FMT, ...) do { fmt::print(std::cerr, FMT_STRING(FMT), ##__VA_ARGS__); std::cerr << "\n" << std::flush; } while (false)
#endif

#endif // TRACE_H
